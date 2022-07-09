#include "CLProgram.h"

float CLProgram::totalMS = 0;
int CLProgram::totalCalls = 0;

CLProgram::CLProgram(const char* fPath) 
{
	int cmdQIdx = CLContext::Instance().CreateNewQueue();
	this->queue = *CLContext::Instance().GetQueue(cmdQIdx);

	queue.getInfo<cl::Context>(CL_QUEUE_CONTEXT,&this->context);
	queue.getInfo<cl::Device>(CL_QUEUE_DEVICE,&this->devices[0]);

	if(fPath)
		filePath = std::string(fPath);
}

CLProgram::~CLProgram()
{

}

int CLProgram::RunKernel(cl::Kernel kernel, cl::NDRange globalSize, cl::NDRange localSize)
{
	int err = 0;
	size_t threads;
	err = kernel.getWorkGroupInfo<size_t>(devices[0],CL_KERNEL_WORK_GROUP_SIZE,&threads); if(err) return err;

	err = queue.enqueueNDRangeKernel(kernel,cl::NullRange,globalSize,localSize,0,0); if(err) return err;
	err = queue.finish(); if(err) return err;
	return err;
}

int CLProgram::StartUseMemoryObjects(std::vector<CLMemoryObject*> memObjs)
{
	//aaaaaaaaaargh!!! undocumented, but immensely important for better frame rates
	//glFinish();

	cl::vector<cl::Memory> clglMems;
	int err = 0;
	for(int i = 0; i < memObjs.size(); i++)
		if(memObjs.at(i)->IsGLBound())
			clglMems.push_back(memObjs.at(i)->CLObject());
	err = queue.enqueueAcquireGLObjects(&clglMems,0,0); 
	return err;
}

int CLProgram::EndUseMemoryObjects(std::vector<CLMemoryObject*> memObjs, const cl::CommandQueue& queue)
{
	cl::vector<cl::Memory> clglMems;
	int err = 0;
	for(int i = 0; i < memObjs.size(); i++)
		if(memObjs.at(i)->IsGLBound())
		{
			clglMems.push_back(memObjs.at(i)->CLObject());
			//memObjs.at(i)->SynchronizeFromCL(queue);
		}
	err = queue.enqueueReleaseGLObjects(&clglMems,0,0);

	return err;
}

int CLProgram::BuildProgram()
{
	int err = 0;
	std::string options = std::string("");
	if(!CLMemoryObject::IsImage3DWriteSupported(context))
		options = options.append(" -D NO_IM3D_WRITE_SUPPORT");

#ifdef _WIN32 //stupid NVIDIA bug on Windows: spaces in the path result in a crash, even with quoted paths
	char buf2[MAX_PATH];
	int lenghth = GetShortPathName(GetDirPathFromFilePath(filePath).c_str(),buf2,MAX_PATH);
	options = options.append(" -I ").append(buf2);
#else
	options = options.append(" -I \"").append(GetDirPathFromFilePath(filePath)).append("\"");
#endif

	program = cl::buildProgramFromFile(context,filePath,options,&buildLog,&err); //cl::buildProgramFromFile(context,"kernel.cl","");
	if(err == CL_SUCCESS)
		program.createKernels(&kernels);

	return err;
}

const cl::Kernel& CLProgram::GetKernel(std::string kernel, int *err)
{
	cl::STRING_CLASS buff;

	for(int i = 0; i < kernels.size();i++)
	{
		kernels[i].getInfo<cl::STRING_CLASS>(CL_KERNEL_FUNCTION_NAME,&buff);
		if(strcmp(buff.c_str(),kernel.c_str())==0)
			return kernels[i];
	}
	if(err)
		*err = -1; //kernel with specified name not found
	return cl::Kernel();
}

std::string CLProgram::GetDirPathFromFilePath(std::string fPath)
{
	std::string fPathStdSep = replacein(fPath,"/","\\");
	return fPathStdSep.substr(0,fPath.find_last_of("\\"));
}

int CLProgram::Rebuild()
{
	return BuildProgram();
}