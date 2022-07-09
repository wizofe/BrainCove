#ifndef DATAFILTER_HEADER
#define DATAFILTER_HEADER

#include "../DLLDefines.h"
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

enum FilterTarget
{
	NO_FILTER = 0,
	CORRELATION_FILTER = 1,
	DISTANCE_FILTER = 2 // not yet implemented
};

enum FilterRange
{
	FILTER_ABOVE = 1,
	FILTER_BELOW = 2,
	FILTER_BETWEEN = 3,
	FILTER_OUTSIDE = 4
};


struct Rendering_EXPORT DataFilter
{
	DataFilter();

	bool operator==(const DataFilter &rhs)
	{
		return target == rhs.target && range == rhs.range && rangeStart == rhs.rangeStart && rangeEnd == rhs.rangeEnd;
	}

	int target;
	int range;

	float rangeStart;
	float rangeEnd;
};

#endif //ARENDERABLE_HEADER