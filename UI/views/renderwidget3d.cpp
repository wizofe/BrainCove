#include "renderwidget3d.h"

RenderWidget3D::RenderWidget3D(QWidget *parent) 
: RenderWidget(parent) 
{
	this->RaycastVolume	= NULL;
	this->AtlasVolume	= NULL;
	this->MaskVolume	= NULL;
	this->ShiftDown     = false;
	this->SplitStart[0] = 0;
	this->SplitStart[1] = 0;
	
	this->Name = QString("3D Visualization");
	viewType = ViewType::Anatomical3D;

	SetupRenderer();
	SetupMapper();
	SetupLayout();
	SetupActions();
	SetupPicker();
}

RenderWidget3D::~RenderWidget3D()
{
	//if(this->RaycastVolume)
	//	this->RaycastVolume->Delete();
	this->RaycastVolume = NULL;
}

double totalFrames3D = 0;
double totalRenderTime3D = 0;
QElapsedTimer timer3D;

void CallbackFunction3D(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
	glFinish();

	qint64 elapsed = timer3D.elapsed();

	vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
  
	totalFrames3D++;
	
	totalRenderTime3D += (double)elapsed / 1000.0;

	double fps = totalFrames3D/totalRenderTime3D;
	std::cout << "3d FPS: " << fps << std::endl;

	int w,h;
	renderer->GetTiledSize(&w,&h);

	std::cout << "3d size: " << w << ", " << h << std::endl;
 
  //std::cout << "Callback" << std::endl;
}

void CallbackFunction3DS(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
  timer3D.restart();
}

void RenderWidget3D::SetData(DataFile *data)
{
	RenderWidget::SetData(data);
}

void RenderWidget3D::SetLayout(BaseLayout *lay)
{
	BrainLayout *brlay = static_cast<BrainLayout*>(lay);
	
	if(brlay)
		this->Layout = brlay;
}

void RenderWidget3D::SetupRenderer()
{
	//setup our render system
	this->RenderSystem = new VTKRenderSystem();
	this->RenderSystem->SetupRenderer(this->QRenderWidget);
	this->RenderSystem->GetRenderer()->ResetCamera();

	callback = vtkCallbackCommand::New();
	callback->SetCallback(CallbackFunction3D);

	callbackStart = vtkCallbackCommand::New();
	callbackStart->SetCallback(CallbackFunction3DS);

	this->RenderSystem->GetRenderer()->AddObserver(vtkCommand::StartEvent, callbackStart);
	this->RenderSystem->GetRenderer()->AddObserver(vtkCommand::EndEvent, callback);
  
  //dark gray
	this->RenderSystem->GetRenderer()->SetBackground(0.2333,0.2333,0.2333);
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();

	//add the dummy volume
	this->RenderSystem->GetRenderer()->AddVolume(this->DummyVolume);

	this->InteractorStyle = View3DInteractorStyle::New();
	this->ImageInteractorStyle = vtkInteractorStyleImage::New();

	this->SlotConnector->Connect(this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor(),vtkCommand::MouseMoveEvent,this,SLOT(on_renderWidget_mouseMove()));
	this->SlotConnector->Connect(this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor(),vtkCommand::MiddleButtonPressEvent,this,SLOT(on_renderWidget_middleButtonDown()));
	this->SlotConnector->Connect(this->InteractorStyle,vtkCommand::InteractionEvent,this,SLOT(on_renderWidget_viewChanged()));

	this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->InteractorStyle);

	this->UpdateAtlasLabels();

}

void RenderWidget3D::SetupMapper()
{
	this->MatrixMapper = GPUVolumeRayCastMapper::New();
	this->DummyVolume->SetMapper(this->MatrixMapper);

	std::string dataDir = std::string(DATA_DIR);
	std::string dataFile = std::string(dataDir).append("/brain2.dvtf");

	vtkLookupTable *atlascolors = ColorMapping::ImportFromDevideTF(dataFile,256);

	this->AtlasLabelActors = vtkActorCollection::New();

	this->LabelActor = vtkFollower::New();
	this->LabelSource = vtkVectorText::New();
	this->LabelMapper = vtkPolyDataMapper::New();
	/*this->LabelMapper->SetInput(this->LabelSource->GetOutput());
	this->LabelActor->SetMapper(this->LabelMapper);
	this->LabelActor->SetCamera(this->RenderSystem->GetRenderer()->GetActiveCamera());

	this->RenderSystem->GetRenderer()->AddActor(this->LabelActor);*/

	((GPUVolumeRayCastMapper*)this->MatrixMapper)->SetAtlasColors(atlascolors);
}

void RenderWidget3D::SetupLayout()
{
	this->Layout = new BrainLayout();
}

void RenderWidget3D::SetupActions()
{
	actionMouse_Hover = new QAction(QIcon(":/ui/icons/fatcow/32x32/zoom_layer.png"), tr("Enable/disable &hover"), this);
	actionMouse_Hover->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
	actionMouse_Hover->setStatusTip(tr("Enable or disable mouse hover"));
	actionMouse_Hover->setCheckable(true);
	actionMouse_Hover->setChecked(true);
	this->Actions.append(actionMouse_Hover);
	
	actionSave_Correlation = new QAction(QIcon(":/ui/icons/fatcow/32x32/layer_grid.png"), tr("Switch sampler"), this);
	actionSave_Correlation->setStatusTip(tr("Switch the sampler (inter-voxel interpolation)"));
	connect(actionSave_Correlation,SIGNAL(triggered()),this,SLOT(on_actionSave_Correlation_triggered()));

	this->Actions.append(actionSave_Correlation);

	actionChange_RayMethod = new QAction(QIcon(":/ui/icons/fatcow/32x32/brain.png"), tr("Switch 3d/flat &mapping"), this);
	actionChange_RayMethod->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_M));
	actionChange_RayMethod->setStatusTip(tr("Switch between 3d and flat visualization"));
	actionChange_RayMethod->setCheckable(true);
	actionChange_RayMethod->setChecked(true);
	connect(actionChange_RayMethod,SIGNAL(triggered()),this,SLOT(on_actionChange_RayMethod_triggered()));
	this->Actions.append(actionChange_RayMethod);
	
	actionFPS_calc = new QAction(tr("Calculate FPS"), this);
	actionFPS_calc->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
	actionFPS_calc->setStatusTip(tr("Enable or disable mouse hover"));
	this->Actions.append(actionFPS_calc);

	connect(actionFPS_calc,SIGNAL(triggered()),this,SLOT(on_actionFPS_calc_clicked()));
}

void RenderWidget3D::SetupPicker()
{
	this->VoxelPicker = new BrainVoxelSelector();
	this->PickWidget = new QVTKWidget();
	//this->layout()->addWidget(this->PickWidget);
}

vtkImageData *RenderWidget3D::GetRaycastVolume()
{
	return this->RaycastVolume;
}

vtkImageData *RenderWidget3D::GetAtlasVolume()
{
	if(!this->AtlasVolume)
	{
		VolumeReader2 = vtkXMLImageDataReader::New();
		std::string dataDir = std::string(DATA_DIR);
		std::string dataFile = std::string(dataDir).append("/aal_new.vti");
		VolumeReader2->SetFileName(dataFile.c_str());
		VolumeReader2->Update();
		this->AtlasVolume = VolumeReader2->GetOutput();
	}
	return this->AtlasVolume;
}

vtkImageData *RenderWidget3D::GetMaskVolume()
{
	return this->MaskVolume;
}

void RenderWidget3D::UpdateAtlasLabels()
{
	/*vtkPointSource *pointSource = vtkPointSource::New();
	pointSource->SetRadius(200);
	pointSource->SetCenter(180.5f/2.0f,212.0f/2.0f,152.5f/2.0f);
	pointSource->SetNumberOfPoints(6);
	pointSource->Update();

	// Add label array.
	vtkStringArray *labels = vtkStringArray::New();
	labels->SetNumberOfValues(6);
	labels->SetName("labels");
	labels->SetValue(0, "Priority 10");
	labels->SetValue(1, "Priority 7");
	labels->SetValue(2, "Priority 6");
	labels->SetValue(3, "Priority 4");
	labels->SetValue(4, "Priority 4");
	labels->SetValue(5, "Priority 4");
	pointSource->GetOutput()->GetPointData()->AddArray(labels);

	// Add priority array.
	vtkIntArray *sizes = vtkIntArray::New();
	sizes->SetNumberOfValues(6);
	sizes->SetName("sizes");
	sizes->SetValue(0, 2);
	sizes->SetValue(1, 2);
	sizes->SetValue(2, 2);
	sizes->SetValue(3, 2);
	sizes->SetValue(4, 2);
	sizes->SetValue(5, 2);
	pointSource->GetOutput()->GetPointData()->AddArray(sizes);

	vtkPointSetToLabelHierarchy *pointSetToLabelHierarchyFilter = vtkPointSetToLabelHierarchy::New();
	pointSetToLabelHierarchyFilter->SetInputConnection(pointSource->GetOutputPort());
	pointSetToLabelHierarchyFilter->SetLabelArrayName("labels");
	pointSetToLabelHierarchyFilter->SetPriorityArrayName("sizes");
	pointSetToLabelHierarchyFilter->Update();

	vtkLabelPlacementMapper *labelMapper = vtkLabelPlacementMapper::New();
	labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
	vtkActor2D *labelActor = vtkActor2D::New();
	labelActor->SetMapper(labelMapper);*/

	//this->RenderSystem->GetRenderer()->AddActor( labelActor );
	// create our own labels that will follow and face the camera
  /*vtkFollower *xLabel = vtkFollower::New();
  vtkVectorText *xText = vtkVectorText::New();
  vtkPolyDataMapper *xTextMapper = vtkPolyDataMapper::New();
  xText->SetText( "TEST TEST TEST" );
  xTextMapper->SetInputConnection( xText->GetOutputPort() );
  xLabel->SetMapper( xTextMapper );
  xLabel->SetScale( 3 );
  xLabel->SetCamera(this->RenderSystem->GetRenderer()->GetActiveCamera());
  xLabel->SetPosition(180.5f/2.0f,212.0f/2.0f,152.5f); // XAxisTip
  xLabel->PickableOff();
  this->RenderSystem->GetRenderer()->AddActor( xLabel );*/

}

void RenderWidget3D::Update()
{
	this->MatrixMapper->SetConnectivityData(this->InputFile);
	this->MatrixMapper->SetLayoutSystem(this->Layout);
	this->MatrixMapper->SetInput(GetRaycastVolume());
	this->MatrixMapper->SetAtlasVolume(GetAtlasVolume());

	this->VoxelPicker->SetPickData(this->GetMaskVolume(),this->MatrixMapper->GetVolumeMatrix(this->GetMaskVolume(),this->DummyVolume));
	this->VoxelPicker->SetVoxelData(this->MatrixMapper->GetInput());
	this->VoxelPicker->SetAtlasData(this->MatrixMapper->GetAtlasVolume(),this->MatrixMapper->GetVolumeMatrix(MatrixMapper->GetAtlasVolume(),this->DummyVolume));
	this->VoxelPicker->SetLayout(this->Layout);
	this->VoxelPicker->StartBrushing();

	this->DummyVolume->SetVisibility(this->InputFile != NULL);
	if(!this->CameraReset)
	{
		this->RenderSystem->GetRenderer()->ResetCamera();
		this->SaveCamera();
		this->CameraReset = true;
	}
}

void RenderWidget3D::on_actionFPS_calc_clicked()
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
	totalFrames3D = 0;
	totalRenderTime3D = 0;
	
	timer3D.restart();
}

void RenderWidget3D::on_renderWidget_mouseMove()
{
	if(!actionMouse_Hover->isChecked() || !this->InputFile)
	{
		if(this->InputFile) //we want to render, we just don't want to 'pick'
			this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
		return;
	}

	int pos[2];
	this->InteractorStyle->GetInteractor()->GetEventPosition(pos);
	DataSelection *selection = this->VoxelPicker->PickVolume(pos,this->RenderSystem->GetRenderer(),this->MatrixMapper->GetViewMatrix(),this->MatrixMapper->GetViewBounds());

	int angle = 0;

	if(this->InteractorStyle->GetInteractor()->GetShiftKey())
	{
		vtkMatrix4x4 *leftMatrix;
		vtkMatrix4x4 *rightMatrix;

		GPUVolumeRayCastMapper *mapper = static_cast<GPUVolumeRayCastMapper*>(this->GetMapper());
		BrainVoxelSelector *selector = static_cast<BrainVoxelSelector*>(this->GetSelector());
		CLTwoPassRayCaster *rayCaster = static_cast<CLTwoPassRayCaster*>(mapper->GetRayCaster());

		if(!this->ShiftDown)
		{
			this->ShiftDown = true;
			this->SplitStart[0] = pos[0];
			this->SplitStart[1] = pos[1];

			DataSelection *sel = ((BrainVoxelSelector*)this->VoxelPicker)->PickLastVolume3D(pos,this->RenderSystem->GetRenderer(),this->MatrixMapper->GetViewMatrix(),this->MatrixMapper->GetViewBounds());
		
			for(int i = 0;i<3;i++)
				this->PlanePos[i] = sel->position()[i];

			this->PlanePos[3] = 1;
			this->PlaneNorm[3] = 1;

			double normal[4];
			double normal2[4];

			vtkCamera *cam = this->RenderSystem->GetRenderer()->GetActiveCamera();
			cam->GetViewPlaneNormal(normal);
			cam->GetViewUp(this->RotateAxis);

			vtkTransform *tr = vtkTransform::New();
			tr->Identity();
			tr->RotateWXYZ(90,this->RotateAxis);
			tr->TransformVector(normal,normal2);
			tr->Delete();
		
			for(int i = 0;i<3;i++)
				this->PlaneNorm[i] = normal2[i];

		}
		else
		{
			angle = (this->SplitStart[0] - pos[0]);

			printf("%d\n",angle);

			if(angle > 90) angle = 90;
			if(angle < -90) angle = -90;
		}

		if(mapper)
		{
			selector->SetBrainSplit(this->PlanePos,this->PlaneNorm);
			selector->SetDisplacementFromAngle(angle,this->RotateAxis);

			leftMatrix = selector->GetLeftDisplacementMatrix();
			rightMatrix = selector->GetRightDisplacementMatrix();
			mapper->SetBrainSplit(this->PlanePos,this->PlaneNorm,angle);

			rayCaster->SetLeftDisplacementMatrix(leftMatrix);
			rayCaster->SetRightDisplacementMatrix(rightMatrix);
		}
	}
	else
	{
		this->MatrixMapper->SetSelection(selection);
		this->ShiftDown = false;
	}
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
	
	ShareSelection(selection);
}

void RenderWidget3D::on_renderWidget_middleButtonDown()
{
	actionMouse_Hover->setChecked(!actionMouse_Hover->isChecked());
}

void RenderWidget3D::on_renderWidget_viewChanged()
{
	ShareView(this->RenderSystem->GetRenderer());
}

void RenderWidget3D::on_actionSave_Correlation_triggered()
{
	((CLTwoPassRayCaster*)((GPUVolumeRayCastMapper*)this->MatrixMapper)->GetRayCaster())->SaveCorrelationVolume();
}

void RenderWidget3D::on_actionChange_RayMethod_triggered()
{
	if(actionChange_RayMethod->isChecked())
	{
		this->RestoreCamera();
		this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->InteractorStyle);		
		((BrainVoxelSelector*)this->VoxelPicker)->SetRenderType(RenderType::Template3D);
		((GPUVolumeRayCastMapper*)this->MatrixMapper)->SetRaySetupMethod(0);
	}
	else
	{
		this->SaveCamera();
		this->RenderSystem->GetRenderer()->ResetCamera();
		//renderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(ImageInteractorStyle);
		((BrainVoxelSelector*)this->VoxelPicker)->SetRenderType(RenderType::MappedCortex);
		((GPUVolumeRayCastMapper*)this->MatrixMapper)->SetRaySetupMethod(1);
	}
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}