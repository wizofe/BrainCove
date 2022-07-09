#ifndef CLTexture2DObject_HEADER
#define CLTexture2DObject_HEADER

#include "../DLLDefines.h"
#include "CLMemoryObject.h"
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/
class Rendering_EXPORT CLTexture2DObject : public CLMemoryObject
{
public:
	~CLTexture2DObject();

	static CLTexture2DObject* New(const cl::Context& ct,PixelType type,unsigned int width, unsigned int height,void* data);

	virtual void SynchronizeFromCL(const cl::CommandQueue& queue);

	void UpdateFromHostData(unsigned int width, unsigned int height, unsigned char *data);
	void CopyToHostData(unsigned char *allocated_data);

	void Resize(unsigned int newWidth, unsigned int newHeight);

	size_t ImageWidth() { return _width; }
	size_t ImageHeight() { return _height; }

	size_t TextureWidth() { return _texWidth; }
	size_t TextureHeight() { return _texHeight; }

	void Bind() { if(_glTexture) glBindTexture(GL_TEXTURE_2D, _glTexture); }

	virtual cl::Memory CLObject() const 
	{ 
		if(_isImage) return image2d; 
		else return buffer;
	}


protected:
	CLTexture2DObject(const cl::Context& ct, bool useImages, bool useGL, PixelType type, unsigned int width, unsigned int height,void* data);
	
	unsigned int _width;
	unsigned int _height;

	unsigned int _texWidth;
	unsigned int _texHeight;

	GLint internalFormat;
	GLenum format, ptype;
};

#endif //CLMemoryObject_HEADER