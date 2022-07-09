#include "VTKRenderSystem.h"

VTKRenderSystem::VTKRenderSystem()
{
}

void VTKRenderSystem::SetupRenderer(QWidget* widget)
{
	QVTKWidget *qWidget = (QVTKWidget*)widget;

	renderer = vtkRenderer::New();

	interactor = vtkRenderWindowInteractor::New();

	style = vtkInteractorStyleTrackballCamera::New();
	
	interactor->SetInteractorStyle(style);

	qWidget->GetRenderWindow()->AddRenderer(renderer);

	qWidget->GetRenderWindow()->SetInteractor(interactor);

	qWidget->GetRenderWindow()->Render();

}

Renderer *VTKRenderSystem::GetRenderer()
{
	return renderer;
}

void VTKRenderSystem::Render()
{
	renderer->GetRenderWindow()->Render();
}
