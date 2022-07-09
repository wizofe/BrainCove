#ifndef GPUGENERICRAYCASTMAPPER_HEADER
#define GPUGENERICRAYCASTMAPPER_HEADER

#include "../DLLDefines.h"
#include <vtkVolumeMapper.h>
#include <DataFile.h>
#include "CLRayCaster.h"
#include <Layouts/BaseLayout.h>
#include <Selectors/DataSelection.h>
#include <Filters/DataFilter.h>
#include <vector>
#include <algorithm>


/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT GPUGenericRayCastMapper : public vtkVolumeMapper
{

public:
	virtual void SetConnectivityData( DataFile *) = 0;
	virtual DataFile *GetConnectivityData() = 0;
	
	virtual void SetMouseCoordinates(int x, int y) = 0;
	virtual int* GetMouseCoordinates() = 0;

	virtual void AddFilter(DataFilter *filter)
	{
		Filters.push_back(filter);
		Refresh();
	}

	virtual void RemoveFilter(DataFilter *filter)
	{
		std::vector<DataFilter*>::iterator pos = std::find(Filters.begin(),Filters.end(),filter);
		Filters.erase(pos);
		Refresh();
	}

	virtual void SetSelection(DataSelection *selection)
	{
		if(Selection != selection)
		{
			Selection = selection;
			Refresh();
		}
	}

	virtual DataSelection *GetSelection()
	{
		return Selection;
	}

	// Description:
	// This is the actual program that will run to draw pixels on the texture
	virtual void SetRayCaster(CLRayCaster* rayCaster) = 0;
	virtual CLRayCaster* GetRayCaster() = 0;

protected:

	virtual void Refresh() = 0;

	DataSelection *Selection;
	std::vector<DataFilter*> Filters;
};

#endif //GPUGENERICRAYCASTMAPPER_HEADER