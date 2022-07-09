#ifndef RENDERWIDGET3D_H
#define RENDERWIDGET3D_H

#include "renderwidget.h"
#include "styles/View3DInteractorStyle.h"
#include <Mappers/GPUVolumeRayCastMapper.h>
#include <Layouts/BrainLayout.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkXMLImageDataReader.h>
#include <Selectors/BrainVoxelSelector.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkActorCollection.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkPointSource.h>
#include <vtkStringArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkActor2D.h>

class RenderWidget3D : public RenderWidget
{
	Q_OBJECT

public slots:
	void on_actionFPS_calc_clicked();
	void on_renderWidget_mouseMove();
	void on_renderWidget_middleButtonDown();
	void on_renderWidget_viewChanged();
	void on_actionChange_RayMethod_triggered();
	void on_actionSave_Correlation_triggered();

public:
	UI_EXPORT RenderWidget3D(QWidget *parent = 0);
	UI_EXPORT ~RenderWidget3D();
	UI_EXPORT virtual void SetData(DataFile *data);
	UI_EXPORT void SetLayout(BaseLayout *lay);
	UI_EXPORT void SetRayCastVolume(vtkImageData *data) { RaycastVolume = data; }
	UI_EXPORT void SetMaskVolume(vtkImageData *data)
	{ 
		MaskVolume = data; 
		MatrixMapper->SetMaskVolume(data); 
	}

protected:
	virtual void Update();

private:
	void SetupRenderer();
	void SetupMapper();
	void SetupLayout();
	void SetupActions();
	void SetupPicker();

	void UpdateAtlasLabels();

	vtkImageData *GetAtlasVolume();
	vtkImageData *AtlasVolume;

	vtkImageData *GetRaycastVolume();
	vtkImageData *RaycastVolume;

	vtkImageData *GetMaskVolume();
	vtkImageData *MaskVolume;

	vtkXMLImageDataReader *VolumeReader2;
	BrainLayout *Layout;
	BrainLayout1D *VoxelReorder;

	QAction *actionMouse_Hover;
	QAction *actionChange_Sampler;
	QAction *actionChange_RayMethod;
	QAction *actionSave_Correlation;

	vtkInteractorStyleImage *ImageInteractorStyle;

	vtkActorCollection *AtlasLabelActors;

	vtkFollower *LabelActor;
	vtkVectorText *LabelSource;
	vtkPolyDataMapper *LabelMapper;

	QVTKWidget *PickWidget;

	QAction *actionFPS_calc;

	bool ShiftDown;
	int SplitStart[2];

	float PlanePos[4];
	float PlaneNorm[4];
	double RotateAxis[3];

};

#endif // RENDERWIDGET3D_H