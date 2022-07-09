#ifndef ARENDERABLE_HEADER
#define ARENDERABLE_HEADER

#include "../DLLDefines.h"
//#include "glew.h"

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT ARenderable
{

public:
	ARenderable();

	virtual void Draw() const = 0;

protected:
};

#endif //ARENDERABLE_HEADER