#ifndef BRAINLAYOUT_HEADER
#define BRAINLAYOUT_HEADER

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

class Rendering_EXPORT BrainLayout : public BaseLayout
{

public:
	BrainLayout();
	~BrainLayout();

	static BrainLayout* FromFile(std::string filePath);
	static BrainLayout* FromText(std::string text);

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

	virtual void GetSpacing(double[3]);
	virtual double* GetSpacing()
	{
		double *spacing = new double[3];
		GetSpacing(spacing);
		return spacing;
	}

	virtual int GetNumberOfPositions();

	virtual int GetNumberOfDimensions();

	virtual void SetPositions(std::vector<POINT3D> pos);

	virtual void SetSpacing(double sX, double sY, double sZ);

	virtual void SetSpacing(double spac[3])
	{
		SetSpacing(spac[0],spac[1],spac[2]);
	}

private:
	STRING_VECTOR Split(const char *str, char c);

	void ResetBounds() 
	{
		if(bounds)
			delete[] bounds;
		bounds = NULL;
	}

	PointsReader *reader;
	std::vector<POINT3D> positions;

	double *bounds;

	double spacing[3];
};

#endif //BRAINLAYOUT_HEADER