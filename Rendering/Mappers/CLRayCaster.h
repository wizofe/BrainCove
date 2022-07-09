#ifndef CLRAYCASTER_HEADER
#define CLRAYCASTER_HEADER

#include "../DLLDefines.h"
#include <Utils/clext.h>
#include <vtkOpenGL.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <CLWrappers/CLProgram.h>
#include <CLWrappers/CLTexture2DObject.h>
#include <DataFile.h>
#include <Utils/ColorMapping.h>
#include <sstream>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <map>
//#include "../kernels/kernel_defines.h"
#include <Selectors/BaseVoxelSelector.h>
#include <CLWrappers/CLMemoryObject.h>
#include <CLWrappers/CLBufferObject.h>
#include <CLWrappers/CLTexture3DObject.h>
#include <Layouts/BrainLayout1D.h>
#include <Selectors/DataSelection.h>
#include <Filters/DataFilter.h>
#include <QTime>
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

typedef struct ViewParameters
{
   cl_float16	viewToWorldMatrix;
   cl_float8	bounds;
   cl_uint2		imageInUseSize;
   cl_uint2		viewPortSize;
   cl_uint2		imageOrigin;
   cl_float		cameraThickness;
   cl_float		dummy;
} ViewParameters;

class Rendering_EXPORT CLRayCaster : public CLProgram
{
public:
	CLRayCaster(std::string programFile);
	~CLRayCaster();

	virtual void SetLookupTable(vtkLookupTable *lut);
	virtual void SetDataFile(DataFile* file);
	virtual void SetComparisonDataFile(DataFile* file);
	virtual void SetLayout(BaseLayout* layout);

	virtual void SetRenderModeToComparison() { IsComparing = true; }
	virtual void SetRenderModeToNormal() { IsComparing = false; }

	virtual void SetRaycastVolume(vtkImageData *data, vtkMatrix4x4* volumeMatrix);
	virtual void SetMaskVolume(vtkImageData *data, vtkMatrix4x4* volumeMatrix) {}
	virtual void SetAtlasVolume(vtkImageData *data, vtkMatrix4x4* volumeMatrix) {}

	virtual int CastRays(vtkRenderer *renderer, CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, DataSelection* voxel_selection, std::vector<DataFilter> filters);

protected:
	virtual void CreateDefaultColorMap();
	virtual void CreateDefaultLayout();
	virtual void ReCreateSplatTexture(int w, int h);

	virtual void RecreateTexture();
	void UpdateSelectionBuffers(DataSelection* selection);

	ViewParameters CreateViewStruct(CLTexture2DObject* texture, int viewPortSize[2], int imageOrigin[2],vtkImageData *data,vtkMatrix4x4 *viewToVoxelsMatrix,double camThickness);

	cl_float16 ToCLMat(vtkMatrix4x4* mat);

	CLTexture3DObject *RaycastVolumeTexture;
	vtkMatrix4x4 *RaycastVolumeMatrix;

	BaseMatrix<float> *Matrix;
	BaseMatrix<float> *ComparisonMatrix;
	BaseLayout *Layout;

	CLTexture2DObject *LutTexture;

	CLTexture2DObject *SplatTexture;

	CLBufferObject *MatrixBufferGPU;
	CLBufferObject *ComparisonMatrixBufferGPU;
	cl::Buffer SelectionElementsBuffer;
	cl::Buffer SelectionWeightsBuffer;

	cl::vector<cl::Memory> *memsGL;

	cl::Buffer ElementOrderBuffer;

	vtkImageData *RaycastVolume;

	bool IsComparing;
};
#endif //CLRAYCASTER_HEADER