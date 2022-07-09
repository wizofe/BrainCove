#ifndef VOXELSELECTION_HEADER
#define VOXELSELECTION_HEADER

#include "../DLLDefines.h"
#include "DataSelection.h"
#include <vector>
#include <algorithm>
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT VoxelSelection : public DataSelection
{

public:
	VoxelSelection();
	~VoxelSelection();

	virtual int contentType() { return VOXELTYPE; }
	virtual int structureType() { return UNSTRUCTUREDSELECTION; }
	virtual int size() { return ids.size(); }

	virtual int* elements() { return &ids[0]; }
	virtual float* weights() { return &w[0]; }

	virtual void clear() { ids.clear(); w.clear(); }

	bool ContainsElement(int id);
	void AddElement(int id, float weight);

private:
	std::vector<int> ids;
	std::vector<float> w;
};

#endif //VOXELSELECTION_HEADER