#ifndef CLPROGRAM_HEADER
#define CLPROGRAM_HEADER

#include "../DLLDefines.h"
#include <Utils/clext.h>
#include "CLContext.h"
#include <vtkOpenGL.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <stdarg.h>
#include <CLWrappers/CLMemoryObject.h>
#include <algorithm>
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLProgram
{
public:
	CLProgram(const char* fPath = 0);
	~CLProgram();

	//run a kernel with specified global and local workgroup size
	int RunKernel(cl::Kernel kernel, cl::NDRange globalSize, cl::NDRange localSize);

	std::string GetDirPathFromFilePath(std::string fPath);

	//Functions to set and get the cl program source from a text file (.cl)
	void SetProgramFile(std::string fPath) { filePath = fPath; }
	std::string GetProgramFile() const { return filePath; }

	const cl::Program &GetProgram() { return program; }

	//Set the active kernel
	void SetKernel(std::string kernel)
	{ 
		activeKernel = kernel; 
		if(std::find(actKernels.begin(), actKernels.end(), kernel) == actKernels.end())
			actKernels.push_back(kernel);
	}

	//Functions to retrieve the active kernels (kernels in use by this program)
	const cl::Kernel& GetKernel(std::string kernel, int *err = NULL);
	const cl::Kernel& GetActiveKernel() { return GetKernel(activeKernel); }
	std::vector<std::string> GetActiveKernels() const { return actKernels; }

	//Functions to share the gl objects with cl
	int StartUseMemoryObjects(std::vector<CLMemoryObject*> memObjs);
	int EndUseMemoryObjects(std::vector<CLMemoryObject*> memObjs,const cl::CommandQueue& queue );

	//Functions for rebuilding 
	std::string programBuildLog() { return buildLog; }
	virtual int BuildProgram();
	virtual int Rebuild();

protected:
	cl::CommandQueue queue;
	cl::Context context;
	cl::Program program;
	cl::vector<cl::Device> devices;
	cl::vector<cl::Kernel> kernels;

	std::string filePath;
	std::string buildLog;

	std::string activeKernel;

	std::vector<std::string> actKernels;

private:
		///
	/// Replace all occurences of a substring in a string with another
	/// string, in-place.
	///
	/// @param s String to replace in. Will be modified.
	/// @param sub Substring to replace.
	/// @param other String to replace substring with.
	///
	/// @return The string after replacements.
	/// from: http://www.velocityreviews.com/forums/t281256-quick-std-string-question.html
	inline std::string &replacein(std::string &s, const std::string &sub, const std::string &other)
	{
		size_t b = 0;
		for (;;)
		{
			b = s.find(sub, b);
			if (b == s.npos) break;
			s.replace(b, sub.size(), other);
			b += other.size();
		}
		return s;
	}

	static float totalMS;
	static int totalCalls;
};
#endif //CLPROGRAM_HEADER