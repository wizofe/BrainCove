#ifndef CLMemoryObject_HEADER
#define CLMemoryObject_HEADER

#include "../DLLDefines.h"
#include "CLContext.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLMemoryObject
{
public:
	enum PixelType
	{
		RGBA_UCHAR,
		RGBA_FLOAT,
		R_INT,
		R_UCHAR,
		R_FLOAT,
		R_SHORT
	};
	
	virtual cl::Memory CLObject() const = 0;
	virtual void SynchronizeFromCL(const cl::CommandQueue& queue) = 0;

	void CalcFormatsAndType(PixelType type,GLint *internalFormat,GLenum *format,GLenum *ptype);
	void GLToCLFormat(GLenum format,GLenum ptype,cl_channel_order *order, cl_channel_type *type);

	bool IsGLBound() { return _useGL; }
	bool IsImage() { return _isImage; }

	static int Pow2Roundup (int x);

	static bool IsImageSupported(const cl::Context& ct);
	static bool IsImage3DWriteSupported(const cl::Context& ct);
	static bool IsOpenGLInteropSupported(const cl::Context& ct);

protected:
	CLMemoryObject(const cl::Context& ct);
	~CLMemoryObject();

	void CreateTextureStore(unsigned int width, unsigned int height, void *data = 0,int *err = 0);
	void CreateGLTextureStore(unsigned int width, unsigned int height, void *data = 0,int *err = 0);

	void CreateTextureBufferStore(unsigned int width, unsigned int height, void *data = 0, int *err = 0);
	void CreateGLTextureBufferStore(unsigned int width, unsigned int height, void *data = 0, int *err = 0);

	void CreateBufferStore(size_t size, void* data,int *err = 0);
	void CreateGLBufferStore(size_t size, void* data,int *err = 0);

	void CreateGLTexture3DStore(unsigned int width, unsigned int height, unsigned int depth, void *data,int *err = 0);
	void CreateTexture3DStore(unsigned int width, unsigned int height, unsigned int depth, cl_channel_order order, cl_channel_type type, void *data,int *err = 0);
	void CreateTexture3DBufferStore(unsigned int width, unsigned int height, unsigned int depth, void *data, unsigned int pixel_size = 4, int *err = 0);
	void CreateGLTexture3DBufferStore(unsigned int width, unsigned int height, unsigned int depth, void *data, int *err = 0);

	//virtual bool IsImage() = 0;
	cl::Context context;
	
	GLuint _glTexture;
	GLuint _glBuffer;
	bool _isImage;
	bool _useGL;
	bool _bufferInitialized;

	cl::Buffer buffer;
	cl::Image2D image2d;
	cl::Image3D image3d;

};

#endif //CLMemoryObject_HEADER