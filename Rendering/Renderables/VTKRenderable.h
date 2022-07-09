#ifndef VTKRENDERABLE_HEADER
#define VTKRENDERABLE_HEADER

#include "../DLLDefines.h"
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT VTKRenderable
{

public:
	VTKRenderable();

	virtual void Draw() const;

protected:
};

#endif //VTKRENDERABLE_HEADER