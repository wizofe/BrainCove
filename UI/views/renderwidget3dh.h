#ifndef RENDERWIDGET3DH_H
#define RENDERWIDGET3DH_H

#include "renderwidget.h"
#include "styles/View3DInteractorStyle.h"
#include <Mappers/GPUVolumeRayCastMapper.h>
//#include <Mappers/GPUMappedVolumeRayCastMapper.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageShiftScale.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkImageCast.h>

class RenderWidget3DH : public RenderWidget
{
	Q_OBJECT

private slots:
	void onrenderWidget_mouseEvent(int,int);
	void onrenderWidget_mouseWheelEvent(double);
	void onrenderWidget_rightButtonDown();
	void on_actionChange_Sampler_triggered();

public:
	UI_EXPORT RenderWidget3DH(QWidget *parent = 0);
	UI_EXPORT ~RenderWidget3DH();
	UI_EXPORT virtual void SetData(DataFile *data);

protected:
	virtual void Update();

private:
	void SetupRenderer();
	void SetupMapper();
	void SetupActions();
	void SetupLayout();

	QAction *actionMouse_Hover;
	QAction *actionChange_Sampler;

	BrainLayout *Layout;

	vtkXMLImageDataReader *reader;
	vtkImageData *data;
	vtkImageShiftScale *shiftScale;
};

#endif // RENDERWIDGET3DH_H
