#include "sliceviewswidget.h"

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

SliceViewsWidget::SliceViewsWidget(QWidget *parent) 
//: RenderWidget(parent)
{
	SetupSubViews();
	this->Name = QString("Slice Viewers");
}

SliceViewsWidget::~SliceViewsWidget()
{

}

void SliceViewsWidget::SetupSubViews()
{
	this->layout()->removeWidget(this->QRenderWidget);
	qDeleteAll(this->children());
	this->setLayout(new QVBoxLayout(this));

	this->CentralWidget = new QWidget(this);
	this->GridLayout = new QGridLayout(this->CentralWidget);
    this->GridLayout->setSpacing(6);
    this->GridLayout->setContentsMargins(0, 0, 0, 0);
	this->CentralWidget->setLayout(this->GridLayout);

	this->layout()->addWidget(this->CentralWidget);

	this->SlicerX = new RenderWidgetSlices(this,0);
	this->SlicerY = new RenderWidgetSlices(this,1);
	this->SlicerZ = new RenderWidgetSlices(this,2);

	this->GridLayout->addWidget(SlicerX,0,0);
	this->GridLayout->addWidget(SlicerY,0,1);
	this->GridLayout->addWidget(SlicerZ,1,0);

	connect(this->SlicerX,SIGNAL(cursorPositionChanged(RenderWidgetSlices *, double*)),this,SLOT(on_sliceWidget_CursorPositionChanged(RenderWidgetSlices *, double*)));
	connect(this->SlicerY,SIGNAL(cursorPositionChanged(RenderWidgetSlices *, double*)),this,SLOT(on_sliceWidget_CursorPositionChanged(RenderWidgetSlices *, double*)));
	connect(this->SlicerZ,SIGNAL(cursorPositionChanged(RenderWidgetSlices *, double*)),this,SLOT(on_sliceWidget_CursorPositionChanged(RenderWidgetSlices *, double*)));

}

void SliceViewsWidget::SetData(DataFile *data)
{
	SlicerX->SetData(data);
	SlicerY->SetData(data);
	SlicerZ->SetData(data);
}

void SliceViewsWidget::Update()
{
}

void SliceViewsWidget::SetLayout(BaseLayout *lay)
{
	SlicerX->SetLayout(lay);
	SlicerY->SetLayout(lay);
	SlicerZ->SetLayout(lay);
}

void SliceViewsWidget::SetRayCastVolume(vtkImageData *rcVol)
{
	SlicerX->SetRayCastVolume(rcVol);
	SlicerY->SetRayCastVolume(rcVol);
	SlicerZ->SetRayCastVolume(rcVol);
}

void SliceViewsWidget::SetMaskVolume(vtkImageData *rcVol)
{
	SlicerX->SetMaskVolume(rcVol);
	SlicerY->SetMaskVolume(rcVol);
	SlicerZ->SetMaskVolume(rcVol);
}

void SliceViewsWidget::on_sliceWidget_CursorPositionChanged(RenderWidgetSlices *sliceViewer, double *pos)
{
	if(SlicerX != sliceViewer)
		SlicerX->SetPosition(pos);
	if(SlicerY != sliceViewer)
		SlicerY->SetPosition(pos);
	if(SlicerZ != sliceViewer)
		SlicerZ->SetPosition(pos);

	int seed = SlicerX->GetPicker3D()->GetIdFromWorldPosition(pos);

	DataSelection *selection = SlicerX->GetPicker3D()->SelectWithSeed(seed);
	
	ShareSelection(selection);
}

void SliceViewsWidget::SetSelection(DataSelection *s)
{
	if(s->contentType() != VOXELTYPE)
		return;

	const double *pos = s->position();
	SlicerX->SetPosition(pos);
	SlicerY->SetPosition(pos);
	SlicerZ->SetPosition(pos);

}
