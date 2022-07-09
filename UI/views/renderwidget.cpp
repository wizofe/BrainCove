#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent) 
: QWidget(parent) 
{
	this->setLayout(new QVBoxLayout(this));
	this->QRenderWidget = new QVTKWidget(this);
	this->layout()->addWidget(QRenderWidget);
	this->CameraReset = false;
	this->DefaultCamera = vtkCamera::New();
	this->VoxelPicker = NULL;
	this->InputFile = NULL;
	this->SlotConnector = vtkEventQtSlotConnect::New();
	this->MatrixMapper = NULL;
	SetupVolume();
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::SetupVolume()
{
	this->DummyVolume = vtkVolume::New();
	this->DummyVolume->SetPosition(0,0,0);
	this->DummyVolume->SetVisibility(false);
}

void RenderWidget::SetData(DataFile *data)
{
	this->InputFile = data;
	this->DataMatrix = data->matrix();
	Update();
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}

DataFile *RenderWidget::GetData()
{
	return this->InputFile;
}

void RenderWidget::SetComparisonData(DataFile *data)
{
	this->MatrixMapper->SetComparisonData(data);
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}

void RenderWidget::SetContext(HDC hDC, HGLRC hRC)
{
	vtkWin32OpenGLRenderWindow* renderWindow = reinterpret_cast<vtkWin32OpenGLRenderWindow*>(QRenderWidget->GetRenderWindow());
	if(renderWindow)
	{
		renderWindow->SetDeviceContext(hDC);
		renderWindow->SetContextId(hRC);
	}
}

void RenderWidget::SaveCamera()
{
	this->DefaultCamera->ShallowCopy(this->RenderSystem->GetRenderer()->GetActiveCamera());
}

void RenderWidget::RestoreCamera()
{
	this->RenderSystem->GetRenderer()->GetActiveCamera()->ShallowCopy(this->DefaultCamera);

    this->RenderSystem->GetRenderer()->GetActiveCamera()->Modified();
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}

void RenderWidget::ShareSelection(DataSelection *selection)
{
	for(int i = 0; i < this->SelectionObservers.size(); i++)
		this->SelectionObservers.at(i)->SetSelection(selection);
}

void RenderWidget::ShareView(vtkRenderer *renderer)
{
	for(int i = 0; i < this->InteractionObservers.size(); i++)
	{
		//Check if the observer is of the same ViewType
		if(this->InteractionObservers.at(i)->GetType() == this->GetType())
			this->InteractionObservers.at(i)->SetView(renderer);
	}
}

void RenderWidget::SetSelection(DataSelection *s)
{
	if(this->MatrixMapper)
	{
		this->MatrixMapper->SetSelection(s);
		this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
	}
}

void RenderWidget::SetView(vtkRenderer *renderer)
{
	this->RenderSystem->GetRenderer()->GetActiveCamera()->ShallowCopy(renderer->GetActiveCamera());
    this->RenderSystem->GetRenderer()->GetActiveCamera()->Modified();
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}