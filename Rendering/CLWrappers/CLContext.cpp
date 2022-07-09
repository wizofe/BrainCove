#include "CLContext.h"

CLContext *CLContext::_instance = NULL;

CLContext::CLContext()
{
	context = NULL;
	devices = NULL;
	platforms = NULL;
}

CLContext::~CLContext()
{
	if(context)
		delete context;

	if(queues)
		delete queues;

	if(devices)
		delete devices;

	if(platforms)
		delete platforms;
}


CLContext &CLContext::Instance()
{
	if(_instance == NULL)
	{
		_instance = new CLContext();
		_instance->InitCL();
	}
	return *_instance;
}

void CLContext::InitCL()
{
	platforms = cl::vector<cl::Platform>();
    // Get available platforms
	cl::Platform::get(&platforms);
 
    // Select the default platform and create a context using this platform and the GPU
	cl::vector<cl_context_properties> props;

	//cl::Platform platf = platforms[1];
	//char extensions[8192];
	//int err = clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, 8192 * sizeof(char), extensions, NULL);

	HGLRC glctxt;
	HDC glwndw;
	cl::fillContextProperties(platforms[0],&props,true,&glwndw,&glctxt);

	cl::STRING_CLASS buff;
	platforms[0].getInfo(CL_PLATFORM_VERSION, &buff);
	printf("%s\n",buff.c_str());

	//context = new cl::Context(PREFERRED_DEVICE_TYPE, props);
	wglMakeCurrent(glwndw,glctxt);
	context = new cl::Context(CL_DEVICE_TYPE_GPU, props);

    // Get a list of devices on this platform
	devices = context->getInfo<CL_CONTEXT_DEVICES>();
  
}

int CLContext::CreateNewQueue()
{
    // Create a command queue and use the first device
	cl::CommandQueue *queue = new cl::CommandQueue(*context, devices[0],CL_QUEUE_PROFILING_ENABLE);
	// Add it to the end of the list of command queues
	queues.push_back(queue);
	// Return it's index
	return queues.size()-1;
}

bool CLContext::IsDeviceAMD()
{
	uint devId = devices[0].getInfo<CL_DEVICE_VENDOR_ID>();
	return devId == 0x1002;
}

bool CLContext::IsDeviceIntel()
{
	uint devId = devices[0].getInfo<CL_DEVICE_VENDOR_ID>();
	return devId == 0x8086;
}

bool CLContext::IsDeviceNvidia()
{
	uint devId = devices[0].getInfo<CL_DEVICE_VENDOR_ID>();
	return devId == 0x10DE;
}

cl::STRING_CLASS CLContext::GetPlatformName()
{
	return platforms[0].getInfo<CL_PLATFORM_NAME>();
}

cl::STRING_CLASS CLContext::GetDeviceName()
{
	return devices[0].getInfo<CL_DEVICE_NAME>();
}

unsigned long CLContext::GetDeviceMemory()
{
	unsigned long mem = devices[0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
	return mem;
}

uint CLContext::GetDeviceComputeCores()
{
	return devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
}

uint CLContext::GetDeviceClock()
{
	return devices[0].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
}
