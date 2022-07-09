#ifndef NQVTKRENDERSYSTEM_HEADER
#define NQVTKRENDERSYSTEM_HEADER

#include "../DLLDefines.h"
#include <QtGui/QWidget>

#include <GL/glew.h>

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <NQVTK/Interactors/ArcballCameraInteractor.h>
#include <NQVTK/Rendering/ArcballCamera.h>
#include <NQVTK/Rendering/Scene.h>
#include <NQVTK/Rendering/LayeredRaycastingRenderer.h>
#include <NQVTK/Styles/LayeredRaycaster.h>

#include "ARenderSystem.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT NQVTKRenderSystem : public ARenderSystem
{

public:
	NQVTKRenderSystem();

	virtual void SetupRenderer(QWidget* widget);

private:
	NQVTK::LayeredRaycastingRenderer *renderer;
	NQVTK::Styles::LayeredRaycaster *style;
	NQVTK::ArcballCamera *camera;
	NQVTK::Scene *scene;

};

#endif //NQVTKRENDERSYSTEM_HEADER