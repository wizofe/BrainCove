#include "View2DInteractorStyle.h"

vtkStandardNewMacro(View2DInteractorStyle);

View2DInteractorStyle::View2DInteractorStyle() :
	vtkInteractorStyleImage()
{
	interact = true;
	middleDown = false;
}

View2DInteractorStyle::~View2DInteractorStyle()
{

}

void View2DInteractorStyle::Rotate()
{
	vtkInteractorStyleImage::Rotate();
}

void View2DInteractorStyle::Spin()
{
	vtkInteractorStyleImage::Spin();
}

void View2DInteractorStyle::Pan()
{
	vtkInteractorStyleImage::Pan();
}

void View2DInteractorStyle::Dolly()
{
	vtkInteractorStyleImage::Dolly();
}

void View2DInteractorStyle::OnMouseMove()
{
	if(middleDown)
	{
		int lxy[2];
		int cxy[2];
		this->Interactor->GetEventPosition(cxy);
		this->Interactor->GetLastEventPosition(lxy);
		
		int deltaY = lxy[1] - cxy[1];

		this->EndPan();

		vtkInteractorStyleImage::OnMouseMove();
		
		return;
	}

	vtkInteractorStyleImage::OnMouseMove();

	//if(!interact)
	//	return;

	int xy[2];

	this->Interactor->GetEventPosition(xy);

}

void View2DInteractorStyle::OnLeftButtonDown()
{
	interact = false;
	vtkInteractorStyleImage::OnLeftButtonDown();
}

void View2DInteractorStyle::OnLeftButtonUp()
{
	interact = true;
	vtkInteractorStyleImage::OnLeftButtonUp();
}

void View2DInteractorStyle::OnMiddleButtonDown()
{
	interact = false;
	middleDown = true;
	vtkInteractorStyleImage::OnMiddleButtonDown();
}

void View2DInteractorStyle::OnMiddleButtonUp()
{
	interact = true;
	middleDown = false;
	vtkInteractorStyleImage::OnMiddleButtonUp();
}

void View2DInteractorStyle::OnRightButtonDown()
{
	interact = false;
	vtkInteractorStyleImage::OnRightButtonDown();
}

void View2DInteractorStyle::OnRightButtonUp()
{
	interact = true;
	vtkInteractorStyleImage::OnRightButtonUp();
}

void View2DInteractorStyle::OnKeyPress()
{
	if(this->Interactor->GetKeyCode() == '+')
	{}
	else if(this->Interactor->GetKeyCode() == '-')
	{}
	vtkInteractorStyleImage::OnKeyPress();
}

void View2DInteractorStyle::OnMouseWheelForward() 
{
	vtkInteractorStyleImage::OnMouseWheelForward();
}

void View2DInteractorStyle::OnMouseWheelBackward()
{
	vtkInteractorStyleImage::OnMouseWheelBackward();
}

