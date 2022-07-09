#ifndef DATAFILE_HEADER
#define DATAFILE_HEADER

#include "DLLDefines.h"
#include "BaseMatrix.hpp"
#include "MatrixIO.h"
#include "PointReader.h"
#include <vector>
#include <QtXml/QDomDocument>
#include <QString>
#include <QFile>
#include <QFileInfo>

//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class MatrixIO_EXPORT DataFile
{

public:
	DataFile();
	~DataFile();

	bool ReadFromFile(const QString& filePath);

	std::vector<POINT3D> points() const { return _points; }
	BaseMatrix<float>* matrix() const { return _matrix; }

	inline double spacingX() { return spaceX; }
	inline double spacingY() { return spaceY; }
	inline double spacingZ() { return spaceZ; }

	inline double originX() { return origX; }
	inline double originY() { return origY; }
	inline double originZ() { return origZ; }

	QString fileName() { return matrixFile; }

protected:

private:
	STRING_VECTOR Split(const char *str, char c);
	bool SetValueFromNode(const QDomNode& node,double *component);

	BaseMatrix<float> *_matrix;
	QString matrixFile;
	std::vector<POINT3D> _points;
	MatrixReader *matrixReader;

	double spaceX;
	double spaceY;
	double spaceZ;

	double origX;
	double origY;
	double origZ;
};

#endif //DATAFILE_HEADER