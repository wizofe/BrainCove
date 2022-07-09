#ifndef CLBufferObject_HEADER
#define CLBufferObject_HEADER

#include "../DLLDefines.h"
#include "CLMemoryObject.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLBufferObject : public CLMemoryObject
{
public:
	static CLBufferObject *New(const cl::Context& ct,size_t size,void *data);
	~CLBufferObject();

	virtual void SynchronizeFromCL(const cl::CommandQueue& queue);
	virtual cl::Memory CLObject() const { return buffer; }
	virtual cl::Buffer CLBuffer() const { return buffer; }

protected:
	CLBufferObject(const cl::Context& ct,bool useGL, size_t size, void *data);

private:
	size_t memory_size;
};

#endif //CLBufferObject_HEADER