#ifndef BRAINLAYOUT1D_HEADER
#define BRAINLAYOUT1D_HEADER

#include "../DLLDefines.h"
#include "BaseLayout.h"
#include <string>
#include <cfloat>
#include "PointReader.h"
#include <sstream>
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT BrainLayout1D : public BaseLayout
{

public:
	BrainLayout1D();
	~BrainLayout1D();

	static BrainLayout1D* FromFile(std::string filePath);
	static BrainLayout1D* FromText(std::string text);

	void LoadPositionsFromFile(std::string filePath);
	void LoadPositionsFromText(std::string text);
	/*void SetPositions(float *posArray)
	{ 
		positions = posArray; 
		ResetBounds();
	}*/

	virtual void GetPosition(int index, double[3]);
	virtual double* GetPosition(int index)
	{
		double *pos = new double[3];
		GetPosition(index,pos);
		return pos;
	}

	virtual void GetBounds(double[6]);
	virtual double* GetBounds()
	{
		double *bds = new double[6];
		GetBounds(bds);
		return bds;
	}

	virtual void GetDimensions(double[3]);
	virtual double* GetDimensions()
	{
		double *dms = new double[3];
		GetDimensions(dms);
		return dms;
	}

	virtual void GetOrigin(double[3]);
	virtual double* GetOrigin()
	{
		double *org = new double[3];
		GetOrigin(org);
		return org;
	}

	virtual void SetSpacing(double sX, double sY, double sZ) {}

	virtual void SetSpacing(double spac[3]) {}

	virtual int GetNumberOfPositions();

	virtual int GetNumberOfDimensions();

	virtual void SetPositions(std::vector<int> pos);

	virtual std::vector<int> GetPositions() { return positions; }


private:
	STRING_VECTOR Split(const char *str, char c);

	void ResetBounds() 
	{
		if(bounds)
			delete[] bounds;
		bounds = NULL;
	}

	PointsReader *reader;
	std::vector<int> positions;

	double *bounds;
};

#endif //BRAINLAYOUT1D_HEADER