#include "CLGLTexture.h"

CLGLTexture::CLGLTexture(unsigned int width, unsigned int height, const CLContext &clCntxt, cl_mem_flags mFlags, unsigned char *texels)
{
	int err = 0;
	CLContxt = clCntxt.Context();

	CLMemFlags = mFlags;

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1,&GLTextureId);

	imageWidth = width;
	imageHeight = height;

	texWidth = Pow2Roundup(width);
	texHeight = Pow2Roundup(height);

	glBind();

	//create the OpenGL texture
    /* Set parameters */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//create the OpenCL image
	CLBuffer = cl::Image2DGL(*CLContxt, CLMemFlags, GL_TEXTURE_2D, 0, GLTextureId, &err);

	if(texels)
		SetData(texels);

	glUnbind();

	glPopAttrib();

}

CLGLTexture::~CLGLTexture()
{
	glDeleteTextures(1, &GLTextureId);
}

void CLGLTexture::glBind()
{
	glBindTexture(GL_TEXTURE_2D, GLTextureId);

}

void CLGLTexture::glUnbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


void CLGLTexture::SetData(unsigned char * texels)
{
	glBind();

    /* Set parameters */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, texels);	
}

void CLGLTexture::GetData(unsigned char *texels)
{
	glBind();

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
}

void CLGLTexture::Resize(unsigned int width, unsigned int height)
{
	int err = 0;
	imageWidth = width;
	imageHeight = height;

	int tw = Pow2Roundup(width);
	int th = Pow2Roundup(height);
	
	//same texture size, no need to resize
	if(tw == texWidth && th == texHeight)
		return;

	texWidth = max(16,tw);
	texHeight = max(16,th);

	//different texture size:
	//  delete old image and texture
	//delete &CLBuffer;

	glDeleteTextures(1, &GLTextureId);

	//and generate new ones
	glGenTextures(1,&GLTextureId);

	glBind();

	//create the OpenGL texture
    /* Set parameters */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//create the OpenCL image
	CLBuffer = cl::Image2DGL(*CLContxt, CLMemFlags, GL_TEXTURE_2D, 0, GLTextureId, &err);

	printf("New size: %d x %d\n",texWidth,texHeight);
}

// Round up to next higher power of 2 (return x if it's already a power
/// of 2).
int CLGLTexture::Pow2Roundup (int x)
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