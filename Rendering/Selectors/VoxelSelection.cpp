#include "VoxelSelection.h"

VoxelSelection::VoxelSelection()
{
}

VoxelSelection::~VoxelSelection()
{
}

bool VoxelSelection::ContainsElement(int id)
{
	return (std::find(ids.begin(),ids.end(),id) != ids.end());
}

void VoxelSelection::AddElement(int id, float weight) 
{
	if(!ContainsElement(id))
	{
		ids.push_back(id); 
		w.push_back(weight); 
	}
}

