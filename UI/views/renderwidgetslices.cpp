#include "renderwidgetslices.h"

RenderWidgetSlices::RenderWidgetSlices(QWidget *parent, int orientation) 
: RenderWidget(parent)
{
	SetupRenderer();
	SetupMapper();
	SetupPicker();
	SetAxisAlignment(orientation);
	this->viewType = ViewType::Matrix;
	this->Name = QString("Slice Viewer");
}

RenderWidgetSlices::~RenderWidgetSlices()
{

}

void RenderWidgetSlices::SetupRenderer()
{
	//setup our render system
	this->RenderSystem = new VTKRenderSystem();
	this->RenderSystem->SetupRenderer(this->QRenderWidget);
	this->RenderSystem->GetRenderer()->ResetCamera();

	//dark gray
	this->RenderSystem->GetRenderer()->SetBackground(0.2333,0.2333,0.2333);
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();

	this->RenderSystem->GetRenderer()->GetActiveCamera()->ParallelProjectionOn();

	this->ImageInteractor = vtkInteractorStyleImage::New();

	this->WidgetX = vtkImagePlaneWidget::New();
	this->WidgetX->DisplayTextOn();
	this->WidgetX->SetInteractor(this->RenderSystem->GetRenderer()->GetRenderWindow()->GetInteractor());
	this->WidgetX->GetInteractor()->SetInteractorStyle(this->ImageInteractor);
 	this->WidgetX->SetResliceInterpolateToNearestNeighbour();
	this->WidgetX->SetRestrictPlaneToVolume(true);

	this->SlotConnector = vtkEventQtSlotConnect::New();
	this->SlotConnector->Connect(this->WidgetX,vtkCommand::InteractionEvent,this,SLOT(on_renderWidget_mouseMove()));

}

void RenderWidgetSlices::SetupMapper()
{
	this->OutlineFilter = vtkOutlineFilter::New();
	this->OutlineMapper = vtkPolyDataMapper::New();
	this->OutlineActor = vtkActor::New();

	this->OutlineMapper->SetInputConnection(this->OutlineFilter->GetOutputPort());

	this->OutlineActor->SetMapper(this->OutlineMapper);

	this->CursorSource = vtkSphereSource::New();
	this->CursorSource->SetRadius(5);

	this->CursorMapper = vtkPolyDataMapper::New();
	this->CursorActor = vtkActor::New();
	this->CursorActor->GetProperty()->SetColor(1,0,0);
	
	this->CursorMapper->SetInput(this->CursorSource->GetOutput());
	this->CursorActor->SetMapper(this->CursorMapper);

	this->RenderSystem->GetRenderer()->AddActor(this->CursorActor);

}

void RenderWidgetSlices::SetupLayout()
{
	this->Layout = new BrainLayout();
}


void RenderWidgetSlices::SetupPicker()
{
	this->VoxelPicker = new BrainVoxelSelector();
}

void RenderWidgetSlices::AlignCamera()
{
	//Align the camera so that it faces the desired widget
	double spacing[3];
	double origin[3];
	int extent[6];

	this->GetRaycastVolume()->GetSpacing(spacing);
	this->GetRaycastVolume()->GetOrigin(origin);
	this->GetRaycastVolume()->GetExtent(extent);

    //#global ox, oy, oz, sx, sy, sz, xMax, xMin, yMax, yMin, zMax, \
    //#      zMin, slice_number
    //#global current_widget
    double cx = origin[0]+(0.5*(extent[1]-extent[0]))*spacing[0];
	double cy = origin[1]+(0.5*(extent[3]-extent[2]))*spacing[1];
	double cz = origin[2]+(0.5*(extent[5]-extent[4]))*spacing[2];

	int iaxis = this->WidgetX->GetPlaneOrientation();
	int slice = this->WidgetX->GetSliceIndex();
    int vx, vy, vz;
    double nx, ny, nz, px, py, pz;
	vx = vy = vz = 0;
	nx = ny = nz = 0;

    if(iaxis == 0)
	{
        vz = 1;
        nx = origin[0] + extent[1] * spacing[0];
        cx = origin[0] + slice * spacing[0];
	}
	else if(iaxis == 1)
	{
        vz = 1;
        ny = origin[1] + extent[3] * spacing[1];
        cy = origin[1] + slice * spacing[1];
	}
    else
	{
        vy = 1;
        nz = origin[2] + extent[5] * spacing[2];
        cz = origin[2] + slice * spacing[2];
	}
    px = cx+nx*2;
    py = cy+ny*2;
    pz = cz+nz*3;

    vtkCamera *camera = this->RenderSystem->GetRenderer()->GetActiveCamera();
    camera->SetViewUp(vx, vy, vz);
    camera->SetFocalPoint(cx, cy, cz);
    camera->SetPosition(px, py, pz);
    camera->OrthogonalizeViewUp();
	this->RenderSystem->GetRenderer()->ResetCamera();
    this->RenderSystem->GetRenderer()->ResetCameraClippingRange();
    this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}

void RenderWidgetSlices::SetData(DataFile *data)
{
	RenderWidget::SetData(data);
}

void RenderWidgetSlices::Update()
{
	this->VoxelPicker->SetPickData(this->GetMaskVolume(),this->MatrixMapper->GetVolumeMatrix(this->GetMaskVolume(),this->DummyVolume));
	this->VoxelPicker->SetVoxelData(this->RaycastVolume);
	//this->VoxelPicker->SetAtlasData(this->MatrixMapper->GetAtlasVolume(),this->MatrixMapper->GetVolumeMatrix(MatrixMapper->GetAtlasVolume(),this->DummyVolume));
	this->VoxelPicker->SetLayout(this->Layout);
	this->VoxelPicker->StartBrushing();

	//this->OutlineFilter->SetInput(this->RaycastVolume);
	//this->OutlineFilter->Update();
	if(!this->WidgetX->GetInput())
	{
		this->WidgetX->SetInput(this->RaycastVolume);
		this->WidgetX->SetPlaneOrientation(this->AxisOrientation);
		this->WidgetX->SetSlicePosition(0);
		this->WidgetX->On();
		AlignCamera();
	}
}

void RenderWidgetSlices::SetAxisAlignment(int axis)
{
	this->AxisOrientation = axis;
}

void RenderWidgetSlices::SetLayout(BaseLayout *lay)
{
	BrainLayout *brlay = static_cast<BrainLayout*>(lay);
	
	if(brlay)
		this->Layout = brlay;
}

void RenderWidgetSlices::SetRayCastVolume(vtkImageData *rcVol)
{
	this->RaycastVolume = rcVol;
}

void RenderWidgetSlices::SetMaskVolume(vtkImageData *rcVol)
{
	this->MaskVolume = rcVol;
}

vtkImageData *RenderWidgetSlices::GetRaycastVolume()
{
	return this->RaycastVolume;
}

vtkImageData *RenderWidgetSlices::GetMaskVolume()
{
	return this->MaskVolume;
}

void RenderWidgetSlices::on_renderWidget_mouseMove()
{
	double *pos = this->WidgetX->GetCurrentCursorPosition();
	pos[3] = 1.0;
	this->CursorSource->SetCenter(pos[0],pos[1],pos[2]);
	this->CursorSource->Update();
	emit cursorPositionChanged(this,pos);

}

void RenderWidgetSlices::SetPosition(const double *pos)
{
	double bounds[6];
	this->GetRaycastVolume()->GetBounds(bounds);
	double mypos[3];
	for(int i = 0; i < 3; i++)
		mypos[i] = pos[i];

	if(!(pos[0] >= bounds[0] && pos[0] <= bounds[1] &&
		pos[1] >= bounds[2] && pos[1] <= bounds[3] &&
		pos[2] >= bounds[4] && pos[2] <= bounds[5]))
		mypos[0] = mypos[1] = mypos[2] = 1;
	
	this->CursorSource->SetCenter(mypos[0],mypos[1],mypos[2]);
	this->CursorSource->Update();
	//if(this->WidgetX->GetSlicePosition() != mypos[this->WidgetX->GetPlaneOrientation()])
	int por = this->WidgetX->GetPlaneOrientation();
	//if(this->WidgetX->GetSlicePosition() != mypos[por])
	{
		this->WidgetX->SetSlicePosition(mypos[por]);
		AlignCamera();
	}
	this->RenderSystem->GetRenderer()->GetRenderWindow()->Render();
}
