#include "GPUVolumeRayCastMapper.h"

vtkStandardNewMacro(GPUVolumeRayCastMapper);

GPUVolumeRayCastMapper::GPUVolumeRayCastMapper() : GPUGenericRayCastMapper()
{
	std::string program(KERNEL_DIR);
	program.append("/kernel4.cl");

	this->RayCaster					= new CLTwoPassRayCaster(program);
	//this->VoxelSelector				= new BrainVoxelSelector();
	this->MaskVolume				= NULL;
	this->AtlasVolume				= NULL;
	this->AtlasLUT					= NULL;
	this->raySetupMethod			= 0;

	this->CreateDefaultAtlasColors();
}

GPUVolumeRayCastMapper::~GPUVolumeRayCastMapper()
{
}

void GPUVolumeRayCastMapper::CreateDefaultAtlasColors()
{
	this->AtlasLUT = ColorMapping::BlueToYellow();
}

void GPUVolumeRayCastMapper::SetAtlasColors(vtkLookupTable* texels) 
{
	if(this->AtlasLUT == texels)
		return;

	this->AtlasLUT = texels;
}

void GPUVolumeRayCastMapper::UpdateRaycaster(vtkVolume *vol)
{	
	((CLTwoPassRayCaster*)this->RayCaster)->SetRaySetupMethod(raySetupMethod);
	((CLTwoPassRayCaster*)this->RayCaster)->SetAtlasColors(this->AtlasLUT);
	this->RayCaster->SetRaycastVolume(this->GetInput(),GetVolumeMatrix(this->GetInput(),vol));
	this->RayCaster->SetMaskVolume(this->GetMaskVolume(),GetVolumeMatrix(this->GetMaskVolume(),vol));
	this->RayCaster->SetAtlasVolume(this->GetAtlasVolume(),GetVolumeMatrix(this->GetAtlasVolume(),vol));
	this->RayCaster->SetLayout(this->Layout);
	this->RayCaster->SetDataFile(this->MatrixFile);
}

void GPUVolumeRayCastMapper::SetBrainSplit(float planePos[4], float planeNormal[4], int direction)
{
	((CLTwoPassRayCaster*)this->RayCaster)->SetBrainSplit(planePos,planeNormal,direction);
}

/*
void GPUVolumeRayCastMapper::UpdatePicker(vtkVolume *vol)
{
	this->VoxelSelector->SetVoxelData(this->GetInput());
	this->VoxelSelector->SetPickData(this->GetMaskVolume(),GetVolumeMatrix(this->GetMaskVolume(),vol));
	this->VoxelSelector->SetLayout(this->Layout);

}
*/

void GPUVolumeRayCastMapper::SetMaskVolume(vtkImageData *maskVol)
{
	this->MaskVolume = maskVol;
}

vtkImageData *GPUVolumeRayCastMapper::GetMaskVolume()
{
	return this->MaskVolume;
}

