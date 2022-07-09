#ifndef CLCONTEXT_HEADER
#define CLCONTEXT_HEADER

#include "../DLLDefines.h"
//#include <GL/glew.h>
#include <Utils/clext.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <map>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
class CLMemoryObject;

class Rendering_EXPORT CLContext : public vtkObject
{
public:
	static CLContext& Instance();
	~CLContext();

	inline cl::Context *Context() const { return context; }
	
	int CreateNewQueue();
	
	inline cl::CommandQueue *GetQueue(unsigned int queueNum) 
	{
		if(queueNum >= queues.size())
			return NULL;
		else
			return queues[queueNum]; 
	}

	inline CLMemoryObject* GetSharedMemory(void *cpuPointer) 
	{
		return sharedMemory[cpuPointer];
	}

	inline void SetSharedMemory(void *cpuPointer, CLMemoryObject* mem)
	{
		sharedMemory[cpuPointer] = mem;
	}

	inline bool ContainsSharedMemory(void *cpuPointer)
	{
		return sharedMemory.find(cpuPointer) != sharedMemory.end();
	}

	bool IsDeviceAMD();
	bool IsDeviceIntel();
	bool IsDeviceNvidia();

	cl::STRING_CLASS GetPlatformName();
	cl::STRING_CLASS GetDeviceName();
	unsigned long GetDeviceMemory();
	uint GetDeviceComputeCores();
	uint GetDeviceClock();

protected:
	void InitCL();

private:
	CLContext();

	cl::Context *context;
	cl::vector<cl::Device> devices;
	cl::vector<cl::Platform> platforms;
	cl::vector<cl::CommandQueue*> queues;
	std::map<void*,CLMemoryObject*> sharedMemory;

	static CLContext *_instance;

};
#endif //CLCONTEXT_HEADER