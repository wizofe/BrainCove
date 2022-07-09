#ifndef NQVTKRENDERABLE_HEADER
#define NQVTKRENDERABLE_HEADER

#include "DLLDefines.h"
#include "ARenderable.h"
#include "NQVTK/Renderables/Renderable.h"

#include "GLBlaat/GL.h"
#include "GLBlaat/GLBuffer.h"
#include "GLBlaat/GLProgram.h"
#include "GLBlaat/GLShader.h"
#include <QTextStream>
#include <QFile>
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
class AttributeSet;

class Rendering_EXPORT NQVTKRenderable : public ARenderable
{

public:
	NQVTKRenderable();

	virtual void Draw() const = 0;

protected:
	GLProgram *CreateShader();
	std::string GetSourceFromFile(const char* filePath);
	GLProgram *shader;
	NQVTK::Vector3 color;
	GLfloat opacity;
};

#endif //NQVTKRENDERABLE_HEADER