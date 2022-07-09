#include "renderwidget2d.h"

RenderWidget2D::RenderWidget2D(QWidget *parent) 
: RenderWidget(parent)
{
	SetupRenderer();
	SetupMapper();
	SetupLayout();
	SetupActions();
	SetupPicker();
	this->viewType = ViewType::Matrix;
	this->Name = QString("2D Visualization");
}

RenderWidget2D::~RenderWidget2D()
{

}

double totalFrames = 0;
double totalRenderTime = 0;
QElapsedTimer timer;

void CallbackFunction2D(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
	qint64 elapsed = timer.elapsed();

	vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
  
	totalFrames++;
	
	totalRenderTime += (double)elapsed / 1000.0;

	double fps = totalFrames/totalRenderTime;
	std::cout << "2d FPS: " << fps << std::endl;

	int w,h;
	renderer->GetTiledSize(&w,&h);

	std::cout << "2d size: " << w << ", " << h << std::endl;
  
  //std::cout << "Callback" << std::endl;
}

void CallbackFunction2DS(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
	timer.restart();

	glFlush();

}
 
void RenderWidget2D::SetupRenderer()
{
	//setup our render system
	this->RenderSystem = new VTKRenderSystem();
	this->RenderSystem->SetupRenderer(this->QRenderWidget);
	this->RenderSystem->GetRenderer()->ResetCamera();

	callback = vtkCallbackCommand::New();
	callback->SetCallback(CallbackFunction2D);

	callbackStart = vtkCallbackCommand::New();
	callbackStart->SetCallback(CallbackFunction2DS);

	this->RenderSystem->GetRenderer()->AddObserver(vtkCommand::StartEvent, callbackStart);
	this->RenderSystem->GetRenderer()->AddObserver(vtkCommand::EndEvent, callback);

	//dark gray
	this->RenderSystem->GetRenderer()->SetBackground(0.2333,0.2333,0.2333);
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();

	//add the dummy volume
	this->RenderSystem->GetRenderer()->AddVolume(this->DummyVolume);

	this->RenderSystem->GetRenderer()->GetActiveCamera()->ParallelProjectionOn();

	this->InteractorStyle = View2DInteractorStyle::New();
	
	this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->InteractorStyle);

	this->SlotConnector = vtkEventQtSlotConnect::New();

	this->SlotConnector->Connect(this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor(),vtkCommand::MouseMoveEvent,this,SLOT(on_renderWidget_mouseMove()));
	this->SlotConnector->Connect(this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor(),vtkCommand::MiddleButtonPressEvent,this,SLOT(on_renderWidget_Toggle_hover()));

	this->SlotConnector->Connect(this->InteractorStyle,vtkCommand::InteractionEvent,this,SLOT(on_renderWidget_viewChanged()));
	this->SlotConnector->Connect(this->InteractorStyle,vtkCommand::LeftButtonPressEvent,this,SLOT(on_renderWidget_leftButtonDown()));
	this->SlotConnector->Connect(this->InteractorStyle,vtkCommand::LeftButtonReleaseEvent,this,SLOT(on_renderWidget_leftButtonUp()));

}

void RenderWidget2D::SetupMapper()
{
	this->MatrixMapper = GPUPlaneRayCastMapper::New();
	this->DummyVolume->SetMapper(this->MatrixMapper);
}

void RenderWidget2D::SetupLayout()
{
	
	this->Layout = new BrainLayout1D();
	//std::string csvFile = std::string(DATA_DIR);
	//std::string csvFile = std::string("C:\\andre_temp\\Data\\voxel_connectivity\\20918_voxels\\subj1");
	std::string csvFile = std::string("B:\\Data\\voxel_connectivity\\amc");
	csvFile = csvFile.append("/coords20k.csv");
	this->Layout->LoadPositionsFromFile(csvFile.c_str());
}


void RenderWidget2D::SetupActions()
{
	actionMouse_Hover = new QAction(QIcon(":/ui/icons/fatcow/32x32/zoom_layer.png"), tr("Enable/disable &hover"), this);
	actionMouse_Hover->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
	actionMouse_Hover->setStatusTip(tr("Enable or disable mouse hover"));
	actionMouse_Hover->setCheckable(true);
	actionMouse_Hover->setChecked(true);
	this->Actions.append(actionMouse_Hover);

	actionFPS_calc = new QAction(tr("Calculate FPS"), this);
	actionFPS_calc->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
	actionFPS_calc->setStatusTip(tr("Enable or disable mouse hover"));
	this->Actions.append(actionFPS_calc);

	connect(actionFPS_calc,SIGNAL(triggered()),this,SLOT(on_actionFPS_calc_clicked()));
}

void RenderWidget2D::SetupPicker()
{
	this->VoxelPicker = new MatrixVoxelSelector();
}

void RenderWidget2D::on_actionFPS_calc_clicked()
{
	/*
	double time = 0;
	for(int i = 0; i < 200; i++)
	{
		this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
		time+= this->RenderSystem->GetRenderer()->GetLastRenderTimeInSeconds();
	}
	double fps = 200.0/time;

	std::cout << "Average FPS: " << fps << std::endl;*/
	totalFrames = 0;
	totalRenderTime = 0;
	
	timer.restart();
}

void RenderWidget2D::on_actionMouse_Hover_clicked()
{
	int pos[2];
	this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->GetLastEventPosition(pos);
	
	this->InteractorStyle->Zoom();
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}

void RenderWidget2D::on_renderWidget_leftButtonDown()
{
	bool shiftPressed = this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->GetShiftKey();
	bool ctrlPressed = this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->GetControlKey();
	
	if(shiftPressed)
		this->InteractorStyle->StartPan();
	else if(ctrlPressed)
		this->InteractorStyle->StartSpin();
	else
		this->VoxelPicker->StartBrushing();
}

void RenderWidget2D::on_renderWidget_Toggle_hover()
{
	actionMouse_Hover->setChecked(!actionMouse_Hover->isChecked());
}

void RenderWidget2D::on_renderWidget_leftButtonUp()
{
	this->InteractorStyle->EndPan();
	this->InteractorStyle->EndSpin();
	this->VoxelPicker->EndBrushing();
}

void RenderWidget2D::on_renderWidget_mouseMove()
{
	if(!actionMouse_Hover->isChecked())
		return;

	int pos[2];
	this->InteractorStyle->GetInteractor()->GetEventPosition(pos);	
	
	DataSelection *selection = this->VoxelPicker->PickVolume(pos,this->RenderSystem->GetRenderer(),this->MatrixMapper->GetViewMatrix(),this->MatrixMapper->GetViewBounds());
	selection->SetLayout(this->Layout);
	
	this->MatrixMapper->SetSelection(selection);
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();

	ShareSelection(selection);
}

void RenderWidget2D::on_renderWidget_viewChanged()
{
	ShareView(this->RenderSystem->GetRenderer());
}

void RenderWidget2D::Update()
{
	this->MatrixMapper->SetConnectivityData(this->InputFile);

	this->VoxelPicker->SetPickData(this->MatrixMapper->GetInput(),this->MatrixMapper->GetVolumeMatrix(this->MatrixMapper->GetInput(),this->DummyVolume));
	this->VoxelPicker->SetVoxelData(this->MatrixMapper->GetInput());
	this->VoxelPicker->SetLayout(this->Layout);

	this->MatrixMapper->SetLayoutSystem(this->Layout);
	this->DummyVolume->SetVisibility(this->DataMatrix!=NULL);
	if(!this->CameraReset)
	{
		this->RenderSystem->GetRenderer()->ResetCamera();
		this->SaveCamera();
		this->CameraReset = true;
	}
}
