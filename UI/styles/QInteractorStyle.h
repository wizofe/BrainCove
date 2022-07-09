#ifndef QINTERACTORSTYLE_H
#define QINTERACTORSTYLE_H

#include <QObject>
#include <vtkRenderer.h>

class QInteractorStyle : public QObject
{
	Q_OBJECT

public slots:
	void on_viewPointChanged(vtkRenderer *otherRenderer) { CopyViewSettings(otherRenderer); }

signals:
	void viewPointChanged(vtkRenderer *thisRenderer);
	void mouseMoved(int x, int y);
	void mouseWheelRotated(double d);
	void rightButtonDown();
	void rightButtonUp();
	void leftButtonDown();
	void leftButtonUp();
	void middleButtonDown();
	void middleButtonUp();

protected:
	QInteractorStyle() {}
	~QInteractorStyle() {}

	virtual void CopyViewSettings(vtkRenderer *otherRenderer) = 0;

};

#endif // QINTERACTORSTYLE_H