#ifndef RENDERWIDGETSLICES_H
#define RENDERWIDGETSLICES_H

#include "renderwidget.h"
#include <Layouts/BrainLayout.h>
#include <vtkImagePlaneWidget.h>
#include <vtkActor.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyleImage.h>
#include <Selectors/BrainVoxelSelector.h>

class RenderWidgetSlices : public RenderWidget
{
	Q_OBJECT

signals:
	void cursorPositionChanged(RenderWidgetSlices *sliceViewer, double *pos);

private slots:
	void on_renderWidget_mouseMove();
	/*void on_actionMouse_Hover_clicked();
	void on_renderWidget_mouseMove();
	void on_renderWidget_viewChanged();
	void on_renderWidget_leftButtonUp();
	void on_renderWidget_leftButtonDown();
	void on_renderWidget_Toggle_hover();*/

public:
	UI_EXPORT RenderWidgetSlices(QWidget *parent = 0, int orientation = 0);
	UI_EXPORT ~RenderWidgetSlices();

	UI_EXPORT virtual void SetData(DataFile *data);
	UI_EXPORT void SetRayCastVolume(vtkImageData *rcVol);
	UI_EXPORT void SetMaskVolume(vtkImageData *rcVol);
	UI_EXPORT void SetAxisAlignment(int axis);
	UI_EXPORT void SetPosition(const double pos[4]);

	UI_EXPORT BrainVoxelSelector* GetPicker3D() 
	{ 
		BrainVoxelSelector *bv = static_cast<BrainVoxelSelector*>(VoxelPicker);
		return bv; 
	}

	UI_EXPORT void SetLayout(BaseLayout *lay);

protected: 
	virtual void Update();
	vtkImageData *GetRaycastVolume();
	vtkImageData *GetMaskVolume();
	virtual void AlignCamera();

private:
	void SetupRenderer();
	void SetupMapper();
	void SetupLayout();
	void SetupPicker();

	vtkEventQtSlotConnect *SlotConnector;
	
	BrainLayout *Layout;

	vtkImagePlaneWidget *WidgetX;
	vtkImageData *RaycastVolume;
	vtkImageData *MaskVolume;

	vtkPolyDataMapper *CursorMapper;
	vtkActor *CursorActor;
	vtkSphereSource *CursorSource;

	vtkOutlineFilter *OutlineFilter;
	vtkPolyDataMapper *OutlineMapper;
	vtkActor *OutlineActor;

	vtkInteractorStyleImage *ImageInteractor;

	int AxisOrientation;
};

#endif // RENDERWIDGETSLICES_H
