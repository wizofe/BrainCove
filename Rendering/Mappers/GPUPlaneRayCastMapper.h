#ifndef GPUPLANERAYCASTMAPPER_HEADER
#define GPUPLANERAYCASTMAPPER_HEADER

#include "../DLLDefines.h"
#include "GPUGenericRayCastMapper.h"
#include <Selectors/MatrixVoxelSelector.h>

class Rendering_EXPORT GPUPlaneRayCastMapper : public GPUGenericRayCastMapper
{
public:
	static GPUPlaneRayCastMapper *New();
	vtkTypeMacro(GPUPlaneRayCastMapper,GPUGenericRayCastMapper);

	void SetInputTo3DPlane();

	virtual vtkImageData* GetMaskVolume() { return NULL; }
	virtual vtkImageData* GetAtlasVolume() { return NULL; }

protected:
	GPUPlaneRayCastMapper();
	~GPUPlaneRayCastMapper();

	//refresh should notify changes
	//virtual void Refresh() {}

	virtual void UpdateRaycaster(vtkVolume *vol);
	//virtual void UpdatePicker(vtkVolume *vol);

private:
	GPUPlaneRayCastMapper(const GPUPlaneRayCastMapper&);  // Not implemented.
	void operator=(const GPUPlaneRayCastMapper&);  // Not implemented.

	vtkImageData *GetDummyVolume();

	vtkImageData *DummyVolume;

	bool Started;
};

#endif //GPUPLANERAYCASTMAPPER_HEADER