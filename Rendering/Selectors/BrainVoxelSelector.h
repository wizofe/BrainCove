#ifndef BRAINVOXELSELECTOR_HEADER
#define BRAINVOXELSELECTOR_HEADER
#include "../DLLDefines.h"
#include <map>
#include <math.h>
#include "BaseVoxelSelector.h"
#include <Selectors/VoxelSelection.h>
#include <Layouts/BrainLayout.h>
#include <vtkTransform.h>

#define M_PI 3.14159265358979323846264338327f
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

enum RenderType
{
	Template3D,
	MappedCortex
};

class Rendering_EXPORT BrainVoxelSelector : public BaseVoxelSelector
{

public:
	BrainVoxelSelector();

	virtual DataSelection* SelectWithSeed(int seed);
	virtual DataSelection* SelectWithTemplate(int seed);
	virtual DataSelection* PickVolume(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]);

	virtual DataSelection* PickLastVolume3D(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]);
	virtual DataSelection* PickVolume3D(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]);
	virtual DataSelection* PickVolume2DMapped(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]);

	virtual void SetRenderType(RenderType type);

	virtual vtkMatrix4x4* GetLeftDisplacementMatrix() { return LeftDisplaceMatrix; }
	virtual vtkMatrix4x4* GetRightDisplacementMatrix() { return RightDisplaceMatrix; }

	virtual void SetLeftDisplacementMatrix(vtkMatrix4x4* mat) { LeftDisplaceMatrix = mat; }
	virtual void SetRightDisplacementMatrix(vtkMatrix4x4* mat) { RightDisplaceMatrix = mat; }

	virtual void SetDisplacementFromAngle(float angle, double viewUp[3]);

	void SetBrainSplit(float planePos[4],float planeNorm[4]);

protected:
	float Dot(float v1[4], float v2[4]) { return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]; }
	
	void Clone(float v1[4], float* v_out)
	{ 
		for(int i = 0; i < 4; i++)
			v_out[i] = v1[i];
	}
	
	void Add(float v1[4], float v2[4], float* v_out)
	{  
		for(int i = 0; i < 3; i++)
			v_out[i] = v1[i]+v2[i];

		//v_out[3] = v1[3];
	}

	void Subtract(float v1[4], float v2[4], float* v_out)
	{  
		for(int i = 0; i < 3; i++)
			v_out[i] = v1[i]-v2[i];

		//v_out[3] = v1[3];
	}

	void Normalize(float v1[4], float* v_out)
	{
		float length = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
		for(int i = 0; i < 3; i++)
			v_out[i] = v1[i]/length;
		v_out[3] = 1;
	}

	int Size;	
	int Type;

	vtkMatrix4x4 *LeftDisplaceMatrix;
	vtkMatrix4x4 *RightDisplaceMatrix;

	float PlanePosition[4];
	float PlaneNormal[4];

};

#endif //BRAINVOXELSELECTOR_HEADER