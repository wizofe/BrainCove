#ifndef ARENDERSYSTEM_HEADER
#define ARENDERSYSTEM_HEADER

#include "../DLLDefines.h"
#include <QtGui/QWidget>

#ifdef USE_NQVTK
	#include <NQVTK/Rendering/Renderer.h>
	#define Renderer NQVTK::Renderer
#else
	#include <vtkRenderer.h>
	#define Renderer vtkRenderer
#endif
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT ARenderSystem
{

protected:
	ARenderSystem();

public:
	virtual void SetupRenderer(QWidget* widget) = 0;

	virtual void Render() = 0;

	virtual Renderer *GetRenderer() = 0;


};

#endif //ARENDERABLE_HEADER