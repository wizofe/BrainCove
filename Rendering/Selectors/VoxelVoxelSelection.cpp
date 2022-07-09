#include "VoxelVoxelSelection.h"

VoxelVoxelSelection::VoxelVoxelSelection() : DataSelection()
{
}

VoxelVoxelSelection::~VoxelVoxelSelection()
{
}

bool VoxelVoxelSelection::ContainsElement(int id1, int id2)
{
	std::vector<int>::iterator it;
	for(it=ids.begin();it!=ids.end();it++)
	{
		if(*it == id1 && (it - ids.begin())%2 == 0)
		{
			it++;
			if(it!=ids.end() && *it == id2)
				return true;
		}
		else if(*it == id2 && (it - ids.begin())%2 == 0) //symmetrical link (A->B ~ B->A)
		{
			it++;
			if(it!=ids.end() && *it == id1)
				return true;
		}
	}
	return false;
}

void VoxelVoxelSelection::AddElement(int id1, int id2, float weight) 
{
	switch(structureType())
	{
		case UNSTRUCTUREDSELECTION:
			if(!ContainsElement(id1,id2))
			{
				ids.push_back(id1); 
				ids.push_back(id2); 
				w.push_back(weight); 
			}
			break;
		case RECTANGLESELECTION:
			if(ids.size()==0)
			{
				//initialize with a rectangle that is a single point
				ids.push_back(id1);
				ids.push_back(id1);
				ids.push_back(id2);
				ids.push_back(id2);
				w.push_back(weight);
				w.push_back(weight);
			}
			else
			{
				if(id1 < ids[0])
					ids[0] = id1;
				if(id1 > ids[1])
					ids[1] = id1;
				if(id2 < ids[2])
					ids[2] = id2;
				if(id2 > ids[3])
					ids[3] = id2;
				//ids[1] = id1;
				//ids[3] = id2;
			}
			break;
	}
}