#include "CLTexture2DObject.h"

CLTexture2DObject* CLTexture2DObject::New(const cl::Context& ct,PixelType type,unsigned int width, unsigned int height,void* data)
{
	bool useGL = CLMemoryObject::IsOpenGLInteropSupported(ct);
	bool useIm = CLMemoryObject::IsImageSupported(ct);	

	return new CLTexture2DObject(ct,useIm,useGL,type,width,height,data);
}

unsigned char* GetDummyData(int size)
{
	unsigned char* data = new unsigned char[size];
	for(int i = 0; i< size; i++)
	{
		data[i]=i%(size/10);
	}
	return data;
}

CLTexture2DObject::CLTexture2DObject(const cl::Context& ct, bool useImages, bool useGL, PixelType type, unsigned int width, unsigned int height,void* data) : CLMemoryObject(ct)
{
	_isImage = useImages;
	_useGL = useGL;
	_glTexture = 0;
	_glBuffer = 0;

	glGenTextures(1,&_glTexture);
	glBindTexture(GL_TEXTURE_2D,_glTexture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	_texWidth = Pow2Roundup(width);
	_texHeight = Pow2Roundup(height);
	_width = width;
	_height = height;

	CalcFormatsAndType(type,&internalFormat,&format,&ptype);
	
	if(useImages && useGL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _texWidth, _texHeight, 0, format, ptype, 0);
		GLenum errg = glGetError();
		//if(!data)
		//	data = GetDummyData(_texWidth*_texHeight*4);
		if(data) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, format, ptype, data);
		errg = glGetError();
		CreateGLTextureStore(width,height,data,(int*)&errg);
	}
	else if(useImages)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _texWidth, _texHeight, 0, format, ptype, 0);
		if(data)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, format, ptype, data);
		CreateTextureStore(width,height,data);
	}
	else if(useGL)
		CreateGLTextureBufferStore(width,height,data);
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _texWidth, _texHeight, 0, format, ptype, 0);
		if(data)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, format, ptype, data);
		CreateTextureBufferStore(width,height,data); 
	}
	glBindTexture(GL_TEXTURE_2D,0);
}

CLTexture2DObject::~CLTexture2DObject()
{
}

void CLTexture2DObject::SynchronizeFromCL(const cl::CommandQueue& queue)
{
	glBindTexture(GL_TEXTURE_2D,_glTexture);
	size_t bufsize = _width*_height*4;
	if(!_useGL) 
	{
		unsigned char *tempdata = new unsigned char[bufsize];
		if(!_isImage) //copy from cl buffer to gl texture
			queue.enqueueReadBuffer(buffer,CL_TRUE,0,bufsize,tempdata,0,0);
		else		  //copy from cl image to gl texture
		{
			cl::size_t<3> origin;
			origin.push_back(0); origin.push_back(0); origin.push_back(0);
			cl::size_t<3> size;
			size.push_back(_width); origin.push_back(_height); origin.push_back(0);
			queue.enqueueReadImage(image2d,CL_TRUE,origin,size,_width*4,_width*_height*4,tempdata,0,0);
		}
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,_width,_height,format,ptype,tempdata);
		delete [] tempdata;
	}
	else
	{
		if(!_isImage) //copy from gl buffer to gl texture
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _glBuffer); // bind pbo
			glTexSubImage2D(GL_TEXTURE_2D,0,0,0,_width,_height,format,ptype,0); //copy from pbo to texture
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0); // unbind pbo
		}
		else		  //no copy required
		{
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CLTexture2DObject::UpdateFromHostData(unsigned int width, unsigned int height, unsigned char *data)
{
	int err = 0;

	//update the texture
	_width = width;
	_height = height;

	unsigned int tw = Pow2Roundup(width);
	unsigned int th = Pow2Roundup(height);
	if(tw != _texWidth || th != _texHeight)
	{
		glDeleteTextures(1,&_glTexture);
		glGenTextures(1,&_glTexture);
		glBindTexture(GL_TEXTURE_2D,_glTexture);
		glEnable(GL_TEXTURE_2D); 
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D,0,internalFormat,tw,th,0,format,ptype,0);
		err = (int)glGetError();
		_texWidth = tw;
		_texHeight = th;
	}
	else if(data == NULL)
		return;

	glBindTexture(GL_TEXTURE_2D,_glTexture);
	if(_isImage && _useGL)
	{
		if(data)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, width, height, format, ptype, data);	
		err = (int)glGetError();
		CreateGLTextureStore(width,height,data,&err);
	}
	else if(_isImage)
	{
		if(data)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, width, height, format, ptype, data);		
		err = (int)glGetError();
		CreateTextureStore(width,height,data,&err);
	}
	else if(_useGL)
		CreateGLTextureBufferStore(width,height,data,&err);
	else
	{
		if(data)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, width, height, format, ptype, data);		
		err = (int)glGetError();
		CreateTextureBufferStore(width,height,data,&err); 
	}

	glBindTexture(GL_TEXTURE_2D,0);

}

void CLTexture2DObject::Resize(unsigned int newWidth, unsigned int newHeight)
{
	UpdateFromHostData(newWidth,newHeight,0);
}

void CLTexture2DObject::CopyToHostData(unsigned char *allocated_data)
{
	if(!allocated_data)
		return;

	glBindTexture(GL_TEXTURE_2D,_glTexture);
	glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,allocated_data);
	glBindTexture(GL_TEXTURE_2D,0);

}