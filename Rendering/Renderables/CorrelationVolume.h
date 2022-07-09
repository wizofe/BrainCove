#ifndef CORRELATIONVOLUME_HEADER
#define CORRELATIONVOLUME_HEADER

#include "DLLDefines.h"
#ifdef USE_NQVTK
#include "NQVTKRenderable.h"
#define BaseRenderable NQVTKRenderable
#else
#include "VTKRenderable.h"
#define BaseRenderable VTKRenderable
#endif

#include <QTextStream>
#include <QFile>
#include <vtkCellArray.h>
#include "BaseMatrix.hpp"

//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
class Rendering_EXPORT CorrelationVolume : public BaseRenderable
{
	typedef BaseRenderable Superclass;

public:
	CorrelationVolume();
	void SetCorrelations(const BaseMatrix<float>& matrix);

	void Draw() const;

private:
	BaseMatrix<float>* matrix;
	vtkCellArray *vtkMatrix;
};

#endif //CORRELATIONVOLUME_HEADER