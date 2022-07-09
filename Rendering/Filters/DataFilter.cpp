#include "DataFilter.h"

DataFilter::DataFilter()
{
	target = FilterTarget::NO_FILTER;
	range = FilterRange::FILTER_ABOVE;

	rangeStart = -1;
	rangeEnd = 1;
}