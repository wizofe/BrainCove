#include "NQVTKRenderSystem.h"

NQVTKRenderSystem::NQVTKRenderSystem()
{
}

void NQVTKRenderSystem::SetupRenderer(QWidget* widget)
{
	glewInit();

	NQVTKWidget *nqWidget = (NQVTKWidget*)widget;

	renderer = new NQVTK::LayeredRaycastingRenderer();

	style = new NQVTK::Styles::LayeredRaycaster();
	renderer->SetStyle(style);

	camera = new NQVTK::ArcballCamera();
	renderer->SetCamera(camera);

	nqWidget->SetRenderer(renderer);
	nqWidget->SetInteractor(new NQVTK::ArcballCameraInteractor(camera));

	scene = new NQVTK::Scene();

	renderer->SetScene(scene);
}