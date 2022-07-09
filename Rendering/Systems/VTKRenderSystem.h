#ifndef VTKRENDERSYSTEM_HEADER
#define VTKRENDERSYSTEM_HEADER

#include "../DLLDefines.h"
#include <QtGui/QWidget>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "ARenderSystem.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>

//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT VTKRenderSystem : public ARenderSystem
{

public:
	VTKRenderSystem();

	virtual void SetupRenderer(QWidget* widget);
	virtual Renderer *GetRenderer();

	virtual void Render();

private:
	vtkRenderer *renderer;
	vtkRenderWindowInteractor *interactor;
	vtkInteractorStyle *style;

};

#endif //VTKRENDERSYSTEM_HEADER