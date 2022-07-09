#include "GPUPlaneRayCastMapper.h"

vtkStandardNewMacro(GPUPlaneRayCastMapper);

GPUPlaneRayCastMapper::GPUPlaneRayCastMapper() : GPUGenericRayCastMapper()
{
	this->DummyVolume	=	 NULL;

	std::string kernel(KERNEL_DIR);
	kernel.append("/kernel4.cl");

	this->RayCaster = new CLRayCaster(kernel);
	//this->VoxelSelector = new MatrixVoxelSelector();
	this->Started = false;
	this->SetInputTo3DPlane();
}

GPUPlaneRayCastMapper::~GPUPlaneRayCastMapper()
{
	if(this->DummyVolume)
		this->DummyVolume->Delete();
}

void GPUPlaneRayCastMapper::SetInputTo3DPlane()
{
	this->SetInput(GetDummyVolume());
}

void GPUPlaneRayCastMapper::UpdateRaycaster(vtkVolume *vol)
{	
	this->RayCaster->SetRaycastVolume(GetDummyVolume(),GetVolumeMatrix(GetDummyVolume(),vol));
	this->RayCaster->SetLayout(this->Layout);
	this->RayCaster->SetDataFile(this->MatrixFile);
}

/*
void GPUPlaneRayCastMapper::UpdatePicker(vtkVolume *vol)
{
	this->VoxelSelector->SetVoxelData(this->GetInput());
	this->VoxelSelector->SetPickData(this->GetInput(),GetVolumeMatrix(GetDummyVolume(),vol));
	this->VoxelSelector->SetLayout(this->Layout);

	if(!this->Started)
	{
		this->VoxelSelector->StartBrushing();
		this->Started = true;
	}
}
*/

vtkImageData *GPUPlaneRayCastMapper::GetDummyVolume()
{
	if(!this->DummyVolume)
	{
		this->DummyVolume = vtkImageData::New();
		this->DummyVolume->SetScalarTypeToInt();
		this->DummyVolume->SetExtent(0,99,0,99,0,1);
		this->DummyVolume->AllocateScalars();

		int* dataPtr = static_cast<int*>(this->DummyVolume->GetScalarPointer());
		int nPts = this->DummyVolume->GetNumberOfPoints();

		for(int i = 0; i < nPts; i++)
			*dataPtr = 1;

	}
	return this->DummyVolume;
}