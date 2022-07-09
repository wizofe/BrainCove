#include "mdiwidget.h"

MDIWidget::MDIWidget(QWidget *parent, HGLRC hglRC) 
: QMainWindow(parent) 
{
	this->CentralWidget = new QWidget(this);
	this->setCentralWidget(this->CentralWidget);
	
	this->CentralWidget->setContentsMargins(0,0,0,0);

	this->Splitter = new QSplitter(this->CentralWidget);
    this->Splitter->setOrientation(Qt::Vertical);

	this->TopSplitter = new QSplitter(this->CentralWidget);
    this->TopSplitter->setOrientation(Qt::Horizontal);

	this->Widget2D = new RenderWidget2D(this->TopSplitter);
	this->Widget3D = new RenderWidget3D(this->Splitter);
	this->SlicerWidget = new SliceViewsWidget(this->TopSplitter);

	this->Widget2D->setContentsMargins(0,0,0,0);
	this->Widget3D->setContentsMargins(0,0,0,0);
	this->SlicerWidget->setContentsMargins(0,0,0,0);
		
	this->WindowToolBar = new QToolBar(this);
	this->WindowToolBar->setIconSize(QSize(16,16));
	this->WindowToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

	this->addToolBar(Qt::RightToolBarArea, this->WindowToolBar);

	this->VerticalLayout = new QVBoxLayout();
    this->VerticalLayout->setSpacing(6);
    this->VerticalLayout->setContentsMargins(0, 0, 0, 0);

	this->TopSplitter->addWidget(this->Widget2D);
	this->TopSplitter->addWidget(this->SlicerWidget);
		
	this->VerticalLayout->addWidget(this->Splitter);
	this->Splitter->addWidget(this->TopSplitter);
	this->Splitter->addWidget(this->Widget3D);
	this->Splitter->setContentsMargins(0,0,0,0);
	//this->Splitter->setStyleSheet("QSplitterHandle:hover {} QSplitter::handle:horizontal:hover {background-color:red;}");
	
	this->centralWidget()->setLayout(this->VerticalLayout);

	this->CorrelationThresholdSlider = new QSlider(this->WindowToolBar);
	this->CorrelationThresholdSlider->setOrientation(Qt::Vertical);
	this->CorrelationThresholdSlider->setMinimum(0);
	this->CorrelationThresholdSlider->setValue(60);
	this->CorrelationThresholdSlider->setMaximum(100);

	this->SharedContext = hglRC;

	this->MaskVolume = NULL;
	this->PointsToVolumeConverter	= NULL;
	this->Layout = NULL;

	SetupActions();
	LinkWidgetContexts();
	on_actionChange_ViewLink_triggered();
}

MDIWidget::~MDIWidget()
{
}

void MDIWidget::SetupActions()
{
	QList<QAction*> q1Actions = this->Widget2D->GetActions();
	this->WindowToolBar->addActions(q1Actions);

	QList<QAction*> q2Actions = this->Widget3D->GetActions();

	this->WindowToolBar->addSeparator();
	this->WindowToolBar->addActions(q2Actions);

	this->LinkViewsAction = new QAction(QIcon(":/ui/icons/fatcow/32x32/link.png"), tr("Enable/disable &link"), this);
	this->LinkViewsAction->setStatusTip(tr("Enable or disable view link"));
	this->LinkViewsAction->setCheckable(true);
	this->LinkViewsAction->setChecked(true);
	connect(this->LinkViewsAction,SIGNAL(triggered()),this,SLOT(on_actionChange_ViewLink_triggered()));

	this->WindowToolBar->addSeparator();
	this->WindowToolBar->addAction(this->LinkViewsAction);

	this->WindowToolBar->addWidget(this->CorrelationThresholdSlider);
	connect(this->CorrelationThresholdSlider,SIGNAL(valueChanged(int)),this,SLOT(on_actionThreshold_changed()));
	
}

void MDIWidget::LinkWidgetContexts()
{
	vtkWin32OpenGLRenderWindow *w1 = vtkWin32OpenGLRenderWindow::SafeDownCast(this->Widget2D->GetRenderWindow());
	vtkWin32OpenGLRenderWindow *w2 = vtkWin32OpenGLRenderWindow::SafeDownCast(this->Widget3D->GetRenderWindow());
	HGLRC w1rc = (HGLRC)w1->GetGenericDisplayId();
	HGLRC w2rc = (HGLRC)w2->GetGenericDisplayId();

	bool ok = false;
	if(this->SharedContext)
	{
		ok = wglShareLists(this->SharedContext,w1rc);
		ok = wglShareLists(this->SharedContext,w2rc);
	}
	else
	{
		if(!wglShareLists(w1rc,w2rc))
		{
			this->SharedContext = w1rc;
			wglShareLists(w2rc,w1rc);
		}
		else
			this->SharedContext = w2rc;
	}
	DWORD err = GetLastError();
}

DataFile *MDIWidget::GetData()
{
	return this->Widget2D->GetData();
}

void MDIWidget::SetData(DataFile *data)
{
	BrainLayout *brainLayout = new BrainLayout();
	brainLayout->SetPositions(data->points());
	brainLayout->SetSpacing(data->spacingX(),data->spacingY(),data->spacingZ());

	this->setWindowTitle(data->fileName());

	this->Layout = brainLayout;

	this->Widget3D->SetLayout(brainLayout);
	this->SlicerWidget->SetLayout(brainLayout);

	this->Widget3D->SetMaskVolume(this->GetMaskVolume());
	this->SlicerWidget->SetMaskVolume(this->GetMaskVolume());

	this->Widget2D->SetData(data);
	this->Widget3D->SetData(data);
	this->SlicerWidget->SetData(data);

	on_actionThreshold_changed();
}

void MDIWidget::SetComparisonData(DataFile *data)
{
	this->Widget2D->SetComparisonData(data);
	this->Widget3D->SetComparisonData(data);

	on_actionThreshold_changed();
}

void MDIWidget::SetHemisphereSeparation(int angle)
{
	BrainVoxelSelector *selector = static_cast<BrainVoxelSelector*>(this->Widget3D->GetSelector());
	double viewUp[4] = {0,0,1,1};
	float normal[4] = {-1,0,0,1};
	float position[4] = {180.5f/2.0f,30,152.5f/2.0f,1};
	//this->Widget3D->GetRenderSystem()->GetRenderer()->GetActiveCamera()->GetViewUp(viewUp);

	selector->SetBrainSplit(position,normal);
	selector->SetDisplacementFromAngle(-angle,viewUp);
	vtkMatrix4x4 *leftMatrix = selector->GetLeftDisplacementMatrix();
	vtkMatrix4x4 *rightMatrix = selector->GetRightDisplacementMatrix();

	GPUVolumeRayCastMapper *mapper = static_cast<GPUVolumeRayCastMapper*>(this->Widget3D->GetMapper());
	if(mapper)
	{
		CLTwoPassRayCaster *rayCaster = static_cast<CLTwoPassRayCaster*>(mapper->GetRayCaster());
		rayCaster->SetBrainSplit(position,normal,0);
		rayCaster->SetLeftDisplacementMatrix(leftMatrix);
		rayCaster->SetRightDisplacementMatrix(rightMatrix);
		this->Widget3D->GetRenderWindow()->Render();
	}

}

void MDIWidget::SetRenderModeToComparison()
{
	this->Widget2D->GetMapper()->SetRenderModeToComparison();
	this->Widget3D->GetMapper()->SetRenderModeToComparison();
}

void MDIWidget::SetRenderModeToNormal()
{
	this->Widget2D->GetMapper()->SetRenderModeToNormal();
	this->Widget3D->GetMapper()->SetRenderModeToNormal();
}

void MDIWidget::RebuildKernels()
{
	this->Widget2D->GetMapper()->GetRayCaster()->Rebuild();
	this->Widget3D->GetMapper()->GetRayCaster()->Rebuild();
}

void MDIWidget::on_actionChange_ViewLink_triggered()
{
	if(this->LinkViewsAction->isChecked())
	{
		this->Widget2D->AddSelectionObserver(this->Widget3D);
		this->Widget3D->AddSelectionObserver(this->Widget2D);
		this->Widget2D->AddSelectionObserver(this->SlicerWidget);
		this->Widget3D->AddSelectionObserver(this->SlicerWidget);
		this->SlicerWidget->AddSelectionObserver(this->Widget3D);
		this->SlicerWidget->AddSelectionObserver(this->Widget2D);
	}
	else
	{
		this->Widget2D->RemoveSelectionObserver(this->Widget3D);
		this->Widget3D->RemoveSelectionObserver(this->Widget2D);
		this->Widget2D->RemoveSelectionObserver(this->SlicerWidget);
		this->Widget3D->RemoveSelectionObserver(this->SlicerWidget);
		this->SlicerWidget->RemoveSelectionObserver(this->Widget3D);
		this->SlicerWidget->RemoveSelectionObserver(this->Widget2D);
	}
}

void MDIWidget::on_actionThreshold_changed()
{
	float value = (float)this->CorrelationThresholdSlider->value() / 100.0f;

	this->Widget2D->GetMapper()->RemoveFilter(this->Filter);
	this->Widget3D->GetMapper()->RemoveFilter(this->Filter);

	DataFilter filter;
	filter.target = FilterTarget::CORRELATION_FILTER;
	filter.range = FilterRange::FILTER_ABOVE;
	filter.rangeStart = value;
	this->Filter = filter;

	this->Widget2D->GetMapper()->AddFilter(this->Filter);
	this->Widget2D->GetRenderWindow()->Render();
	this->Widget3D->GetMapper()->AddFilter(this->Filter);
	this->Widget3D->GetRenderWindow()->Render();
}

int MDIWidget::LinkContexts(const MDIWidget *other)
{
	HGLRC w1rc = this->GetSharedContext();
	HGLRC w2rc = other->GetSharedContext();

	if(!wglShareLists(w1rc,w2rc))
		wglShareLists(w2rc,w1rc);

	return (int)GetLastError();
}

QList<RenderWidget*> MDIWidget::GetRenderWidgets()
{
	QList<RenderWidget*> widgets;
	widgets.append(this->Widget2D);
	widgets.append(this->Widget3D);
	widgets.append(this->SlicerWidget);
	return widgets;
}

void MDIWidget::RegisterSelectionProducer(MDIWidget *otherView, bool bidirectional)
{
	//Get the list of widgets that can produce and consume 
	QList<RenderWidget*> myWidgets = this->GetRenderWidgets();
	QList<RenderWidget*> otherWidgets = otherView->GetRenderWidgets();

	//Connect all my widgets with all widgets of the other view 
	for(int i = 0; i < otherWidgets.size(); i++)
		for(int j = 0; j < myWidgets.size(); j++)
			myWidgets.at(j)->AddSelectionObserver(otherWidgets.at(i));

	//if bidirectional, do the same for the other view
	if(bidirectional)
		otherView->RegisterSelectionProducer(this, false);
}

void MDIWidget::RegisterViewChangeProducer(MDIWidget *otherView, ViewType types, bool bidirectional)
{
	//Get the list of widgets that can produce and consume 
	QList<RenderWidget*> myWidgets = this->GetRenderWidgets();
	QList<RenderWidget*> otherWidgets = otherView->GetRenderWidgets();

	//Connect all my widgets with all widgets of the other view 
	for(int i = 0; i < otherWidgets.size(); i++)
	{
		//connect only widgets of the desired type
		if(!(otherWidgets.at(i)->GetType() & types))
			continue;
		for(int j = 0; j < myWidgets.size(); j++)
		{
			//connect only widgets of the desired type
			if(!(myWidgets.at(j)->GetType() & types))
				continue;
			myWidgets.at(j)->AddInteractionObserver(otherWidgets.at(i));
		}
	}
	//if bidirectional, do the same for the other view
	if(bidirectional)
		otherView->RegisterViewChangeProducer(this, types, false);

}

void MDIWidget::SetContextVolume(vtkImageData* contextData)
{
	//forward to the slice widgets and the 3d render widget
	//the matrix widget does not use this volume

	this->Widget3D->SetRayCastVolume(contextData);
	this->SlicerWidget->SetRayCastVolume(contextData);
}

vtkImageData *MDIWidget::GetMaskVolume()
{
	if(!this->MaskVolume)
	{
		PointsToVolumeConverter = PointsToVolumeData::New();
		PointsToVolumeConverter->SetLayout(this->Layout);
		PointsToVolumeConverter->Update();
		this->MaskVolume = PointsToVolumeConverter->GetOutput();
	}
	return this->MaskVolume;
}