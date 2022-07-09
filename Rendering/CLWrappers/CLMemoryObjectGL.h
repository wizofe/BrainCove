#ifndef CLMemoryGLObject_HEADER
#define CLMemoryGLObject_HEADER

#include "../DLLDefines.h"
#include "CLMemoryObject.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLMemoryGLObject : public CLMemoryObject
{
public:

	CLMemoryGLObject(const cl::Context& ct)  : CLMemoryObject(ct) {}
	~CLMemoryGLObject();
	
	GLuint GetGLName() { return glId; }

private:
	GLuint glId; 
};
#endif //CLMemoryGLObject_HEADER