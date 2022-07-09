#include "MatrixIO.h"

//-----------------------------------------------------------
MatrixReader::MatrixReader(std::string fPath)
{
	filePath = fPath;

	matSize = -1;
}

//-----------------------------------------------------------
MatrixReader::~MatrixReader()
{
	if(fileStream.is_open())
		fileStream.close();
}

//-----------------------------------------------------------
bool MatrixReader::Open()
{
	if(filePath.empty())
		throw std::exception("Input file is not set. Use SetFileName to set the input file.");

	fileStream.open(filePath.c_str(), std::ios_base::binary| std::fstream::in | std::ios_base::ate);
	
	if(fileStream.fail())
		throw std::exception("Error opening file.");

	//get the length of the file (in bytes)
	size_t length = fileStream.tellg();

	//guess the shape based on the file length
	GuessShape(length,isTriang);

	//reset position to start of stream
	fileStream.seekg(0);

	return true;
}

//-----------------------------------------------------------
bool MatrixReader::Open(std::string fPath)
{
	filePath = fPath;

	return Open();
}

//-----------------------------------------------------------
void MatrixReader::Close()
{
	fileStream.close();
}

//-----------------------------------------------------------
void MatrixReader::SetFileName(const std::string fPath)
{
	filePath = fPath;
}

//-----------------------------------------------------------
std::string MatrixReader::GetFileName() const
{
	return filePath;
}

//-----------------------------------------------------------
void MatrixReader::GuessShape(const size_t fSize, bool triangular)
{
	size_t dim = fSize;
	dim -= 32; //skip header
	dim *= 2; //make square (instead of triangular)
	dim /= 4; //floats are 4 bytes
	size_t dim_1 = (size_t)ceil(sqrt((double)dim));

	SetShape(dim_1, triangular);
}

//-----------------------------------------------------------
void MatrixReader::SetShape(size_t dim, bool triangular)
{
	SetShape(dim,dim,triangular);
}

//-----------------------------------------------------------
void MatrixReader::SetShape(size_t dim1, size_t dim2, bool triangular)
{
	dimension1 = dim1;
	dimension2 = dim2;
	isTriang = triangular;

	if(isTriang)
		matSize = (int)(dimension1*(dimension2-1)*0.5);
	else
		matSize = (int)(dimension1*dimension2);
}

//-----------------------------------------------------------
size_t MatrixReader::GetSize()
{
	return matSize;
}

//-----------------------------------------------------------
bool MatrixReader::MatrixIsTriangular()
{
	return isTriang;
}

//-----------------------------------------------------------
bool MatrixReader::ReadAsFloatArray(float** arr)
{
	if(matSize < 0)
		throw std::exception("The matrix size has not been set. Cannot read a file of arbitrary size. Use SetSize to set the matrix size before calling a Read funtion.");
	
	*arr = new float[matSize];

	if(!fileStream.is_open())
		Open();

	//skip header
	fileStream.seekg(32);

	//fill float array with data from the binary file
	fileStream.read((char*)(*arr),sizeof(float)*matSize);

	return true;
}

//-----------------------------------------------------------
BaseMatrix<float>* MatrixReader::ReadAsMatrix()
{
	if(MatrixIsTriangular())
		return ReadAsTriangularMatrix();
	else
		return ReadAsSquareMatrix();

}

//-----------------------------------------------------------
UpperTriangularMatrix<float>* MatrixReader::ReadAsTriangularMatrix()
{
	if(matSize < 0)
		throw std::exception("The matrix size has not been set. Cannot read a file of arbitrary size. Use SetSize to set the matrix size before calling a Read funtion.");

	UpperTriangularMatrix<float> *matrix = new UpperTriangularMatrix<float>(dimension1);

	if(!fileStream.is_open())
		Open();

	//skip header
	fileStream.seekg(32);

	//fill raw float array with data from the binary file
	fileStream.read((char*)matrix->RawData(),sizeof(float)*matrix->NumElements());

	return matrix;
}

//-----------------------------------------------------------
SquareMatrix<float>* MatrixReader::ReadAsSquareMatrix()
{
	if(matSize < 0)
		throw std::exception("The matrix size has not been set. Cannot read a file of arbitrary size. Use SetSize to set the matrix size before calling a Read funtion.");

	SquareMatrix<float> *matrix = new SquareMatrix<float>(dimension1,dimension2);

	if(!fileStream.is_open())
		Open();

	//skip header
	fileStream.seekg(32);

	//fill raw float array with data from the binary file
	fileStream.read((char*)matrix->RawData(),sizeof(float)*matrix->NumElements());

	return matrix;
}
