#include "LinkedViewInteractorStyle.h"

LinkedViewInteractorStyle::LinkedViewInteractorStyle() 
	: vtkRenderWindowInteractor()
{
	interact = true;
	middleDown = false;
}

LinkedViewInteractorStyle::~LinkedViewInteractorStyle()
{

}

void LinkedViewInteractorStyle::setRenderer(vtkRenderer* render)
{
	renderer = render;
}

void LinkedViewInteractorStyle::on_viewPointChanged(vtkRenderer *otherRenderer)
{
	//viewport in other window changed, adjust to that!
	vtkCamera *myCam = renderer->GetActiveCamera();
	vtkCamera *otherCam = otherRenderer->GetActiveCamera();
	myCam->SetPosition(otherCam->GetPosition());
	myCam->SetFocalPoint(otherCam->GetFocalPoint());
	myCam->SetViewUp(otherCam->GetViewUp());
	myCam->SetViewAngle(otherCam->GetViewAngle());
	myCam->SetClippingRange(otherCam->GetClippingRange());
	renderer->GetRenderWindow()->Render();
}