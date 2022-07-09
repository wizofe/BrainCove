#ifndef DATASELECTION_HEADER
#define DATASELECTION_HEADER

#include "../DLLDefines.h"
#include <Layouts/BrainLayout1D.h>
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
#define NOSELECTION 0
#define VOXELTYPE 1
#define LINKTYPE 2

#define UNSTRUCTUREDSELECTION 1
#define RECTANGLESELECTION 2

class Rendering_EXPORT DataSelection
{

public:
	virtual int contentType() = 0;
	virtual int structureType() { return structType; }

	virtual int size() = 0;

	virtual int* elements() = 0;
	virtual float* weights() = 0;

	virtual void clear() = 0;

	virtual bool isEmpty() { return size() == 0; }

	virtual BrainLayout1D * layout() { return VoxelReorder; }

	virtual double const* position() { return &pos[0]; }
	virtual void SetStructureTypeToRectangle() { structType = RECTANGLESELECTION; }
	virtual void SetLayout(BrainLayout1D *layout) 
	{
		VoxelReorder = layout; 
	}

	virtual void SetPosition(double x, double y, double z)
	{ 
		pos[0] = x; 
		pos[1] = y;
		pos[2] = z;
	}

protected:
	DataSelection();

	int structType;
	double pos[3];

	BrainLayout1D *VoxelReorder;
};

#endif //DATASELECTION_HEADER