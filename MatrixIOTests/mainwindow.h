#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include <QtGui/QPlastiqueStyle>
#include <CLWrappers/CLMemoryObject.h>
#include <editors/kerneleditorform.h>
#include "DataFile.h"
#include <Views/mdiwidget.h>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

private slots:
	void on_actionOpen_Matrix_triggered();
	void on_actionEdit_kernels_triggered();
	void on_actionLink_Interaction_triggered();	
	void on_actionUse_difference_mode_triggered();
	void on_actionReset_stylesheet_triggered();
	void on_brainSeparationSlider_valueChanged();

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

private:
	void ApplyStylesheet();
	void Open(const QString& filePath);
	void SetActiveFile(DataFile *file);
	void AddActions();
	void InitGL();
	void InitCL();
	void ShowRenderInfo();

	vtkImageData *GetRaycastVolume();
	vtkXMLImageDataReader *VolumeReader;

	vtkImageData *GetMaskVolume();

	BaseMatrix<float> *dataMatrix;

	std::vector<DataFile*> openFiles;
	std::vector<MDIWidget*> openWindows;
	DataFile* activeFile;
	Ui::MainWindow ui;

	vtkImageData *RaycastVolume;

	HGLRC hglRC;

	//KernelEditorForm kernelEditor;
};

#endif // TESTWINDOW_H
