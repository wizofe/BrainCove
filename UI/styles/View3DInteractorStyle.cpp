#include "View3DInteractorStyle.h"

vtkStandardNewMacro(View3DInteractorStyle);

View3DInteractorStyle::View3DInteractorStyle() :
	vtkInteractorStyleTrackballCamera()
{
	interact = true;
	middleDown = false;
}

View3DInteractorStyle::~View3DInteractorStyle()
{

}

void View3DInteractorStyle::Rotate()
{
	vtkInteractorStyleTrackballCamera::Rotate();
	this->InvokeEvent(vtkCommand::InteractionEvent);
}

void View3DInteractorStyle::Spin()
{
	vtkInteractorStyleTrackballCamera::Spin();
	this->InvokeEvent(vtkCommand::InteractionEvent);
}

void View3DInteractorStyle::Pan()
{
	vtkInteractorStyleTrackballCamera::Pan();
	this->InvokeEvent(vtkCommand::InteractionEvent);
}

void View3DInteractorStyle::Dolly()
{
	vtkInteractorStyleTrackballCamera::Dolly();
	this->InvokeEvent(vtkCommand::InteractionEvent);

}

void View3DInteractorStyle::OnMouseMove()
{
	if(middleDown)
	{
		this->EndPan();
		vtkInteractorStyleTrackballCamera::OnMouseMove();
		return;
	}

	vtkInteractorStyleTrackballCamera::OnMouseMove();

	if(!interact)
		return;

	//int *xy = new int[2];

	//this->Interactor->GetEventPosition(xy);

	//this->InvokeEvent(vtkCommand::MouseMoveEvent, xy);

}

void View3DInteractorStyle::OnLeftButtonDown()
{
	interact = false;

	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

	//this->InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
}

void View3DInteractorStyle::OnLeftButtonUp()
{
	interact = true;
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();

	//this->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
}

void View3DInteractorStyle::OnMiddleButtonDown()
{
	interact = false;
	middleDown = true;
	vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();

	//this->InvokeEvent(vtkCommand::MiddleButtonPressEvent, NULL);
}

void View3DInteractorStyle::OnMiddleButtonUp()
{
	interact = true;
	middleDown = false;
	vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();

	//this->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, NULL);
}

void View3DInteractorStyle::OnRightButtonDown()
{
	interact = false;
	vtkInteractorStyleTrackballCamera::OnRightButtonDown();

	//this->InvokeEvent(vtkCommand::RightButtonPressEvent, NULL);
}

void View3DInteractorStyle::OnRightButtonUp()
{
	interact = true;
	vtkInteractorStyleTrackballCamera::OnRightButtonUp();

	//this->InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
}

void View3DInteractorStyle::OnKeyPress()
{
	if(this->Interactor->GetKeyCode() == '+')
	{}
	else if(this->Interactor->GetKeyCode() == '-')
	{}
	vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void View3DInteractorStyle::OnMouseWheelForward() 
{
	vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
	this->InvokeEvent(vtkCommand::InteractionEvent);
}

void View3DInteractorStyle::OnMouseWheelBackward()
{
	vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
	this->InvokeEvent(vtkCommand::InteractionEvent);
}
