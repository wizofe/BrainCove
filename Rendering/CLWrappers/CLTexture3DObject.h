#ifndef CLTexture3DObject_HEADER
#define CLTexture3DObject_HEADER

#include "../DLLDefines.h"
#include "CLMemoryObject.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
class Rendering_EXPORT CLTexture3DObject : public CLMemoryObject
{
public:
	~CLTexture3DObject();

	static CLTexture3DObject* New(const cl::Context& ct,PixelType type,unsigned int width, unsigned int height,unsigned int depth, bool enableWrite, void* data);

	virtual void SynchronizeFromCL(const cl::CommandQueue& queue);

	void UpdateFromHostData(unsigned int width, unsigned int height, unsigned int depth, void *data);
	void CopyToHostData(unsigned char *allocated_data);

	void Resize(unsigned int newWidth, unsigned int newHeight, unsigned int newDepth);

	size_t ImageWidth() { return _width; }
	size_t ImageHeight() { return _height; }
	size_t ImageDepth() { return _depth; }

	size_t TextureWidth() { return _texWidth; }
	size_t TextureHeight() { return _texHeight; }
	size_t TextureDepth() { return _texDepth; }

	void Bind() { if(_glTexture) glBindTexture(GL_TEXTURE_3D, _glTexture); }

	virtual cl::Memory CLObject() const 
	{ 
		if(_isImage) return image3d; 
		else return buffer;
	}

	virtual cl::Image3D CLImage() const
	{
		return image3d;
	}

	virtual cl::Buffer CLBuffer() const
	{
		return buffer;
	}

protected:
	CLTexture3DObject(const cl::Context& ct, bool useImages, bool useGL,PixelType type,unsigned int width, unsigned int height,unsigned int depth, void* data);
	
	unsigned int _width;
	unsigned int _height;
	unsigned int _depth;

	unsigned int _texWidth;
	unsigned int _texHeight;
	unsigned int _texDepth;

	GLint internalFormat;
	GLenum format, ptype;
	cl_channel_order order;
	cl_channel_type ctype;
};

#endif //CLTexture3DObject_HEADER