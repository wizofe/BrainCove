#ifndef SLICEVIEWSWIDGET_H
#define SLICEVIEWSWIDGET_H

#include "renderwidget.h"
#include <Layouts/BrainLayout.h>
#include <vtkImagePlaneWidget.h>
#include <vtkActor.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkOutlineFilter.h>
#include <vtkImageShrink3D.h>
#include <vtkEventQtSlotConnect.h>
#include <Selectors/BrainVoxelSelector.h>
#include <Views/renderwidgetslices.h>

class SliceViewsWidget : public RenderWidget
{
	Q_OBJECT

private slots:
	void on_sliceWidget_CursorPositionChanged(RenderWidgetSlices *sliceViewer, double *pos);
	//void on_renderWidget_mouseMove();
	/*void on_actionMouse_Hover_clicked();
	void on_renderWidget_mouseMove();
	void on_renderWidget_viewChanged();
	void on_renderWidget_leftButtonUp();
	void on_renderWidget_leftButtonDown();
	void on_renderWidget_Toggle_hover();*/

public:
	UI_EXPORT SliceViewsWidget(QWidget *parent = 0);
	UI_EXPORT ~SliceViewsWidget();

	UI_EXPORT virtual void SetData(DataFile *data);
	UI_EXPORT void SetRayCastVolume(vtkImageData *rcVol);
	UI_EXPORT void SetMaskVolume(vtkImageData *rcVol);

	UI_EXPORT void SetLayout(BaseLayout *lay);

	UI_EXPORT virtual void SetSelection(DataSelection *s);

protected: 
	virtual void Update();

private:
	void SetupSubViews();

	BrainLayout *Layout;

	RenderWidgetSlices *SlicerX;
	RenderWidgetSlices *SlicerY;
	RenderWidgetSlices *SlicerZ;

	QGridLayout *GridLayout;
	QWidget *CentralWidget;

	vtkImageData *RaycastVolume;
	vtkImageData *MaskVolume;
};

#endif // SLICEVIEWSWIDGET_H
