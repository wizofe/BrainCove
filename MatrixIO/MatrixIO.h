#ifndef MATRIXIO_HEADER
#define MATRIXIO_HEADER

#include <fstream>
#include <string>
#include <math.h>
#include "BaseMatrix.hpp"
#include "SquareMatrix.hpp"
#include "UpperTriangularMatrix.hpp"
#include "DLLDefines.h"

/*
	exports for static functions:
	{
	  public:
		static MatrixIO_EXPORT void MyExportedFunction(int i);
	};
*/

class MatrixIO_EXPORT MatrixReader
{
public: 
	MatrixReader(std::string fPath);
	~MatrixReader();

	bool Open();
	bool Open(const std::string fPath);

	void Close();

	void SetFileName(const std::string fPath);
	std::string GetFileName() const;

	void GuessShape(size_t fSize, bool triangular);
	void SetShape(size_t dimension1, size_t dimension2, bool triangular);
	void SetShape(size_t dimension, bool triangular);
	
	size_t GetSize();
	size_t GetDimension();
	bool MatrixIsTriangular();

	bool ReadAsFloatArray(float** arr);

	BaseMatrix<float>* ReadAsMatrix();
	UpperTriangularMatrix<float>* ReadAsTriangularMatrix();
	SquareMatrix<float>* ReadAsSquareMatrix();

private:
	std::string filePath;
	std::ifstream fileStream;

	bool isTriang;
	size_t dimension1;
	size_t dimension2;
	int matSize;
};

#endif