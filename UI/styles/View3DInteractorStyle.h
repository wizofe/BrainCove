#ifndef VIEW3DINTERACTORSTYLE_H
#define VIEW3DINTERACTORSTYLE_H

#include <QObject>
#include <QPoint>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include "QInteractorStyle.h"

class View3DInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static View3DInteractorStyle *New();
	
	virtual void Rotate();
	virtual void Spin();
	virtual void Pan();
	virtual void Dolly();
	virtual void OnMouseMove();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnMiddleButtonDown();
	virtual void OnMiddleButtonUp();
	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnKeyPress();

	virtual void OnMouseWheelForward();
	virtual void OnMouseWheelBackward();

	QInteractorStyle *GetStyle() const { return style; }

protected:
	View3DInteractorStyle();
	~View3DInteractorStyle();

	void CopyViewSettings(vtkRenderer *otherRenderer)
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

private:
	vtkRenderer* renderer;
	bool interact;
	bool middleDown;
	QInteractorStyle *style;
};

#endif // VIEW3DINTERACTORSTYLE_H