#ifndef POINTREADER_HEADER
#define POINTREADER_HEADER

#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>
#include "BaseMatrix.hpp"
#include "SquareMatrix.hpp"
#include "UpperTriangularMatrix.hpp"
#include "DLLDefines.h"

typedef std::string STRING_CLASS;
typedef std::vector<STRING_CLASS> STRING_VECTOR;
typedef std::vector<float> POINT3D;

/*
	exports for static functions:
	{
	  public:
		static MatrixIO_EXPORT void MyExportedFunction(int i);
	};
*/

class MatrixIO_EXPORT PointsReader
{
public: 
	PointsReader(std::string fPath);
	~PointsReader();

	bool Open();
	bool Open(const std::string fPath);

	void Close();

	void SetFileName(const std::string fPath) { filePath = fPath; }
	std::string GetFileName() const { return filePath; }

	std::vector<POINT3D>& GetPositions();

	int GetLength() const { return length; }

private:
	STRING_VECTOR Split(const char *str, char c = ' ');

	std::string filePath;
	std::ifstream fileStream;

	int length;
};

#endif //POINTREADER_HEADER