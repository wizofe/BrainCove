#ifndef BASELAYOUT_HEADER
#define BASELAYOUT_HEADER

#include "../DLLDefines.h"
#include "BaseMatrix.hpp"
#include <vector>
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT BaseLayout
{

public:
	virtual void GetPosition(int index, double[3]) = 0;
	virtual double* GetPosition(int index) = 0;

	virtual void GetBounds(double[6]) = 0;
	virtual double* GetBounds() = 0;

	virtual void GetDimensions(double[3]) = 0;
	virtual double* GetDimensions() = 0;

	virtual void GetOrigin(double[3]) = 0;
	virtual double* GetOrigin() = 0;

	virtual double* GetSpacing()
	{
		double *spac = new double[3];
		GetSpacing(spac);
		return spac;
	}
	
	virtual void GetSpacing(double spac[3])
	{
		spac[0] = spac[1] = spac[2] = 1;
	}

	virtual void SetSpacing(double sX, double sY, double sZ) = 0;

	virtual void SetSpacing(double spac[3]) = 0;

	virtual int GetNumberOfPositions() = 0;

	virtual int GetNumberOfDimensions() = 0;

protected:
	BaseLayout();

};

#endif //ARENDERABLE_HEADER