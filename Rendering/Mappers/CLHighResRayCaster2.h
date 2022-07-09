#ifndef CLHighResRayCaster_HEADER
#define CLHighResRayCaster_HEADER

#include "../DLLDefines.h"
#include <Utils/clext.h>
#include <vtkOpenGL.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include "CLRayCaster.h"
#include <CLWrappers/CLTexture3DObject.h>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLHighResRayCaster : public CLRayCaster
{
public:
	CLHighResRayCaster(std::string kernel);
	~CLHighResRayCaster();

	virtual int CastRays(CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, std::map<int,float> voxel_selection, double correlation);

	virtual void SetMaskData(vtkImageData *mask);

protected:
	virtual void CreateColorMap();
	virtual void RecreateTexture();
	virtual int GetRaycastingMethod() { return DOUBLE_PASS_RAYCASTING_LOOP; }

	void RecreateVolumeData();
	int PreprocessVolumeWithVoxel(std::map<int,float> voxel_selection);
	int ToFirstMultipleOf(int val, int multiple);

	virtual bool SelectionChanged(std::map<int,float> voxel_selection);

	vtkImageData *MaskData;
	CLTexture3DObject *MaskTexture;

	std::map<int,float> last_voxel_selection;
};
#endif //CLHighResRayCaster_HEADER