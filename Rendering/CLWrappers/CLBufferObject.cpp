#include "CLBufferObject.h"

CLBufferObject* CLBufferObject::New(const cl::Context& ct,size_t size,void *data)
{
	if(CLMemoryObject::IsOpenGLInteropSupported(ct) && CLContext::Instance().IsDeviceAMD())
		return new CLBufferObject(ct,true,size,data);
	else
		return new CLBufferObject(ct,false,size,data);
}


CLBufferObject::CLBufferObject(const cl::Context& ct,bool useGL, size_t size, void *data) : CLMemoryObject(ct)
{
	int err = 0;
	memory_size = size;
	_glBuffer = 0;
	//useGL = false;
	_useGL = useGL;

	if(useGL)
		CreateGLBufferStore(size,data,&err);
	else
		CreateBufferStore(size,data,&err);
}

CLBufferObject::~CLBufferObject()
{
}

void CLBufferObject::SynchronizeFromCL(const cl::CommandQueue& queue)
{
	//do nothing
}
