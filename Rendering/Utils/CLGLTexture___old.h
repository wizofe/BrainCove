#ifndef CLGLTEXTURE_HEADER
#define CLGLTEXTURE_HEADER

#include "../DLLDefines.h"
#include "clext.h"
#include "CLContext.h"
#include <vtkOpenGL.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLGLTexture : public vtkObject
{
public:
	CLGLTexture(unsigned int width, unsigned int height, const CLContext &clCntxt, cl_mem_flags mFlags, unsigned char *texels = NULL);
	~CLGLTexture();

	void glBind();
	void glUnbind();

	void SetData(unsigned char * texels);
	void GetData(unsigned char * texels);

	unsigned int TextureWidth() { return texWidth; }
	unsigned int TextureHeight() { return texHeight; }

	unsigned int ImageWidth() { return imageWidth; }
	unsigned int ImageHeight() { return imageHeight; }

	unsigned int NextPow2(unsigned int nonPow2) { return Pow2Roundup(nonPow2); }

	void Resize(unsigned int width, unsigned int height);

	vtkGetMacro( GLTextureId, GLuint );
	inline cl::Image2DGL GetCLBuffer() { return CLBuffer; }

	static CLGLTexture *New();

	// Round up to next higher power of 2 (return x if it's already a power
	/// of 2).
	static int Pow2Roundup (int x);

private:
	GLuint GLTextureId;
	GLuint texWidth;
	GLuint texHeight;

	GLuint imageWidth;
	GLuint imageHeight;

	cl::Image2DGL CLBuffer;
	cl::Context *CLContxt;

	cl_mem_flags CLMemFlags;

};
#endif //CLGLTEXTURE_HEADER