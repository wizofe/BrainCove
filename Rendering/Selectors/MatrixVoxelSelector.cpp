#include "MatrixVoxelSelector.h"

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

MatrixVoxelSelector::MatrixVoxelSelector() : BaseVoxelSelector()
{
	this->CurrentSelection = NULL;
	this->CurrentHover = NULL;
	this->brushing = false;
	this->SavedStart = false;
}

MatrixVoxelSelector::~MatrixVoxelSelector()
{
	if(this->CurrentSelection)
		delete this->CurrentSelection;
	if(this->CurrentHover)
		delete this->CurrentHover;
}

DataSelection* MatrixVoxelSelector::SelectWithSeed(int seed)
{
	return this->CurrentSelection;
}

DataSelection* MatrixVoxelSelector::PickVolume(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *ViewToVoxelsMatrix, int imageBounds[4])
{
	if(!brushing && this->CurrentSelection && !this->CurrentSelection->isEmpty())
		return this->CurrentSelection;

	if(!this->CurrentHover)
		this->CurrentHover = new VoxelVoxelSelection();
	if(!this->CurrentSelection)
		this->CurrentSelection = new VoxelVoxelSelection();
	this->CurrentSelection->SetStructureTypeToRectangle();

	if(!this->GetVoxelData())
		return this->CurrentSelection;

	int x = coords[0];
	int y = coords[1];

	if(x<imageBounds[0] || x>imageBounds[1] || y<imageBounds[2] || y>imageBounds[3])
		return this->CurrentSelection;

	float viewRay[4];
	float rayCenter[3];
	float voxelPoint[4];
	float rayStart[4];
	float rayEnd[4];

	vtkCamera *cam = renderer->GetActiveCamera();

	double viToVoMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			viToVoMat[4*i+j] = ViewToVoxelsMatrix->GetElement(i,j);

	// We need to know what the center ray is (in voxel coordinates) to
	// compute sampling distance later on. Save it in an instance variable.

	// This is tbe near end of the center ray in view coordinates
	// Convert it to voxel coordinates
	viewRay[0] = viewRay[1] = viewRay[2] = 0.0;  
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );

	// This is the far end of the center ray in view coordinates
	// Convert it to voxel coordiantes
	viewRay[2] = 1.0;  
	vtkVRCMultiplyPointMacro( viewRay, voxelPoint,
							viToVoMat );

	// Turn these two points into a vector
	rayCenter[0] = voxelPoint[0] - rayStart[0];
	rayCenter[1] = voxelPoint[1] - rayStart[1];
	rayCenter[2] = voxelPoint[2] - rayStart[2];

	// normalize the vector based on world coordinate distance
	// This way we can scale by sample distance and it will work
	// out even though we are in voxel coordinates
	rayCenter[0] /= cam->GetThickness();
	rayCenter[1] /= cam->GetThickness();
	rayCenter[2] /= cam->GetThickness();

	float centerScale = sqrt( (rayCenter[0] * rayCenter[0]) +
							(rayCenter[1] * rayCenter[1]) +
							(rayCenter[2] * rayCenter[2]) );

	rayCenter[0] /= centerScale;
	rayCenter[1] /= centerScale;
	rayCenter[2] /= centerScale;

	int viewportSize[2];
	renderer->GetTiledSize(&viewportSize[0], &viewportSize[1]);

	NormalizeCoordinates(x,y,0,1,viewportSize,viewRay);
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );
	
	for(int i = 0; i < 3; i++)
		voxelPoint[i] = rayStart[i];

	voxelPoint[2] = 0; //our ray will hit at z=0, which is where the plane is.

	float cVoxelPoint[4];

	vtkVRCMultiplyPointMacro(voxelPoint,cVoxelPoint,this->WorldToPickVoxelMatrix);

	double bounds[6];
	this->GetVoxelData()->GetBounds(bounds);

	if(!(voxelPoint[0] < bounds[0] || voxelPoint[0] > bounds[1] ||
			voxelPoint[1] < bounds[2] || voxelPoint[1] > bounds[3] ||
			voxelPoint[2] < bounds[4] || voxelPoint[2] > bounds[5]))
	{
		float xx = (voxelPoint[0]-bounds[0]) / (bounds[1]-bounds[0]+1);
		float yy = (voxelPoint[1]-bounds[2]) / (bounds[3]-bounds[2]+1);
		double *v1 = this->Layout->GetPosition((int)(xx*this->Layout->GetNumberOfPositions()));
		double *v2 = this->Layout->GetPosition((int)(yy*this->Layout->GetNumberOfPositions()));
		
		if(brushing)
		{
			this->CurrentSelection->clear();
			int xxx = (int)(xx*this->Layout->GetNumberOfPositions());
			int yyy = (int)(yy*this->Layout->GetNumberOfPositions());
			if(!this->SavedStart)
			{
				this->CurrentSelection->SetPosition(voxelPoint[0],voxelPoint[1],0);
				this->StartPoint[0] = xxx;
				this->StartPoint[1] = yyy;
				this->SavedStart = true;
			}
			this->CurrentSelection->AddElement(this->StartPoint[0],this->StartPoint[1],1);
			this->CurrentSelection->AddElement(xxx,yyy,1);
		}
		else
		{
			this->CurrentHover->clear();
			this->CurrentHover->AddElement((int)v1[0],(int)v2[0],1);
			this->CurrentHover->SetPosition(voxelPoint[0],voxelPoint[1],0);
			return this->CurrentHover;
		}
	}
	if(!this->CurrentSelection->isEmpty())
		return this->CurrentSelection;
	else
		return this->CurrentHover;
}

void MatrixVoxelSelector::StartBrushing()
{
	brushing = true;
	if(!this->CurrentSelection)
	{
		this->CurrentSelection = new VoxelVoxelSelection();
		this->CurrentSelection->SetStructureTypeToRectangle();
	}
	this->SavedStart = false;
	this->CurrentSelection->clear();
}

void MatrixVoxelSelector::EndBrushing()
{
	brushing = false;
}
