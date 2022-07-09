#ifndef MATRIXVOXELSELECTOR_HEADER
#define MATRIXVOXELSELECTOR_HEADER

#include "../DLLDefines.h"
#include "BaseVoxelSelector.h"
#include "VoxelVoxelSelection.h"
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT MatrixVoxelSelector : public BaseVoxelSelector
{

public:
	MatrixVoxelSelector();
	~MatrixVoxelSelector();

	virtual DataSelection* SelectWithSeed(int seed);
	virtual DataSelection* PickVolume(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *, int imageBounds[4]);

	virtual void StartBrushing();
	virtual void EndBrushing();

protected:
	VoxelVoxelSelection *CurrentSelection;
	VoxelVoxelSelection *CurrentHover;
	bool brushing;
	bool SavedStart;
	int StartPoint[2];

};

#endif //MATRIXVOXELSELECTOR_HEADER