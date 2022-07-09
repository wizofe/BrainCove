#ifndef BASEVOXELSELECTOR_HEADER
#define BASEVOXELSELECTOR_HEADER

#include "../DLLDefines.h"
#include "BaseMatrix.hpp"
#include <Layouts/BaseLayout.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCamera.h>
#include <Selectors/DataSelection.h>
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT BaseVoxelSelector
{

public:
	virtual void SetLayout(BaseLayout* layout);
	virtual BaseLayout* GetLayout();
	virtual void SetPickData(vtkImageData *data,vtkMatrix4x4 *volMat);
	virtual void SetAtlasData(vtkImageData *data,vtkMatrix4x4 *volMat);
	virtual void SetVoxelData(vtkImageData *data);
	virtual vtkImageData* GetVoxelData() const;
	virtual vtkImageData* GetPickData() const;
	virtual vtkImageData* GetAtlasData() const;

	virtual DataSelection* SelectWithSeed(int seed) = 0;
	virtual DataSelection* PickVolume(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]) = 0;

	virtual int GetIdFromWorldPosition(double world[4]);

	virtual void StartBrushing() {};
	virtual void EndBrushing() {};

protected:
	BaseVoxelSelector();

	void NormalizeCoordinates(int x, int y, float z, float w, int *viewPortSize, float outCoord[4]);

	BaseLayout *Layout;
	vtkImageData *Data;
	vtkImageData *AtlasData;
	vtkImageData *PickData;
	double WorldToPickVoxelMatrix[16];
	double PickVoxelToWorldMatrix[16];
	double WorldToAtlasVoxelMatrix[16];
	double AtlasVoxelToWorldMatrix[16];
};

#endif //BASEVOXELSELECTOR_HEADER