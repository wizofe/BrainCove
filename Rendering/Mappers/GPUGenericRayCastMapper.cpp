/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkVolumeRayCastMapper.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "GPUGenericRayCastMapper.h"

#include "vtkCamera.h"
#include "vtkDataArray.h"
#include "vtkEncodedGradientEstimator.h"
#include "vtkEncodedGradientShader.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkGarbageCollector.h"
#include "vtkGraphicsFactory.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlaneCollection.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"
#include "vtkTransform.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastFunction.h"
#include "vtkRayCastImageDisplayHelper.h"

#include <math.h>
#include <time.h>

//vtkStandardNewMacro(GPUGenericRayCastMapper);

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

#define vtkVRCMultiplyViewPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; }

#define vtkVRCMultiplyNormalMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[4]  + A[2]*M[8]; \
	B[1] = A[0]*M[1]  + A[1]*M[5]  + A[2]*M[9]; \
	B[2] = A[0]*M[2]  + A[1]*M[6]  + A[2]*M[10]

// Construct a new vtkVolumeRayCastMapper with default values
GPUGenericRayCastMapper::GPUGenericRayCastMapper()
{
	this->ImageSampleDistance        =  1.0;
	this->MinimumImageSampleDistance =  1.0;
	this->MaximumImageSampleDistance = 10.0;

	this->PerspectiveMatrix      = vtkMatrix4x4::New();
	this->ViewToWorldMatrix      = vtkMatrix4x4::New();
	this->ViewToVoxelsMatrix     = vtkMatrix4x4::New();
	this->VoxelsToViewMatrix     = vtkMatrix4x4::New();
	this->WorldToVoxelsMatrix    = vtkMatrix4x4::New();
	this->VoxelsToWorldMatrix    = vtkMatrix4x4::New();
	this->WorldToViewMatrix		 = vtkMatrix4x4::New();

	this->VolumeMatrix           = vtkMatrix4x4::New();

	this->PerspectiveTransform   = vtkTransform::New();
	this->VoxelsTransform        = vtkTransform::New();
	this->VoxelsToViewTransform  = vtkTransform::New();
	this->WorldToViewTransform  = vtkTransform::New();

	this->ImageMemorySize[0]     = 0;
	this->ImageMemorySize[1]     = 0;

	this->RowBounds              = NULL;
	this->OldRowBounds           = NULL;

	this->ZBuffer                = NULL;
	this->ZBufferSize[0]         = 0;
	this->ZBufferSize[1]         = 0;
	this->ZBufferOrigin[0]       = 0;
	this->ZBufferOrigin[1]       = 0;

	this->Painter				 = NULL;
	this->RayCaster   		     = NULL;
	this->Texture				 = NULL;
	this->Layout				 = NULL;

	this->Selection				  = NULL;
	this->SelectionChanged		  = false;

	this->MatrixFile			  = NULL;
	this->CompareMatrixFile		  = NULL;

	this->IsComparing			  = false;
}

// Destruct a vtkVolumeRayCastMapper - clean up any memory used
GPUGenericRayCastMapper::~GPUGenericRayCastMapper()
{
	this->PerspectiveMatrix->Delete();
	this->ViewToWorldMatrix->Delete();
	this->WorldToViewMatrix->Delete();
	this->ViewToVoxelsMatrix->Delete();
	this->VoxelsToViewMatrix->Delete();
	this->WorldToVoxelsMatrix->Delete();
	this->VoxelsToWorldMatrix->Delete();

	this->VolumeMatrix->Delete();

	this->VoxelsTransform->Delete();
	this->VoxelsToViewTransform->Delete();
	this->WorldToViewTransform->Delete();
	this->PerspectiveTransform->Delete();

	if ( this->RowBounds )
	{
		delete [] this->RowBounds;
		delete [] this->OldRowBounds;
	}

	delete this->RayCaster;
}

void GPUGenericRayCastMapper::ReleaseGraphicsResources(vtkWindow *)
{
}

int* GPUGenericRayCastMapper::GetViewBounds()
{
	int *bounds = new int[4];
	bounds[0] = this->ImageOrigin[0];
	bounds[1] = this->ImageOrigin[0]+this->ImageInUseSize[0];
	bounds[2] = this->ImageOrigin[1];
	bounds[3] = this->ImageOrigin[1]+this->ImageInUseSize[1];
	return bounds;
}

void GPUGenericRayCastMapper::SetRayCaster(CLRayCaster* rayCaster)
{
	this->RayCaster = rayCaster;
	this->Modified();
}

void GPUGenericRayCastMapper::SetComparisonData( DataFile *file)
{ 
	this->CompareMatrixFile = file; 
	this->RayCaster->SetComparisonDataFile(this->CompareMatrixFile);
}

void GPUGenericRayCastMapper::Render( vtkRenderer *ren, vtkVolume *vol )
{
	//printf("Rendering...\n");
	//make sure the raycaster has the latest data
	UpdateRaycaster(vol);
	//UpdatePicker(vol);

	// make sure that we have scalar input and update the scalar input
	if ( this->GetInput() == NULL ) 
	{
		vtkErrorMacro(<< "No Input!");
		return;
	}
	if ( this->MatrixFile == NULL )
	{
		vtkErrorMacro(<< "No Connectivity Matrix!");
		return;
	}
	if ( vol == NULL)
	{
		vtkErrorMacro(<< "No volume!");
		return;
	}

	if ( this->GetRayCaster() == NULL)
	{
		vtkErrorMacro(<< "No raycaster!");
		return;
	}

	if ( this->IsComparing && !this->CompareMatrixFile)
	{
		vtkErrorMacro(<< "Visualization mode is set to comparison, but no Compare Matrix File is given!");
		return;
	}

	// Get the camera from the renderer
	vtkCamera *cam = ren->GetActiveCamera();

	// Get the aspect ratio from the renderer. This is needed for the
	// computation of the perspective matrix
	ren->ComputeAspect();
	double *aspect = ren->GetAspect();

	// Keep track of the projection matrix - we'll need it in a couple of
	// places Get the projection matrix. The method is called perspective, but
	// the matrix is valid for perspective and parallel viewing transforms.
	// Don't replace this with the GetCompositePerspectiveTransformMatrix
	// because that turns off stereo rendering!!!
	this->PerspectiveTransform->Identity();
	this->PerspectiveTransform->Concatenate(
		cam->GetProjectionTransformMatrix(aspect[0]/aspect[1],0.0, 1.0 ));
	this->PerspectiveTransform->Concatenate(cam->GetViewTransformMatrix());
	this->PerspectiveMatrix->DeepCopy(this->PerspectiveTransform->GetMatrix());

	// Compute some matrices from voxels to view and vice versa based 
	// on the whole input
	this->ComputeMatrices( this->GetInput(), vol );

	// How big is the viewport in pixels?
	double *viewport   =  ren->GetViewport();
	int *renWinSize   =  ren->GetRenderWindow()->GetSize();

	// Save this so that we can restore it if the image is cancelled
	double oldImageSampleDistance = this->ImageSampleDistance;

	// The full image fills the viewport. First, compute the actual viewport
	// size, then divide by the ImageSampleDistance to find the full image
	// size in pixels
	int width, height;
	ren->GetTiledSize(&width, &height);
	this->ImageViewportSize[0] = 
		static_cast<int>(width/this->ImageSampleDistance);
	this->ImageViewportSize[1] = 
		static_cast<int>(height/this->ImageSampleDistance);

	//printf("%d,%d\n",width,height);

	// Compute row bounds. This will also compute the size of the texture to
	// render, allocate the space if necessary, and clear the texture where
	// required
	if ( this->ComputeRowBounds( vol, ren ) )
	{
		//Initialize OPENCL kernel
		//Render to texture!
		if(this->Filters.size() == 0)
			this->Filters.push_back(this->EmptyFilter);

		RayCaster->CastRays(ren,Texture,ViewToWorldMatrix,this->ImageOrigin,this->ImageViewportSize,cam->GetThickness(), this->RowBounds, this->Selection, this->Filters); 

		// Do we need to capture the z buffer to intermix intersecting
		// geometry? If so, do it here*/
		if ( ren->GetNumberOfPropsRendered() )
		{
			int x1, x2, y1, y2;

			// turn this->ImageOrigin into (x1,y1) in window (not viewport!)
			// coordinates. 
			x1 = static_cast<int> (
				viewport[0] * static_cast<double>(renWinSize[0]) +
				static_cast<double>(this->ImageOrigin[0]) * this->ImageSampleDistance );
			y1 = static_cast<int> (
				viewport[1] * static_cast<double>(renWinSize[1]) +
				static_cast<double>(this->ImageOrigin[1]) * this->ImageSampleDistance);

			// compute z buffer size
			this->ZBufferSize[0] = static_cast<int>(
				static_cast<double>(this->ImageInUseSize[0]) * this->ImageSampleDistance);
			this->ZBufferSize[1] = static_cast<int>(
				static_cast<double>(this->ImageInUseSize[1]) * this->ImageSampleDistance);

			// Use the size to compute (x2,y2) in window coordinates
			x2 = x1 + this->ZBufferSize[0] - 1;
			y2 = y1 + this->ZBufferSize[1] - 1;

			// This is the z buffer origin (in viewport coordinates)
			this->ZBufferOrigin[0] = static_cast<int>(
				static_cast<double>(this->ImageOrigin[0]) * this->ImageSampleDistance);
			this->ZBufferOrigin[1] = static_cast<int>(
				static_cast<double>(this->ImageOrigin[1]) * this->ImageSampleDistance);

			// Capture the z buffer
			this->ZBuffer = ren->GetRenderWindow()->GetZbufferData(x1,y1,x2,y2);
		}

		if ( !ren->GetRenderWindow()->GetAbortRender() )
		{
			float depth = this->MinimumViewDistance;

			//Draw texture to screen!
			if(!Painter)
				Painter = new CLTexturePainter();

			Painter->Paint(vol,ren,Texture,ImageViewportSize, ImageOrigin, depth);

			this->Timer->StopTimer();
			this->TimeToDraw = this->Timer->GetElapsedTime();
		}
		// Restore the image sample distance so that automatic adjustment
		// will work correctly
		else
		{
			this->ImageSampleDistance = oldImageSampleDistance;
		}

		if ( this->ZBuffer )
		{
			delete [] this->ZBuffer;
			this->ZBuffer = NULL;
		}
	}  
	/*
	if(this->SelectionChanged)
	{
		this->SelectionChanged = false;
		this->NotifyObservers();
	}
	*/
}

/*
void GPUGenericRayCastMapper::NotifyObservers()
{
	for(int i = 0;i < this->Observers.size(); i++)
	{
		this->Observers.at(i)->SetSelection(this->GetSelection());
	}
}
*/
double GPUGenericRayCastMapper::GetZBufferValue(int x, int y)
{
	int xPos, yPos;

	xPos = static_cast<int>(static_cast<float>(x) * this->ImageSampleDistance);
	yPos = static_cast<int>(static_cast<float>(y) * this->ImageSampleDistance);

	xPos = (xPos >= this->ZBufferSize[0])?(this->ZBufferSize[0]-1):(xPos);
	yPos = (yPos >= this->ZBufferSize[1])?(this->ZBufferSize[1]-1):(yPos);

	return *(this->ZBuffer + yPos*this->ZBufferSize[0] + xPos);
}

void GPUGenericRayCastMapper::NormalizeCoordinates(int x, int y, float z, float w, int *viewPortSize, float outCoord[4])
{
	float offsetX = 1.0f / viewPortSize[0];
	float offsetY = 1.0f / viewPortSize[1];

	float nx = ((float)x) / (float)(viewPortSize[0]) * 2.0f - 1.0f + offsetX;
	float ny = ((float)y) / (float)(viewPortSize[1]) * 2.0f - 1.0f + offsetY;

	outCoord[0] = nx;
	outCoord[1] = ny;
	outCoord[2] = z;
	outCoord[3] = w;
}

int GPUGenericRayCastMapper::ComputeRowBounds(vtkVolume   *vol,
											vtkRenderer *ren)
{
	float voxelPoint[3];
	float viewPoint[8][4];
	int i, j, k;
	float minX, minY, maxX, maxY, minZ, maxZ;

	minX =  1.0;
	minY =  1.0;
	maxX = -1.0;
	maxY = -1.0;
	minZ =  1.0;
	maxZ =  0.0;

	float bounds[6];
	int dim[3];

	this->GetInput()->GetDimensions(dim);
	bounds[0] = bounds[2] = bounds[4] = 0.0;
	// The rounding tie-breaker is used to protect against a very small rounding error
	// introduced in the QuickFloor function, which is used by the vtkFloorFuncMacro macro.
	bounds[1] = static_cast<float>(dim[0]-1) - vtkFastNumericConversion::RoundingTieBreaker();
	bounds[3] = static_cast<float>(dim[1]-1) - vtkFastNumericConversion::RoundingTieBreaker();
	bounds[5] = static_cast<float>(dim[2]-1) - vtkFastNumericConversion::RoundingTieBreaker();

	double camPos[3];
	double worldBounds[6];
	vol->GetBounds( worldBounds );
	int insideFlag = 0;
	ren->GetActiveCamera()->GetPosition( camPos );
	if ( camPos[0] >= worldBounds[0] &&
		camPos[0] <= worldBounds[1] &&
		camPos[1] >= worldBounds[2] &&
		camPos[1] <= worldBounds[3] &&
		camPos[2] >= worldBounds[4] &&
		camPos[2] <= worldBounds[5] )
	{
		insideFlag = 1;
	}

	// Copy the voxelsToView matrix to 16 floats
	float voxelsToViewMatrix[16];
	for ( j = 0; j < 4; j++ )
	{
		for ( i = 0; i < 4; i++ )
		{
			voxelsToViewMatrix[j*4+i] = 
				static_cast<float>(this->VoxelsToViewMatrix->GetElement(j,i));
		}
	}

	// Convert the voxel bounds to view coordinates to find out the
	// size and location of the image we need to generate. 
	int idx = 0;
	if ( insideFlag )
	{
		minX = -1.0;
		maxX =  1.0;
		minY = -1.0;
		maxY =  1.0;
		minZ =  0.001;
		maxZ =  0.001;
	}
	else
	{
		for ( k = 0; k < 2; k++ )
		{
			voxelPoint[2] = bounds[4+k];
			for ( j = 0; j < 2; j++ )
			{
				voxelPoint[1] = bounds[2+j];
				for ( i = 0; i < 2; i++ )
				{
					voxelPoint[0] = bounds[i];
					vtkVRCMultiplyPointMacro( voxelPoint, viewPoint[idx],
						voxelsToViewMatrix );

					minX = (viewPoint[idx][0]<minX)?(viewPoint[idx][0]):(minX);
					minY = (viewPoint[idx][1]<minY)?(viewPoint[idx][1]):(minY);        
					maxX = (viewPoint[idx][0]>maxX)?(viewPoint[idx][0]):(maxX);
					maxY = (viewPoint[idx][1]>maxY)?(viewPoint[idx][1]):(maxY);
					minZ = (viewPoint[idx][2]<minZ)?(viewPoint[idx][2]):(minZ);
					maxZ = (viewPoint[idx][2]>maxZ)?(viewPoint[idx][2]):(maxZ);
					idx++;
				}
			}
		}
	}

	if ( minZ < 0.001 || maxZ > 0.9999 )
	{
		minX = -1.0;
		maxX =  1.0;
		minY = -1.0;
		maxY =  1.0;
		insideFlag = 1;
	}

	this->MinimumViewDistance = 
		(minZ<0.001)?(0.001):((minZ>0.999)?(0.999):(minZ));

	// We have min/max values from -1.0 to 1.0 now - we want to convert 
	// these to pixel locations. Give a couple of pixels of breathing room
	// on each side if possible
	minX = ( minX + 1.0 ) * 0.5 * this->ImageViewportSize[0] - 2; 
	minY = ( minY + 1.0 ) * 0.5 * this->ImageViewportSize[1] - 2; 
	maxX = ( maxX + 1.0 ) * 0.5 * this->ImageViewportSize[0] + 2; 
	maxY = ( maxY + 1.0 ) * 0.5 * this->ImageViewportSize[1] + 2;

	// If we are outside the view frustum return 0 - there is no need
	// to render anything
	if ( ( minX < 0 && maxX < 0 ) ||
		( minY < 0 && maxY < 0 ) ||
		( minX > this->ImageViewportSize[0]-1 &&
		maxX > this->ImageViewportSize[0]-1 ) ||
		( minY > this->ImageViewportSize[1]-1 &&
		maxY > this->ImageViewportSize[1]-1 ) )
	{
		return 0;
	}

	int oldImageMemorySize[2];
	oldImageMemorySize[0] = this->ImageMemorySize[0];
	oldImageMemorySize[1] = this->ImageMemorySize[1];

	// Swap the row bounds
	int *tmpptr;
	tmpptr = this->RowBounds;
	this->RowBounds = this->OldRowBounds;
	this->OldRowBounds = tmpptr;


	// Check the bounds - the volume might project outside of the 
	// viewing box / frustum so clip it if necessary
	minX = (minX<0)?(0):(minX);
	minY = (minY<0)?(0):(minY);
	maxX = (maxX>this->ImageViewportSize[0]-1)?
		(this->ImageViewportSize[0]-1):(maxX);
	maxY = (maxY>this->ImageViewportSize[1]-1)?
		(this->ImageViewportSize[1]-1):(maxY);

	// Create the new image, and set its size and position
	this->ImageInUseSize[0] = static_cast<int>(maxX - minX + 1.0);
	this->ImageInUseSize[1] = static_cast<int>(maxY - minY + 1.0);

	this->ImageMemorySize[0] = CLMemoryObject::Pow2Roundup(this->ImageInUseSize[0]);//TextureWidth();
	this->ImageMemorySize[1] = CLMemoryObject::Pow2Roundup(this->ImageInUseSize[1]);//TextureHeight();

	this->ImageOrigin[0] = static_cast<int>(minX);
	this->ImageOrigin[1] = static_cast<int>(minY);

	// If the old image size is much too big (more than twice in
	// either direction) then set the old width to 0 which will
	// cause the image to be recreated
	if ( oldImageMemorySize[0] > 2*this->ImageMemorySize[0] ||
		oldImageMemorySize[1] > 2*this->ImageMemorySize[1] )
	{
		oldImageMemorySize[0] = 0;
	}

	// If the old image is big enough (but not too big - we handled
	// that above) then we'll bump up our required size to the
	// previous one. This will keep us from thrashing.
	if ( oldImageMemorySize[0] >= this->ImageMemorySize[0] &&
		oldImageMemorySize[1] >= this->ImageMemorySize[1] )
	{
		this->ImageMemorySize[0] = oldImageMemorySize[0];
		this->ImageMemorySize[1] = oldImageMemorySize[1];
	}

	// Do we already have a texture big enough? If not, create a new one and
	// clear it.
	if (this->ImageMemorySize[0] > oldImageMemorySize[0] ||
		this->ImageMemorySize[1] > oldImageMemorySize[1] )
	{
		// Create the row bounds array. This will store the start / stop pixel
		// for each row. This helps eleminate work in areas outside the bounding
		// hexahedron since a bounding box is not very tight. We keep the old ones
		// too to help with only clearing where required
		if(this->RowBounds)
			delete[] this->RowBounds;
		if(this->OldRowBounds)
			delete[] this->OldRowBounds;

		this->RowBounds = new int [2*this->ImageMemorySize[1]];
		this->OldRowBounds = new int [2*this->ImageMemorySize[1]];

		for ( i = 0; i < this->ImageMemorySize[1]; i++ )
		{
			this->RowBounds[i*2]      = this->ImageMemorySize[0];
			this->RowBounds[i*2+1]    = -1;
			this->OldRowBounds[i*2]   = this->ImageMemorySize[0];
			this->OldRowBounds[i*2+1] = -1;
		}

	}

	if (!this->Texture )
		this->Texture = CLTexture2DObject::New(*CLContext::Instance().Context(),CLMemoryObject::RGBA_UCHAR,this->ImageInUseSize[0],this->ImageInUseSize[1],0);
	else
	{
		this->Texture->Resize(this->ImageInUseSize[0],this->ImageInUseSize[1]);
		//printf("Resizing to %d,%d\n",this->ImageInUseSize[0],this->ImageInUseSize[1]);
	}

	// If we are inside the volume our row bounds indicate every ray must be
	// cast - we don't need to intersect with the 12 lines
	if ( insideFlag )
	{
		for ( j = 0; j < this->ImageInUseSize[1]; j++ )
		{
			this->RowBounds[j*2] = 0;
			this->RowBounds[j*2+1] = this->ImageInUseSize[0] - 1;
		}
	}
	else
	{
		// create an array of lines where the y value of the first vertex is less
		// than or equal to the y value of the second vertex. There are 12 lines,
		// each containing x1, y1, x2, y2 values.
		float lines[12][4];
		float x1, y1, x2, y2;
		int xlow, xhigh;
		int lineIndex[12][2] = {{0,1}, {2,3}, {4,5}, {6,7}, 
		{0,2}, {1,3} ,{4,6}, {5,7},
		{0,4}, {1,5}, {2,6}, {3,7}};

		for ( i = 0; i < 12; i++ )
		{
			x1 = (viewPoint[lineIndex[i][0]][0]+1.0) * 
				0.5*this->ImageViewportSize[0] - this->ImageOrigin[0];

			y1 = (viewPoint[lineIndex[i][0]][1]+1.0) * 
				0.5*this->ImageViewportSize[1] - this->ImageOrigin[1];

			x2 = (viewPoint[lineIndex[i][1]][0]+1.0) * 
				0.5*this->ImageViewportSize[0] - this->ImageOrigin[0];

			y2 = (viewPoint[lineIndex[i][1]][1]+1.0) * 
				0.5*this->ImageViewportSize[1] - this->ImageOrigin[1];

			if ( y1 < y2 )
			{
				lines[i][0] = x1;
				lines[i][1] = y1;
				lines[i][2] = x2;
				lines[i][3] = y2;
			}
			else
			{
				lines[i][0] = x2;
				lines[i][1] = y2;
				lines[i][2] = x1;
				lines[i][3] = y1;
			}
		}

		// Now for each row in the image, find out the start / stop pixel
		// If min > max, then no intersection occurred
		for ( j = 0; j < this->ImageInUseSize[1]; j++ )
		{
			this->RowBounds[j*2] = this->ImageMemorySize[0];
			this->RowBounds[j*2+1] = -1;
			for ( i = 0; i < 12; i++ )
			{
				if ( j >= lines[i][1] && j <= lines[i][3] &&
					( lines[i][1] != lines[i][3] ) )
				{
					x1 = lines[i][0] +
						(static_cast<float>(j) - lines[i][1])/(lines[i][3] - lines[i][1]) *
						(lines[i][2] - lines[i][0] );

					xlow  = static_cast<int>(x1 + 1.5);
					xhigh = static_cast<int>(x1 - 1.0);

					xlow = (xlow<0)?(0):(xlow);
					xlow = (xlow>this->ImageInUseSize[0]-1)?
						(this->ImageInUseSize[0]-1):(xlow);

					xhigh = (xhigh<0)?(0):(xhigh);
					xhigh = (xhigh>this->ImageInUseSize[0]-1)?(
						this->ImageInUseSize[0]-1):(xhigh);

					if ( xlow < this->RowBounds[j*2] )
					{
						this->RowBounds[j*2] = xlow;
					}
					if ( xhigh > this->RowBounds[j*2+1] )
					{
						this->RowBounds[j*2+1] = xhigh;
					}
				}
			}
			// If they are the same this is either a point on the cube or
			// all lines were out of bounds (all on one side or the other)
			// It is safe to ignore the point (since the ray isn't likely
			// to travel through it enough to actually take a sample) and it
			// must be ignored in the case where all lines are out of range
			if ( this->RowBounds[j*2] == this->RowBounds[j*2+1] )
			{
				this->RowBounds[j*2] = this->ImageMemorySize[0];
				this->RowBounds[j*2+1] = -1;
			}
		}
	}

	for ( j = this->ImageInUseSize[1]; j < this->ImageMemorySize[1]; j++ )
	{
		this->RowBounds[j*2] = this->ImageMemorySize[0];
		this->RowBounds[j*2+1] = -1;
	}

	return 1;
}

void GPUGenericRayCastMapper::ComputeMatrices( vtkImageData *data, 
											vtkVolume *vol)
{
	// Get the data spacing. This scaling is not accounted for in
	// the volume's matrix, so we must add it in.
	double volumeSpacing[3];
	data->GetSpacing( volumeSpacing );

	// Get the origin of the data.  This translation is not accounted for in
	// the volume's matrix, so we must add it in.
	float volumeOrigin[3];
	double *bds = data->GetBounds();
	volumeOrigin[0] = bds[0];
	volumeOrigin[1] = bds[2];
	volumeOrigin[2] = bds[4];

	// Get the dimensions of the data.
	int volumeDimensions[3];
	data->GetDimensions( volumeDimensions );

	vtkTransform *voxelsTransform = this->VoxelsTransform;
	vtkTransform *worldTransform = vtkTransform::New();
	vtkTransform *voxelsToViewTransform = this->VoxelsToViewTransform;
	vtkTransform *worldToViewTransform = this->WorldToViewTransform;

	// Get the volume matrix. This is a volume to world matrix right now. 
	// We'll need to invert it, translate by the origin and scale by the 
	// spacing to change it to a world to voxels matrix.
	this->VolumeMatrix->DeepCopy( vol->GetMatrix() );
	voxelsToViewTransform->SetMatrix( VolumeMatrix );
	worldToViewTransform->SetMatrix( VolumeMatrix );

	// Create a transform that will account for the scaling and translation of
	// the scalar data. The is the volume to voxels matrix.
	voxelsTransform->Identity();
	voxelsTransform->Translate(volumeOrigin[0], 
		volumeOrigin[1], 
		volumeOrigin[2] );

	worldTransform->Identity();
	worldTransform->Translate(volumeOrigin[0], 
		volumeOrigin[1], 
		volumeOrigin[2] );

	worldToViewTransform->PreMultiply();
	worldToViewTransform->Concatenate( worldTransform->GetMatrix() );

	voxelsTransform->Scale( volumeSpacing[0],
		volumeSpacing[1],
		volumeSpacing[2] );

	// Now concatenate the volume's matrix with this scalar data matrix
	voxelsToViewTransform->PreMultiply();
	voxelsToViewTransform->Concatenate( voxelsTransform->GetMatrix() );

	// Now we actually have the world to voxels matrix - copy it out
	this->WorldToVoxelsMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );
	this->WorldToVoxelsMatrix->Invert();

	// We also want to invert this to get voxels to world
	this->VoxelsToWorldMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );

	// Compute the voxels to view transform by concatenating the
	// voxels to world matrix with the projection matrix (world to view)
	voxelsToViewTransform->PostMultiply();
	voxelsToViewTransform->Concatenate( this->PerspectiveMatrix );

	worldToViewTransform->PostMultiply();
	worldToViewTransform->Concatenate( this->PerspectiveMatrix );

	this->VoxelsToViewMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );
	this->WorldToViewMatrix->DeepCopy( worldToViewTransform->GetMatrix() );

	this->ViewToVoxelsMatrix->DeepCopy( this->VoxelsToViewMatrix );
	this->ViewToVoxelsMatrix->Invert();  

	this->ViewToWorldMatrix->DeepCopy( this->WorldToViewMatrix );
	this->ViewToWorldMatrix->Invert();  

	vtkMatrix4x4 *perspectiveMatrixInv = vtkMatrix4x4::New();
	perspectiveMatrixInv->DeepCopy(this->PerspectiveMatrix);
	perspectiveMatrixInv->Invert();

	vtkTransform *worldToViewTransformNoPersp = vtkTransform::New();
	worldToViewTransformNoPersp->SetMatrix(WorldToViewMatrix);

	worldToViewTransformNoPersp->PostMultiply();
	worldToViewTransformNoPersp->Concatenate( perspectiveMatrixInv );

	vtkMatrix4x4 *testMat = vtkMatrix4x4::New();
	testMat->DeepCopy(worldToViewTransformNoPersp->GetMatrix());
	
	worldTransform->Delete();
}

vtkMatrix4x4 *GPUGenericRayCastMapper::GetVolumeMatrix( vtkImageData *data, vtkVolume *vol )
{
	// Get the data spacing. This scaling is not accounted for in
	// the volume's matrix, so we must add it in.
	double volumeSpacing[3];
	data->GetSpacing( volumeSpacing );

	// Get the origin of the data.  This translation is not accounted for in
	// the volume's matrix, so we must add it in.
	float volumeOrigin[3];
	double *bds = data->GetBounds();
	volumeOrigin[0] = bds[0];
	volumeOrigin[1] = bds[2];
	volumeOrigin[2] = bds[4];

	// Get the dimensions of the data.
	int volumeDimensions[3];
	data->GetDimensions( volumeDimensions );

	vtkTransform *voxelsTransform = vtkTransform::New();
	vtkTransform *voxelsToViewTransform = vtkTransform::New();
	vtkMatrix4x4 *volMatrix = vtkMatrix4x4::New();
	vtkMatrix4x4* finalMatrix = vtkMatrix4x4::New();

	// Get the volume matrix. This is a volume to world matrix right now. 
	// We'll need to invert it, translate by the origin and scale by the 
	// spacing to change it to a world to voxels matrix.
	volMatrix->DeepCopy( vol->GetMatrix() );
	voxelsToViewTransform->SetMatrix( volMatrix );

	// Create a transform that will account for the scaling and translation of
	// the scalar data. The is the volume to voxels matrix.
	voxelsTransform->Identity();
	voxelsTransform->Translate(volumeOrigin[0], 
		volumeOrigin[1], 
		volumeOrigin[2] );

	voxelsTransform->Scale( volumeSpacing[0],
		volumeSpacing[1],
		volumeSpacing[2]);

	// Now concatenate the volume's matrix with this scalar data matrix
	voxelsToViewTransform->PreMultiply();
	voxelsToViewTransform->Concatenate( voxelsTransform->GetMatrix() );
	voxelsToViewTransform->Inverse();

	finalMatrix->DeepCopy(voxelsToViewTransform->GetMatrix());

	voxelsTransform->Delete();
	voxelsToViewTransform->Delete();
	volMatrix->Delete();

	return finalMatrix;
}

// Get the bounds for the input of this mapper as 
// (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *GPUGenericRayCastMapper::GetBounds()
{
	//BaseMatrix<float> *input = this->GetInput();
	if ( ! this->GetInput() ) 
	{
		vtkMath::UninitializeBounds(this->Bounds);
	}
	else
	{
		this->GetInput()->GetBounds(this->Bounds);
	}
	return this->Bounds;
}

// Print method for vtkVolumeRayCastMapper
void GPUGenericRayCastMapper::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}
