#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QtGUI/QWidget>
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyle.h>
#include <Mappers/GPUGenericRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkRenderWindow.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <QVBoxLayout>
#include <QAction>
#include "../DLLDefines.h"
#include <DataFile.h>
#include <Systems/ARenderSystem.h>
#include <Systems/VTKRenderSystem.h>
#include <Selectors/BaseVoxelSelector.h>
#include <algorithm>
#include <vtkCallbackCommand.h>
#include <QElapsedTimer>

enum ViewType
{
	Matrix = 1,
	Anatomical3D = 2,
	PseudoAnatomical2D = 4
};

inline ViewType operator|(ViewType a, ViewType b)
{return static_cast<ViewType>(static_cast<int>(a) | static_cast<int>(b));}


class RenderWidget : public QWidget
{
	Q_OBJECT

public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	UI_EXPORT inline QList<QAction*> GetActions() { return this->Actions; }

	UI_EXPORT void SetData(DataFile *data);

	UI_EXPORT DataFile *GetData();

	UI_EXPORT void SetRenderModeToComparison() { this->GetMapper()->SetRenderModeToComparison(); }
	UI_EXPORT void SetRenderModeToNormal() { this->GetMapper()->SetRenderModeToNormal(); }

	UI_EXPORT void SetComparisonData(DataFile *data);
	UI_EXPORT void SetContext(HDC hDc, HGLRC hRC);

	UI_EXPORT vtkRenderWindow *GetRenderWindow() { return this->QRenderWidget->GetRenderWindow(); }

	UI_EXPORT GPUGenericRayCastMapper *GetMapper() const { return this->MatrixMapper; }

	UI_EXPORT BaseVoxelSelector *GetSelector() const { return this->VoxelPicker; }

	UI_EXPORT QString name() const { return Name; }

	UI_EXPORT void AddSelectionObserver(RenderWidget *w) { this->SelectionObservers.push_back(w); }
	UI_EXPORT void RemoveSelectionObserver(RenderWidget *w) { this->SelectionObservers.erase(remove(this->SelectionObservers.begin(),this->SelectionObservers.end(),w),this->SelectionObservers.end()); }

	UI_EXPORT void AddInteractionObserver(RenderWidget *w) { this->InteractionObservers.push_back(w); }
	UI_EXPORT void RemoveInteractionObserver(RenderWidget *w) { this->InteractionObservers.erase(remove(this->InteractionObservers.begin(),this->InteractionObservers.end(),w),this->InteractionObservers.end()); }

	UI_EXPORT virtual void SetSelection(DataSelection *s);
	UI_EXPORT void SetView(vtkRenderer *renderer);

	UI_EXPORT virtual void SetLayout(BaseLayout *lay) = 0;

	UI_EXPORT ViewType GetType() { return this->viewType; }

	UI_EXPORT ARenderSystem *GetRenderSystem() { return RenderSystem; }

protected:
	virtual void Update() = 0;
	void SetupVolume();
	void RestoreCamera();
	void SaveCamera();

	void ShareSelection(DataSelection *selection);
	void ShareView(vtkRenderer *renderer);

	QVTKWidget *QRenderWidget;
	QList<QAction*> Actions;
	vtkInteractorStyle *InteractorStyle;
	GPUGenericRayCastMapper *MatrixMapper;
	vtkVolume *DummyVolume;

	QString Name;

	DataFile *InputFile;
	BaseMatrix<float> *DataMatrix;

	vtkCamera *DefaultCamera;

	bool CameraReset;

	ARenderSystem *RenderSystem;

	BaseVoxelSelector *VoxelPicker;

	std::vector<RenderWidget*> SelectionObservers;
	std::vector<RenderWidget*> InteractionObservers;

	vtkEventQtSlotConnect *SlotConnector;
	vtkCallbackCommand *callback;
	vtkCallbackCommand *callbackStart;

	ViewType viewType;

};

#endif // RENDERWIDGET_H
