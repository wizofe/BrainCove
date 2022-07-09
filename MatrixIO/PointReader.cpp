#include "pointreader.h"

PointsReader::PointsReader(std::string fPath)
{
	Open(fPath);
}

PointsReader::~PointsReader()
{
	Close();
}

bool PointsReader::Open()
{
	if(!filePath.empty())
		fileStream.open(filePath.c_str(),std::ios_base::in);
	return fileStream.is_open();
}

bool PointsReader::Open(const std::string fPath)
{
	SetFileName(fPath);
	return Open();
}

void PointsReader::Close()
{
	if(fileStream.is_open())
		fileStream.close();
}

STRING_VECTOR PointsReader::Split(const char *str, char c)
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

std::vector<POINT3D>& PointsReader::GetPositions()
{
	std::vector<POINT3D> *points = new std::vector<POINT3D>();

	char buff[2048];

	while(!fileStream.eof())
	{
		fileStream.getline(buff,2048);

		std::vector<std::string> fStrs = Split(buff,',');
		
		if(fStrs.size() < 3)
			continue;
		
		POINT3D p;
		p.push_back((float)atof(fStrs[0].c_str()));
		p.push_back((float)atof(fStrs[1].c_str()));
		p.push_back((float)atof(fStrs[2].c_str()));
		points->push_back(p);
	}

	return *points;
}