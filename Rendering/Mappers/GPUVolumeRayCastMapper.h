#ifndef GPUVOLUMERAYCASTMAPPER_HEADER
#define GPUVOLUMERAYCASTMAPPER_HEADER

#include "../DLLDefines.h"
#include "GPUGenericRayCastMapper.h"
#include <Mappers/CLTwoPassRayCaster.h>

class Rendering_EXPORT GPUVolumeRayCastMapper : public GPUGenericRayCastMapper
{
public:
	static GPUVolumeRayCastMapper *New();
	vtkTypeMacro(GPUVolumeRayCastMapper,vtkVolumeMapper);

	virtual void SetConnectivityData( DataFile *file) 
	{ 
		GPUGenericRayCastMapper::SetConnectivityData(file);
		BrainLayout *layout = new BrainLayout();
		layout->SetPositions(file->points());
		layout->SetSpacing(file->spacingX(),file->spacingY(),file->spacingZ());
		this->SetLayoutSystem(layout);
	}

	virtual vtkImageData* GetMaskVolume();
	virtual void SetMaskVolume(vtkImageData *mask);
	virtual vtkImageData* GetAtlasVolume() { return this->AtlasVolume; }

	virtual void SetAtlasVolume(vtkImageData* data) { this->AtlasVolume = data; }
	virtual void SetRaySetupMethod(int method) { raySetupMethod = method; }

	virtual void SetBrainSplit(float planePos[4], float planeNormal[4], int direction);

	virtual void SetAtlasColors(vtkLookupTable* texels);

protected:
	GPUVolumeRayCastMapper();
	~GPUVolumeRayCastMapper();

	//refresh should notify changes
	//virtual void Refresh() {}

	virtual void UpdateRaycaster(vtkVolume *vol);
	//virtual void UpdatePicker(vtkVolume *vol);


private:
	GPUVolumeRayCastMapper(const GPUVolumeRayCastMapper&);  // Not implemented.
	void operator=(const GPUVolumeRayCastMapper&);  // Not implemented.

	void CreateDefaultAtlasColors();

	vtkImageData* MaskVolume;
	vtkImageData* AtlasVolume;
	vtkLookupTable* AtlasLUT;
	int raySetupMethod;

};

#endif //GPUVOLUMERAYCASTMAPPER_HEADER