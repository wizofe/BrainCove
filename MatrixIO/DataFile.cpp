#include "DataFile.h"

DataFile::DataFile()
{
	_matrix = NULL;
	spaceX = spaceY = spaceZ = 1.0;
}

DataFile::~DataFile()
{
	if(matrixReader)
	{
		delete matrixReader;
		matrixReader = NULL;
	}
	if(_matrix)
	{
		delete _matrix;
		_matrix = NULL;
	}
	_points.clear();
}

STRING_VECTOR DataFile::Split(const char *str, char c)
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

bool DataFile::ReadFromFile(const QString& filePath)
{
	QFile f(filePath);

	matrixFile = QFileInfo(filePath).fileName();

	QDomDocument doc;
	doc.setContent(&f);

	//get the matrix file name
	QDomNodeList nodes = doc.elementsByTagName(QString("dataFile"));
	if(nodes.length() == 0)
		return false;

	//read the matrix file
	if(!nodes.at(0).hasChildNodes())
		return false;
	QDomNode firstNode = nodes.at(0).firstChild();
	QString nodeValue = firstNode.nodeValue();

	QString fullPath = QFileInfo(filePath).absolutePath()+QLatin1String("/")+nodeValue;
	
	std::string fp = std::string(fullPath.toLatin1().data());

	matrixReader = new MatrixReader(fp);
	matrixReader->Open();

	if(_matrix)
		delete _matrix;

	_matrix = matrixReader->ReadAsTriangularMatrix();
	matrixReader->Close();

	//read the coordinates
	nodes = doc.elementsByTagName(QString("dataCoords"));
	if(nodes.length() == 0)
		return false;

	if(!nodes.at(0).hasChildNodes())
		return false;

	firstNode = nodes.at(0).firstChild();
	nodeValue = firstNode.nodeValue();

	_points.clear();
	
	char buff[2048];
	std::stringstream stream(nodeValue.toStdString());

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
		_points.push_back(p);
	}

	if(_points.size() != _matrix->Dimension())
		return false;

	//read the coordinates
	nodes = doc.elementsByTagName(QString("dataSpacing"));
	if(nodes.length() == 0 || nodes.at(0).childNodes().count() != 3)
		return false;

	firstNode = nodes.at(0);

	if(!SetValueFromNode(firstNode.childNodes().at(0),&spaceX))
		return false;
	if(!SetValueFromNode(firstNode.childNodes().at(1),&spaceY))
		return false;
	if(!SetValueFromNode(firstNode.childNodes().at(2),&spaceZ))
		return false;

	//read the coordinates
	nodes = doc.elementsByTagName(QString("dataOrigin"));
	if(nodes.length() == 0 || nodes.at(0).childNodes().count() != 3)
		return false;

	firstNode = nodes.at(0);

	if(!SetValueFromNode(firstNode.childNodes().at(0),&origX))
		return false;
	if(!SetValueFromNode(firstNode.childNodes().at(1),&origY))
		return false;
	if(!SetValueFromNode(firstNode.childNodes().at(2),&origZ))
		return false;

	return true;
}

bool DataFile::SetValueFromNode(const QDomNode& node,double *component)
{
	if(!component || !node.hasChildNodes())
		return false;

	bool ok = true;
	*component = node.firstChild().nodeValue().toDouble(&ok);
	return ok;

}