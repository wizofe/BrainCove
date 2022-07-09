#pragma warning( disable : 4290 )

#include "mainwindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.showMaximized();
	return a.exec();
}
