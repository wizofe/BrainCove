#include "renderwidget3dh.h"

RenderWidget3DH::RenderWidget3DH(QWidget *parent) 
: RenderWidget(parent) 
{
	SetupLayout();
	SetupRenderer();
	SetupMapper();
	SetupActions();
	_name = QString("3D Visualization 2");
}

RenderWidget3DH::~RenderWidget3DH()
{

}

void RenderWidget3DH::SetData(DataFile *data)
{
	this->Layout->SetPositions(data->points());
	RenderWidget::SetData(data);
}

void RenderWidget3DH::SetupLayout()
{
	this->Layout = new BrainLayout();
}

void RenderWidget3DH::SetupRenderer()
{
	//setup our render system
	renderSystem = new VTKRenderSystem();
	renderSystem->SetupRenderer(this->qRenderWidget);
	renderSystem->GetRenderer()->ResetCamera();

	//dark gray
	renderSystem->GetRenderer()->SetGradientBackground(true);
	renderSystem->GetRenderer()->SetBackground(0.2333,0.2333,0.2333);
	renderSystem->GetRenderer()->SetBackground2(0.6,0.6,0.6);
	renderSystem->GetRenderer()->GetRenderWindow()->Render();

	//add the dummy volume
	renderSystem->GetRenderer()->AddVolume(dummyVolume);

	interactorStyle = View3DInteractorStyle::New();
	connect((View3DInteractorStyle*)interactorStyle,SIGNAL(mouseMoved(int,int)),this,SLOT(onrenderWidget_mouseEvent(int,int)));
	connect((View3DInteractorStyle*)interactorStyle,SIGNAL(mouseWheelRotated(double)),this,SLOT(onrenderWidget_mouseWheelEvent(double)));
	connect((View3DInteractorStyle*)interactorStyle,SIGNAL(rightButtonDown()),this,SLOT(onrenderWidget_rightButtonDown()));

	renderSystem->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);
}

void RenderWidget3DH::SetupMapper()
{
	reader = vtkXMLImageDataReader::New();
	std::string dataDir = std::string(DATA_DIR);
	std::string dataFile = std::string(dataDir).append("/MNI152_T1_0.5mm_brain_th100.vti");
	reader->SetFileName(dataFile.c_str());
	reader->Update();

	//vtkFixedPointVolumeRayCastMapper *rm = vtkFixedPointVolumeRayCastMapper::New();
	matrixMapper = GPUVolumeRayCastMapper::New();
	//dummyVolume->SetMapper(rm);
	dummyVolume->SetMapper(matrixMapper);
	//vtkPiecewiseFunction *otf = vtkPiecewiseFunction::New();
	//vtkColorTransferFunction *ctf = vtkColorTransferFunction::New();
	//dataFile = std::string(dataDir).append("/brain.dvtf");
	//ColorMapping::ImportFromDevideTF(dataFile.c_str(),ctf,otf);
	//dummyVolume->GetProperty()->SetColor(ctf);
	//dummyVolume->GetProperty()->SetScalarOpacity(otf);
	//rm->SetInput(reader->GetOutput());
	matrixMapper->SetInput(reader->GetOutput());
}

void RenderWidget3DH::SetupActions()
{
	/*actionMouse_Hover = new QAction(QIcon(":/ui/icons/fatcow/32x32/zoom_layer.png"), tr("Enable/disable &hover"), this);
	actionMouse_Hover->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
	actionMouse_Hover->setStatusTip(tr("Enable or disable mouse hover"));
	actionMouse_Hover->setCheckable(true);
	actionMouse_Hover->setChecked(true);
	this->actions.append(actionMouse_Hover);
	
	actionChange_Sampler = new QAction(QIcon(":/ui/icons/fatcow/32x32/layer_grid.png"), tr("Switch sampler"), this);
	actionChange_Sampler->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
	actionChange_Sampler->setStatusTip(tr("Switch the sampler (inter-voxel interpolation)"));
	connect(actionChange_Sampler,SIGNAL(triggered()),this,SLOT(on_actionChange_Sampler_triggered()));

	this->actions.append(actionChange_Sampler);
	*/
}

void RenderWidget3DH::Update()
{
	matrixMapper->SetConnectivityData(this->inputFile);
	matrixMapper->SetLayoutSystem(this->Layout);
	dummyVolume->SetVisibility(this->inputFile != NULL);
	if(!cameraReset)
	{
		renderSystem->GetRenderer()->ResetCamera();

		this->SaveCamera();
		cameraReset = true;
	}
}

void RenderWidget3DH::onrenderWidget_mouseEvent(int x, int y)
{
	((GPUVolumeRayCastMapper*)matrixMapper)->SetMouseSelection(x,y);
	renderSystem->GetRenderer()->GetRenderWindow()->Render();
	
	/*int coord[3];
	((GPUVolumeRayCastMapper*)gMapper)->GetVoxelCoordinates(coord);
	SelectionMap picked = ((GPUVolumeRayCastMapper*)gMapper)->GetVoxelsPicked();
	if(picked.size() > 0)
	{
		SelectionEntry entry = *picked.begin();
		voxelLabel.setText(QString("Voxel %1 selected").arg(entry.first));
		dontUpdate = true;
		ui.spinBoxX->setValue(coord[0]);
		ui.spinBoxY->setValue(coord[1]);
		ui.spinBoxZ->setValue(coord[2]);
		dontUpdate = false;
	}*/
}

void RenderWidget3DH::onrenderWidget_mouseWheelEvent(double factor)
{
	renderSystem->GetRenderer()->GetRenderWindow()->Render();
}

void RenderWidget3DH::onrenderWidget_rightButtonDown()
{
	//actionMouse_Hover->setChecked(!actionMouse_Hover->isChecked());
}

void RenderWidget3DH::on_actionChange_Sampler_triggered()
{
	renderSystem->GetRenderer()->GetRenderWindow()->Render();
}