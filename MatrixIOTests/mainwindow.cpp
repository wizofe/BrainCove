#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ApplyStylesheet();
	AddActions();
	dataMatrix = NULL;
	activeFile = NULL;
	hglRC = NULL;
	this->VolumeReader = NULL;
	this->RaycastVolume = NULL;
}

MainWindow::~MainWindow()
{
}

void MainWindow::ApplyStylesheet()
{
	QFile file(":/ui/style_sheet.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());

	qApp->setStyleSheet(styleSheet);
	file.close();
}

void MainWindow::ShowRenderInfo()
{
	CLContext *ctxt = &CLContext::Instance();
	QTreeWidgetItem *pltfItem = new QTreeWidgetItem(QStringList() << QString("Platform:"));
	ui.infoTree->addTopLevelItem(pltfItem);
	QTreeWidgetItem *pltfItemChild = new QTreeWidgetItem(QStringList() << QString::fromStdString(ctxt->GetPlatformName()));
	pltfItem->addChild(pltfItemChild);

	QTreeWidgetItem *devItem = new QTreeWidgetItem(QStringList() << QString("Device:"));
	ui.infoTree->addTopLevelItem(devItem);
	QTreeWidgetItem *devItemChild1 = new QTreeWidgetItem(QStringList() << QString("Name: "));
	QTreeWidgetItem *devItemChild1Child = new QTreeWidgetItem(QStringList() << QString::fromStdString(ctxt->GetDeviceName()));
	devItemChild1->addChild(devItemChild1Child);

	QTreeWidgetItem *devItemChild2 = new QTreeWidgetItem(QStringList() << QString("Memory: "));
	QTreeWidgetItem *devItemChild2Child = new QTreeWidgetItem(QStringList() << QString("%1 MB").arg(ctxt->GetDeviceMemory()/(1024*1024)));
	devItemChild2->addChild(devItemChild2Child);

	int memAvailable = 0;
	if(ctxt->IsDeviceNvidia())
	{
		GLint total_mem_kb = 0;
		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(0x9048,&total_mem_kb); //GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 
		glGetIntegerv(0x9049,&cur_avail_mem_kb); //GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
		memAvailable = (total_mem_kb - cur_avail_mem_kb)/1024;
	}
	else if(ctxt->IsDeviceAMD())
	{
		//#define VBO_FREE_MEMORY_ATI          0x87FB
		//#define TEXTURE_FREE_MEMORY_ATI      0x87FC
		//#define RENDERBUFFER_FREE_MEMORY_ATI 0x87FD
		GLint param[4];
		glGetIntegerv (0x87FB, param);
		memAvailable = param[2] / 1024;
	}
	QTreeWidgetItem *devItemChild2Child2 = new QTreeWidgetItem(QStringList() << QString("%1 MB in use").arg(memAvailable));
	devItemChild2->addChild(devItemChild2Child2);

	QTreeWidgetItem *devItemChild3 = new QTreeWidgetItem(QStringList() << QString("Compute cores: "));
	QTreeWidgetItem *devItemChild3Child = new QTreeWidgetItem(QStringList() << QString("%1").arg(ctxt->GetDeviceComputeCores()));
	devItemChild3->addChild(devItemChild3Child);

	QTreeWidgetItem *devItemChild4 = new QTreeWidgetItem(QStringList() << QString("Clock speed: "));
	QTreeWidgetItem *devItemChild4Child = new QTreeWidgetItem(QStringList() << QString("%1 Mhz").arg(ctxt->GetDeviceClock()));
	devItemChild4->addChild(devItemChild4Child);

	devItem->addChild(devItemChild1);
	devItem->addChild(devItemChild2);
	devItem->addChild(devItemChild3);
	devItem->addChild(devItemChild4);

	ui.infoTree->expandAll();
}

void MainWindow::on_brainSeparationSlider_valueChanged()
{
	for(int i = 0;i < openWindows.size(); i++)
		openWindows[i]->SetHemisphereSeparation(ui.brainSeparationSlider->value());
}

void MainWindow::on_actionOpen_Matrix_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open rs-fMRI files "), "", tr("rs-fMRI files (*.rsfmri)"));
	
	if(!fileName.isNull() && !fileName.isEmpty())
		Open(fileName);

}

void MainWindow::on_actionEdit_kernels_triggered()
{
	for(int i = 0;i < openWindows.size(); i++)
		openWindows[i]->RebuildKernels();
}

void MainWindow::on_actionLink_Interaction_triggered()
{
	if(openWindows.size() < 2)
		return;

	for(int i = 0;i < openWindows.size()-1; i++)
	{
		for(int j = i+1;j < openWindows.size(); j++)
		{
			openWindows.at(i)->RegisterSelectionProducer(openWindows.at(j));
			openWindows.at(i)->RegisterViewChangeProducer(openWindows.at(j),ViewType::Matrix|ViewType::Anatomical3D|ViewType::PseudoAnatomical2D);
		}
	}
}

void MainWindow::on_actionUse_difference_mode_triggered()
{
	for(int i = 1; i < openWindows.size(); i++)
		if(ui.actionUse_difference_mode->isChecked())
			openWindows.at(i)->SetRenderModeToComparison();
		else
			openWindows.at(i)->SetRenderModeToNormal();

}

void MainWindow::on_actionReset_stylesheet_triggered()
{
	ApplyStylesheet();
}

void MainWindow::Open(const QString &fileName)
{
	DataFile *file = new DataFile();
	file->ReadFromFile(fileName);
	openFiles.push_back(file);
	SetActiveFile(file);
}

void MainWindow::SetActiveFile(DataFile *file)
{
	MDIWidget *mdiWindow = new MDIWidget(this,hglRC);
	ui.mdiArea->addSubWindow(mdiWindow,windowFlags() | Qt::Tool);//,Qt::CustomizeWindowHint);
	ui.mdiArea->tileSubWindows();
	mdiWindow->SetContextVolume(this->GetRaycastVolume());
	mdiWindow->SetData(file);
	mdiWindow->show();

	if(openWindows.size() > 0)
	{
		ui.mdiArea->tileSubWindows();
		mdiWindow->SetComparisonData(openWindows.at(openWindows.size()-1)->GetData());
	}
	else
		mdiWindow->showMaximized();

	openWindows.push_back(mdiWindow);
	ShowRenderInfo();

	if(!hglRC)
		hglRC = mdiWindow->GetSharedContext();

}

void MainWindow::AddActions()
{
	
}

vtkImageData *MainWindow::GetRaycastVolume()
{
	if(!this->RaycastVolume)
	{
		VolumeReader = vtkXMLImageDataReader::New();
		std::string dataDir = std::string(DATA_DIR);
		//std::string dataFile = std::string(dataDir).append("/MNI152_T1_0.5mm_brain_th100.vti");
		std::string dataFile = std::string(dataDir).append("/ch2.vti");
		VolumeReader->SetFileName(dataFile.c_str());
		VolumeReader->Update();
		this->RaycastVolume = VolumeReader->GetOutput();
	}
	return this->RaycastVolume;
}
