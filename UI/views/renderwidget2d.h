#ifndef RENDERWIDGET2D_H
#define RENDERWIDGET2D_H

#include "renderwidget.h"
#include "styles/View2DInteractorStyle.h"
#include <Mappers/GPUPlaneRayCastMapper.h>
#include <Layouts/BrainLayout1D.h>
#include <vtkEventQtSlotConnect.h>
#include <Selectors/MatrixVoxelSelector.h>

class RenderWidget2D : public RenderWidget
{
	Q_OBJECT

private slots:
	void on_actionFPS_calc_clicked();
	void on_actionMouse_Hover_clicked();
	void on_renderWidget_mouseMove();
	void on_renderWidget_viewChanged();
	void on_renderWidget_leftButtonUp();
	void on_renderWidget_leftButtonDown();
	void on_renderWidget_Toggle_hover();

public:
	UI_EXPORT RenderWidget2D(QWidget *parent = 0);
	UI_EXPORT ~RenderWidget2D();

	UI_EXPORT virtual void SetLayout(BaseLayout *lay) {}

protected: 
	virtual void Update();

private:
	void SetupRenderer();
	void SetupMapper();
	void SetupLayout();
	void SetupActions();
	void SetupPicker();

	BrainLayout1D *Layout;

	vtkEventQtSlotConnect *slotConnector;

	QAction *actionMouse_Hover;
	QAction *actionFPS_calc;
};

#endif // RENDERWIDGET2D_H
