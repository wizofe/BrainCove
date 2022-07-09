#include "BrainLayout1D.h"

BrainLayout1D::BrainLayout1D() : BaseLayout(), reader(NULL), positions(NULL), bounds(NULL)
{
}

BrainLayout1D::~BrainLayout1D()
{
	if(reader)
		delete reader;
}

BrainLayout1D* BrainLayout1D::FromFile(std::string filePath)
{
	BrainLayout1D *layout = new BrainLayout1D();
	layout->LoadPositionsFromFile(filePath);
	return layout;
}

BrainLayout1D* BrainLayout1D::FromText(std::string text)
{
	BrainLayout1D *layout = new BrainLayout1D();
	layout->LoadPositionsFromText(text);
	return layout;
}

void BrainLayout1D::LoadPositionsFromFile(std::string filePath)
{
	std::ifstream stream(filePath.c_str());
	char buff[2048];

	while(!stream.eof())
	{
		stream.getline(buff,2048);
		positions.push_back((float)atof(buff));
	}
	ResetBounds();
}

void BrainLayout1D::SetPositions(std::vector<int> pos)
{
	positions.clear();
	positions = pos;
	ResetBounds();
}

void BrainLayout1D::LoadPositionsFromText(std::string text)
{
	positions.clear();
	
	char buff[2048];

	std::stringstream stream(text);

	while(!stream.eof())
	{
		stream.getline(buff,2048);

		positions.push_back((float)atof(buff));
	}

	ResetBounds();
}

void BrainLayout1D::GetPosition(int index, double pos[3])
{
	pos[0] = (double)positions[index];
	pos[1] = 0;
	pos[2] = 0;
}

void BrainLayout1D::GetBounds(double bds[6])
{
	if(positions.empty())
	{
		bds[0] = bds[1] = bds[2] = bds[3] = bds[4] = bds[5] = 0;
		return;
	}
	if(!bounds)
	{
		float x,y,z;
		bounds = new double[6];
		bounds[0] = bounds[2] = bounds[4] = FLT_MAX;
		bounds[1] = bounds[3] = bounds[5] = (-FLT_MAX);

		for(int i = 0;i<positions.size();i++)
		{
			x = positions[i];
			if(x > bounds[1])
				bounds[1] = x;
			if(x < bounds[0])
				bounds[0] = x;
		}
		bounds[2] = bounds[3] = bounds[4] = bounds[5] = 0;
	}
	for(int i = 0;i<6;i++)
		bds[i] = bounds[i];
}

void BrainLayout1D::GetDimensions(double dim[3])
{
	if(positions.empty())
		dim[0] = dim[1] = dim[2] = 0;
	else
	{
		double bnds[6];
		GetBounds(bnds);
		dim[0] = bnds[1] - bnds[0];
		dim[1] = dim[2] = 0;
	}
}

void BrainLayout1D::GetOrigin(double org[3])
{
	if(positions.empty())
		org[0] = org[1] = org[2] = 0;
	else
	{
		double bnds[6];
		GetBounds(bnds);
		org[0] = bnds[0];
		org[1] = bnds[2];
		org[2] = bnds[4];
	}
}

int BrainLayout1D::GetNumberOfPositions()
{
	return positions.size();
}

int BrainLayout1D::GetNumberOfDimensions()
{
	return 1;
}