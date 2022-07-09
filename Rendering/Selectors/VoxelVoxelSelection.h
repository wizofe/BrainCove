#ifndef VOXELVOXELSELECTION_HEADER
#define VOXELVOXELSELECTION_HEADER

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

class Rendering_EXPORT VoxelVoxelSelection : public DataSelection
{

public:
	VoxelVoxelSelection();
	~VoxelVoxelSelection();

	virtual int contentType() { return LINKTYPE; }
	virtual int structureType() { return structType; }
	virtual int size() { return w.size(); }

	virtual void clear() { ids.clear(); w.clear(); }

	virtual int* elements() 
	{ 
		if(ids.size() > 0)
			return &ids[0]; 
		else
			return 0;
	}
	virtual float* weights() 
	{ 
		if(w.size() > 0)
			return &w[0]; 
		else
			return 0;
	}

	bool ContainsElement(int id1, int id2);
	void AddElement(int id1, int id2, float weight);

private:
	std::vector<int> ids;
	std::vector<float> w;
};

#endif //VOXELVOXELSELECTION_HEADER