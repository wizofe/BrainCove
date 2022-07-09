#include "BrainLayout.h"

BrainLayout::BrainLayout() : BaseLayout(), reader(NULL), positions(NULL), bounds(NULL)
{
}

BrainLayout::~BrainLayout()
{
	if(reader)
		delete reader;
}

BrainLayout* BrainLayout::FromFile(std::string filePath)
{
	BrainLayout *layout = new BrainLayout();
	layout->LoadPositionsFromFile(filePath);
	return layout;
}

BrainLayout* BrainLayout::FromText(std::string text)
{
	BrainLayout *layout = new BrainLayout();
	layout->LoadPositionsFromText(text);
	return layout;
}

void BrainLayout::LoadPositionsFromFile(std::string filePath)
{
	if(reader)
		delete reader;

	reader = new PointsReader(filePath);
	
	positions.clear();
	positions = reader->GetPositions();

	ResetBounds();

}

void BrainLayout::SetPositions(std::vector<POINT3D> pos)
{
	positions.clear();
	positions = pos;
	ResetBounds();
}

STRING_VECTOR BrainLayout::Split(const char *str, char c)
{
	STRING_VECTOR result;

	while(true)
	{
		const char *begin = str;

		while(*str != c && *str)
			str++;

		result.push_back(std::string(begin, str));

		if(0 == *str++)
			break;
	}

	return result;
}

void BrainLayout::LoadPositionsFromText(std::string text)
{
	positions.clear();
	
	char buff[2048];

	std::stringstream stream(text);

	while(!stream.eof())
	{
		stream.getline(buff,2048);

		std::vector<std::string> fStrs = Split(buff,',');
		
		if(fStrs.size() < 3)
			continue;
		
		POINT3D p;
		p.push_back((float)atof(fStrs[0].c_str()));
		p.push_back((float)atof(fStrs[1].c_str()));
		p.push_back((float)atof(fStrs[2].c_str()));
		positions.push_back(p);
	}

	ResetBounds();
}

void BrainLayout::GetPosition(int index, double pos[3])
{
	pos[0] = (double)positions[index][0];
	pos[1] = (double)positions[index][1];
	pos[2] = (double)positions[index][2];
}

void BrainLayout::GetBounds(double bds[6])
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
			x = positions[i][0];
			y = positions[i][1];
			z = positions[i][2];
			if(x > bounds[1])
				bounds[1] = x;
			if(x < bounds[0])
				bounds[0] = x;
			if(y > bounds[3])
				bounds[3] = y;
			if(y < bounds[2])
				bounds[2] = y;
			if(z > bounds[5])
				bounds[5] = z;
			if(z < bounds[4])
				bounds[4] = z;
		}
	}
	for(int i = 0;i<6;i++)
		bds[i] = bounds[i];
}

void BrainLayout::GetDimensions(double dim[3])
{
	if(positions.empty())
		dim[0] = dim[1] = dim[2] = 0;
	else
	{
		double bnds[6];
		GetBounds(bnds);
		dim[0] = bnds[1] - bnds[0];
		dim[1] = bnds[3] - bnds[2];
		dim[2] = bnds[5] - bnds[4];
	}
}

void BrainLayout::GetSpacing(double spc[3])
{
	for(int i = 0;i<3;i++)
		spc[i] = spacing[i];
}

void BrainLayout::GetOrigin(double org[3])
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

int BrainLayout::GetNumberOfPositions()
{
	return positions.size();
}

int BrainLayout::GetNumberOfDimensions()
{
	return 3;
}

void BrainLayout::SetSpacing(double sX, double sY, double sZ)
{
	spacing[0] = sX;
	spacing[1] = sY;
	spacing[2] = sZ;
}