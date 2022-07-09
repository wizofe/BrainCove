#include "BaseVoxelSelector.h"

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

BaseVoxelSelector::BaseVoxelSelector()
{
	this->Layout = NULL;
	this->Data = NULL;
}

void BaseVoxelSelector::SetLayout(BaseLayout* layout)
{
	this->Layout = layout;
}

BaseLayout* BaseVoxelSelector::GetLayout()
{
	return this->Layout;
}

void BaseVoxelSelector::SetPickData(vtkImageData *data,vtkMatrix4x4 *volMat)
{
	this->PickData = data;

	vtkMatrix4x4 *volMatInv = vtkMatrix4x4::New();
	volMatInv->DeepCopy(volMat);
	volMatInv->Invert();

	for(int i = 0;i<4;i++)
	{
		for(int j = 0;j<4;j++)
		{
			this->WorldToPickVoxelMatrix[4*i+j] = volMat->GetElement(i,j);
			this->PickVoxelToWorldMatrix[4*i+j] = volMatInv->GetElement(i,j);
		}
	}
	volMatInv->Delete();
}


void BaseVoxelSelector::SetAtlasData(vtkImageData *data,vtkMatrix4x4 *volMat)
{
	this->AtlasData = data;

	vtkMatrix4x4 *volMatInv = vtkMatrix4x4::New();
	volMatInv->DeepCopy(volMat);
	volMatInv->Invert();

	for(int i = 0;i<4;i++)
	{
		for(int j = 0;j<4;j++)
		{
			this->WorldToAtlasVoxelMatrix[4*i+j] = volMat->GetElement(i,j);
			this->AtlasVoxelToWorldMatrix[4*i+j] = volMatInv->GetElement(i,j);
		}
	}
	volMatInv->Delete();
}



void BaseVoxelSelector::SetVoxelData(vtkImageData *data)
{
	this->Data = data;
}

vtkImageData* BaseVoxelSelector::GetVoxelData() const
{
	return this->Data;
}

vtkImageData* BaseVoxelSelector::GetPickData() const
{
	return this->PickData;
}

vtkImageData* BaseVoxelSelector::GetAtlasData() const
{
	return this->AtlasData;
}

void BaseVoxelSelector::NormalizeCoordinates(int x, int y, float z, float w, int *viewPortSize, float outCoord[4])
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

int BaseVoxelSelector::GetIdFromWorldPosition(double world[4])
{
	double pickPos[4];
	int voxel = -1;

	int extent[6];
	this->GetPickData()->GetExtent(extent);

	vtkVRCMultiplyPointMacro(world,pickPos,this->WorldToPickVoxelMatrix);
	
	if(!(pickPos[0] < extent[0] || pickPos[0] > extent[1] ||
		pickPos[1] < extent[2] || pickPos[1] > extent[3] ||
		pickPos[2] < extent[4] || pickPos[2] > extent[5]))
	{
		//printf("%.2f, %.2f, %.2f\n",cVoxelPoint[0],cVoxelPoint[1],cVoxelPoint[2]);
		short* ptr = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)(pickPos[0]),(int)(pickPos[1]),(int)(pickPos[2])));
		if(*ptr!=-1)
			voxel = *ptr;
	}
	return voxel;
}