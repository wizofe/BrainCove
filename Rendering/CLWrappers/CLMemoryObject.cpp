#include "CLMemoryObject.h"

CLMemoryObject::CLMemoryObject(const cl::Context& ct)
{
	this->context = ct;
	this->_bufferInitialized = false;
}

CLMemoryObject::~CLMemoryObject()
{
	if(_glTexture)
		glDeleteTextures(1,&_glTexture);
	if(_glBuffer)
		glDeleteBuffers(1,&_glBuffer);
}

void CLMemoryObject::CreateGLTextureStore(unsigned int width, unsigned int height, void *data,int *err)
{
	image2d = cl::Image2DGL(context,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,this->_glTexture,err);
}

void CLMemoryObject::CreateTextureStore(unsigned int width, unsigned int height, void *data,int *err)
{
	image2d = cl::Image2D(context,CL_MEM_WRITE_ONLY,cl::ImageFormat(CL_RGBA,CL_UNSIGNED_INT8),width,height,width*4,data,err);
}

void CLMemoryObject::CreateTextureBufferStore(unsigned int width, unsigned int height, void *data, int *err)
{
	buffer = cl::Buffer(context,CL_MEM_READ_WRITE,width*height*4,data,err);
}

void CLMemoryObject::CreateGLTextureBufferStore(unsigned int width, unsigned int height, void *data, int *err)
{
	if(!_glBuffer)
		glGenBuffers(1, &_glBuffer);
	
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _glBuffer); // bind pbo
	if(!_bufferInitialized)
	{
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB,width*height*4,0, GL_DYNAMIC_DRAW); // initialize pbo
		_bufferInitialized = true;
	}
	if(data)
	{
		void* PBOpointer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY); // obtain pointer to pbo data
		memcpy(PBOpointer,data,width*height*4); // copy data from system memory to pbo
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0); // upload texture from pbo to GPU
	}
	buffer = cl::BufferGL(context,CL_MEM_READ_WRITE,_glBuffer,err);
}

void CLMemoryObject::CreateBufferStore(size_t size, void* data,int *err)
{
	buffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,size,data,err);
}

void CLMemoryObject::CreateGLBufferStore(size_t size, void* data,int *err)
{
	if(!_glBuffer)
		glGenBuffers(1, &_glBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffer);

	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	
	if(err)
		*err = (int)glGetError();

	buffer = cl::BufferGL(context,CL_MEM_READ_ONLY,_glBuffer,err);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void CLMemoryObject::CreateGLTexture3DStore(unsigned int width, unsigned int height, unsigned int depth, void *data,int *err)
{
	image3d = cl::Image3DGL(context,CL_MEM_READ_ONLY,GL_TEXTURE_3D,0,this->_glTexture,err);
}

void CLMemoryObject::CreateTexture3DStore(unsigned int width, unsigned int height, unsigned int depth, cl_channel_order order, cl_channel_type type, void *data,int *err)
{
	image3d = cl::Image3D(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,cl::ImageFormat(order,type),width,height,depth,0,0,data,err);
}

void CLMemoryObject::CreateTexture3DBufferStore(unsigned int width, unsigned int height, unsigned int depth, void *data, unsigned int pixel_size, int *err)
{
	buffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,width*height*depth*pixel_size,data,err);
}

void CLMemoryObject::CreateGLTexture3DBufferStore(unsigned int width, unsigned int height, unsigned int depth, void *data, int *err)
{
	if(!_glBuffer)
		glGenBuffers(1, &_glBuffer);
	
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _glBuffer); // bind pbo
	if(!_bufferInitialized)
	{
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB,width*height*depth*4,0, GL_DYNAMIC_DRAW); // initialize pbo
		_bufferInitialized = true;
	}
	void* PBOpointer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY); // obtain pointer to pbo data
	memcpy(PBOpointer,data,width*height*4); // copy data from system memory to pbo
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, 0); // upload texture from pbo to GPU

	buffer = cl::BufferGL(context,CL_MEM_READ_WRITE,_glBuffer,err);
}


bool CLMemoryObject::IsImageSupported(const cl::Context& ct)
{
	cl::vector<cl::Device> devices = ct.getInfo<CL_CONTEXT_DEVICES>();
	if(devices.size()==0) return false;
	return devices[0].getInfo<CL_DEVICE_IMAGE_SUPPORT>();
}

bool CLMemoryObject::IsImage3DWriteSupported(const cl::Context& ct)
{
	cl::vector<cl::Device> devices = ct.getInfo<CL_CONTEXT_DEVICES>();
	if(devices.size()==0) return false;

	//first look in the platform extensions
	cl::Platform platform = devices[0].getInfo<CL_DEVICE_PLATFORM>();
	cl::STRING_CLASS pextensions = platform.getInfo<CL_PLATFORM_EXTENSIONS>();
	if(pextensions.find("cl_khr_3d_image_writes") != std::string::npos)
		return true;

	cl::STRING_CLASS extensions = devices[0].getInfo<CL_DEVICE_EXTENSIONS>();
	return extensions.find("cl_khr_3d_image_writes") != std::string::npos;
}

bool CLMemoryObject::IsOpenGLInteropSupported(const cl::Context& ct)
{
	cl::vector<cl::Device> devices = ct.getInfo<CL_CONTEXT_DEVICES>();
	if(devices.size()==0) return false;

	//first look in the platform extensions
	cl::Platform platform = devices[0].getInfo<CL_DEVICE_PLATFORM>();
	cl::STRING_CLASS pextensions = platform.getInfo<CL_PLATFORM_EXTENSIONS>();
	if(pextensions.find("cl_khr_gl_sharing") != std::string::npos)
		return true;

	//if not found in the platform extensions, look in the device extensions
	cl::STRING_CLASS dextensions = devices[0].getInfo<CL_DEVICE_EXTENSIONS>();
	return dextensions.find("cl_khr_gl_sharing") != std::string::npos;
}

void CLMemoryObject::CalcFormatsAndType(PixelType type,GLint *internalFormat,GLenum *format,GLenum *ptype)
{
	switch(type)
	{
	case RGBA_UCHAR:
		*internalFormat = GL_RGBA;
		*format = GL_RGBA;
		*ptype = GL_UNSIGNED_BYTE;
		break;
	case RGBA_FLOAT:
		*internalFormat = GL_RGBA;
		*format = GL_RGBA;
		*ptype = GL_FLOAT;
		break;
	case R_INT:
		*internalFormat = GL_R32I;
		*format = GL_RED_INTEGER;
		*ptype = GL_INT;
		break;
	case R_UCHAR:
		*internalFormat = GL_RGBA8UI;
		*format = GL_RED_INTEGER;
		*ptype = GL_UNSIGNED_BYTE;
		break;
	case R_FLOAT:
		*internalFormat = GL_R32F;
		*format = GL_LUMINANCE;
		*ptype = GL_FLOAT;
		break;
	case R_SHORT:
		*internalFormat = GL_R16I;
		*format = GL_RED_INTEGER;
		*ptype = GL_SHORT;
		break;
	}
}

void CLMemoryObject::GLToCLFormat(GLenum format,GLenum ptype,cl_channel_order *order, cl_channel_type *type)
{
	switch(format)
	{
	case GL_RGBA:
	case GL_RGBA32I:
		*order = CL_RGBA;
		break;
	case GL_R:
	case GL_RED:
	case GL_RED_INTEGER:
	case GL_LUMINANCE:
		*order = CL_R;
		break;
	default:
		break;
	}
	switch(ptype)
	{
	case GL_UNSIGNED_INT:
		*type = CL_UNSIGNED_INT32;
		break;
	case GL_INT:
		*type = CL_SIGNED_INT32;
		break;
	case GL_UNSIGNED_BYTE:
		*type = CL_UNSIGNED_INT8;
		break;
	case GL_FLOAT:
		*type = CL_FLOAT;
		break;
	case GL_SHORT:
		*type = CL_SIGNED_INT16;
		break;
	case GL_UNSIGNED_SHORT:
		*type = CL_UNSIGNED_INT16;
		break;
	default:
		break;
	}
}


int CLMemoryObject::Pow2Roundup (int x)
{
	if (x < 0)
		return 0;
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x = x + 1;
	//if(x<16)
	//	x = 16;
	return x;
}
