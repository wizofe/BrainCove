#ifndef CLMemoryObjectCL_HEADER
#define CLMemoryObjectCL_HEADER

#include "../DLLDefines.h"
#include "CLMemoryObject.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLMemoryObjectCL : public CLMemoryObject
{
public:

	CLMemoryObjectCL(const cl::Context& ct)  : CLMemoryObject(ct) {}
	~CLMemoryObjectCL();
	
protected:
	void CreateBuffer
private:
};
#endif //CLMemoryObjectCL_HEADER