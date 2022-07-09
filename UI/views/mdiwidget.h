#ifndef MDIWIDGET_H
#define MDIWIDGET_H

#include <QtGUI/QMainWindow>
#include <QtGUI/QWidget>
#include <QtGUI/QSlider>
#include <QtGUI/QSplitter>
#include <QtGUI/QVBoxLayout>
#include <QtGUI/QMenu>
#include <QtGUI/QToolBar>
#include <Views/renderwidget.h>
#include <Views/renderwidget2d.h>
#include <Views/renderwidget3d.h>
#include <Views/sliceviewswidget.h>
#include "../DLLDefines.h"
#include <DataFile.h> 
#include <Filters/DataFilter.h>
#include <Utils/PointsToVolumeData.h>

class MDIWidget : public QMainWindow
{
	Q_OBJECT

private slots:
	void on_actionChange_ViewLink_triggered();
	void on_actionThreshold_changed();

public:
	UI_EXPORT MDIWidget(QWidget *parent = 0, HGLRC hgLRC = 0);
	UI_EXPORT ~MDIWidget();

	UI_EXPORT DataFile *GetData();

	UI_EXPORT void SetData(DataFile *data);

	UI_EXPORT void SetComparisonData(DataFile *data);

	UI_EXPORT void SetRenderModeToComparison();

	UI_EXPORT void SetRenderModeToNormal();

	UI_EXPORT void SetHemisphereSeparation(int angle);

	UI_EXPORT void RebuildKernels();

	UI_EXPORT int LinkContexts(const MDIWidget *other);

	UI_EXPORT HGLRC GetSharedContext() const { return this->SharedContext; }

	UI_EXPORT void RegisterSelectionProducer(MDIWidget *otherView, bool bidirectional = true);

	UI_EXPORT void RegisterViewChangeProducer(MDIWidget *otherView, ViewType types, bool bidirectional = true);

	UI_EXPORT void SetContextVolume(vtkImageData* contextData);

protected:
	QList<RenderWidget*> GetRenderWidgets();

	void SetupActions();
	void LinkWidgetContexts();

	vtkImageData *GetMaskVolume();

	QWidget *CentralWidget;

	RenderWidget2D *Widget2D;
	RenderWidget3D *Widget3D;
	SliceViewsWidget *SlicerWidget;

	QToolBar *WindowToolBar;
	QVBoxLayout *VerticalLayout;
	QSplitter *Splitter;
	QSplitter *TopSplitter;

	QAction *LinkViewsAction;

	QSlider *CorrelationThresholdSlider;

	DataFilter Filter;

	HGLRC SharedContext;

	vtkImageData *MaskVolume;
	PointsToVolumeData *PointsToVolumeConverter;

	BaseLayout *Layout;

};

#endif // MDIWIDGET_H
