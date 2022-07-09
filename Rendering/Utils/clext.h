#ifndef CLEXT_HPP
#define CLEXT_HPP

#define __NO_STD_VECTOR // Use cl::vector instead of STL version

#define CL_GL_CONTEXT_KHR                       0x2008
#define CL_EGL_DISPLAY_KHR                      0x2009
#define CL_GLX_DISPLAY_KHR                      0x200A
#define CL_WGL_HDC_KHR                          0x200B
#define CL_CGL_SHAREGROUP_KHR					0x200C

//#ifdef USE_GLEW
#include <GL/glew.h>
//#endif

#include <Utils/cl.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <typeinfo.h>

#include <QObject>
#include <QFile>
#include <QTextStream>

namespace cl
{
	cl::Program buildProgramFromFile(const cl::Context& context,
									 const std::string& filename, 
									 const std::string& options,
									 std::string* buildLog, int *err);

	cl::Program buildProgramFromResource(const cl::Context& context,
									 const std::string& resourcePath, 
									 const std::string& options,
									 std::string* buildLog, int *err);

	void fillContextProperties(const cl::Platform& platform, cl::vector<cl_context_properties>* props, 
							   bool gl_sharing, HDC *glWndw = 0, HGLRC *glCtxt = 0);

	struct KernelArg
	{
		::size_t arg_size;
		bool isMemory;
		void *arg;

		KernelArg(::size_t sz, bool isMem, void *a)
		{
			arg_size = sz;
			isMemory = isMem;
			arg = a;
		}
	} ;
	typedef struct KernelArg KernelArgument;
}
#endif //CLEXT_HPP