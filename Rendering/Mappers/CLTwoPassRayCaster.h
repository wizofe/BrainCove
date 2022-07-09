#ifndef CLTwoPassRayCaster_HEADER
#define CLTwoPassRayCaster_HEADER

#include "../DLLDefines.h"
#include <Utils/clext.h>
#include <vtkOpenGL.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include "CLRayCaster.h"
#include <Layouts/BrainLayout.h>
#include <CLWrappers/CLTexture3DObject.h>
#include <vtkXMLImageDataWriter.h>
#include <algorithm>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLTwoPassRayCaster : public CLRayCaster
{
public:
	CLTwoPassRayCaster(std::string kernel);
	~CLTwoPassRayCaster();
	virtual void SetMaskVolume(vtkImageData *data, vtkMatrix4x4 *volumeMatrix);
	virtual void SetAtlasVolume(vtkImageData *data, vtkMatrix4x4 *volumeMatrix);
	virtual void SetLayout(BaseLayout* layout);

	virtual void SetRaySetupMethod(int method) { raySetupMethod = method; }
	virtual void SetBrainSplit(float pos[4], float normal[4], int direction);

	virtual int CastRays(vtkRenderer *renderer, CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, DataSelection* selection, std::vector<DataFilter> filters);

	virtual void SetAtlasColors(vtkLookupTable* texels);
	virtual void SetLeftDisplacementMatrix(vtkMatrix4x4* mat) { LeftDisplacementMatrix->DeepCopy(mat); }
	virtual void SetRightDisplacementMatrix(vtkMatrix4x4* mat) { RightDisplacementMatrix->DeepCopy(mat); }
	virtual void CreateDispMatrices(int direction);

	virtual void SaveCorrelationVolume();

protected:
	virtual void RecreateTexture();

	void RecreateVolumeData();
	int PreprocessVolumeWithVoxel(DataSelection *selection,std::vector<DataFilter> filters);
	int ToFirstMultipleOf(int val, int multiple);

	virtual bool SelectionChanged(DataSelection *selection);
	
	CLTexture3DObject *CorrelationVolumeTexture;
	CLTexture3DObject *IntermediateVolumeTexture;
	vtkImageData *CorrelationVolume;

	vtkImageData *AtlasVolume;
	CLTexture3DObject *AtlasVolumeTexture;
	vtkMatrix4x4 *AtlasVolumeMatrix;

	vtkImageData *MaskVolume;
	CLTexture3DObject *MaskVolumeTexture;
	vtkMatrix4x4 *MaskVolumeMatrix;

	int raySetupMethod;
	vtkMatrix4x4 *LeftDisplacementMatrix;
	vtkMatrix4x4 *RightDisplacementMatrix;

	vtkLookupTable *Lut2Texels;
	CLTexture2DObject *LutTexture2;

	float splitPanePos[4];
	float splitPaneNormal[4];
	int splitPaneDir;

};
#endif //CLRAYCASTER_HEADER