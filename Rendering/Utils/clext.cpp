#include "clext.h"

namespace cl
{

	cl::Program buildProgramFromFile(const cl::Context& context,
									 const std::string& filename, 
									 const std::string& options,
									 std::string *buildLog, int *errcl)
	{
		*errcl = 0;

		std::ifstream fin(filename.c_str());
		fin.seekg(0, std::ios_base::end);
		std::vector<char> buffer(fin.tellg());
		fin.seekg(0);
		fin.read(&buffer[0], buffer.size());

		cl::Program program(context, cl::Program::Sources(1, std::make_pair(&buffer[0], buffer.size())));
		
		if(*errcl = program.build(context.getInfo<CL_CONTEXT_DEVICES>(), options.c_str()))
		{
			program.getBuildInfo<cl::STRING_CLASS>(context.getInfo<CL_CONTEXT_DEVICES>()[0],CL_PROGRAM_BUILD_LOG, buildLog);

			printf("%s\n",buildLog->c_str());
		}
		return program;
	}

	cl::Program buildProgramFromResource(const cl::Context& context,
									 const std::string& resourcePath, 
									 const std::string& options,
									 std::string *buildLog, int *errcl)
	{
		QFile file(resourcePath.c_str());
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			throw std::exception("Cannot open resource file");

		QTextStream in(&file);
		QString text = in.readAll();

		cl::Program program(context, cl::Program::Sources(1, std::make_pair(text.toStdString().c_str(), text.length())));
		if(*errcl = program.build(context.getInfo<CL_CONTEXT_DEVICES>(), options.c_str()))
		{
			program.getBuildInfo<cl::STRING_CLASS>(context.getInfo<CL_CONTEXT_DEVICES>()[0],CL_PROGRAM_BUILD_LOG, buildLog);
			printf("%s\n",buildLog->c_str());
		}
		return program;
	} 

	void fillContextProperties(const cl::Platform& platform, cl::vector<cl_context_properties>* props, 
							   bool gl_sharing, HDC *glWndw, HGLRC *glCtxt)
	{
		props->push_back(CL_CONTEXT_PLATFORM);
		props->push_back((cl_context_properties)platform());

		if(gl_sharing)
		{
			std::string exts;
			platform.getInfo(CL_PLATFORM_EXTENSIONS,&exts);
			//std::string exts = platform.getInfo<CL_PLATFORM_EXTENSIONS>();
			std::string dexts;
			cl::vector<cl::Device> devices;
			platform.getDevices(PREFERRED_DEVICE_TYPE, &devices);
			if(devices.front()())
				dexts = devices.front().getInfo<CL_DEVICE_EXTENSIONS>();
			if(exts.find("cl_khr_gl_sharing") != std::string::npos ||
				dexts.find("cl_khr_gl_sharing") != std::string::npos)
			{
	#ifdef _WIN32
				HDC hDC = wglGetCurrentDC();
				//if(glWndw)
				//	*glWndw = hDC;
				HGLRC hRC = wglGetCurrentContext();
				//if(glCtxt)
				//	*glCtxt = hRC;
				if((hDC != NULL) && (hRC != NULL))
				{
					props->push_back(CL_GL_CONTEXT_KHR);
					props->push_back((cl_context_properties)hRC);
					props->push_back(CL_WGL_HDC_KHR);
					props->push_back((cl_context_properties)hDC);
				}
	#endif
			}
		}

		props->push_back(0);
		glewInit();
	}
}