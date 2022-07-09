#include "CLTexture3DObject.h"

CLTexture3DObject* CLTexture3DObject::New(const cl::Context& ct,PixelType type,unsigned int width, unsigned int height,unsigned int depth, bool enableWrite, void* data)
{
	bool useGL = CLMemoryObject::IsOpenGLInteropSupported(ct);
	bool useIm = CLMemoryObject::IsImage3DWriteSupported(ct) || !enableWrite;	

	return new CLTexture3DObject(ct,useIm,false,type,width,height,depth,data);
}

CLTexture3DObject::CLTexture3DObject(const cl::Context& ct, bool useImages, bool useGL,PixelType type,unsigned int width, unsigned int height,unsigned int depth, void* data) : CLMemoryObject(ct)
{
	int err = 0;

	_isImage = useImages;
	_useGL = useGL;
	_glTexture = 0;
	_glBuffer = 0;

	/*_texWidth = Pow2Roundup(width);
	_texHeight = Pow2Roundup(height);
	_texDepth = Pow2Roundup(depth);
	*/
	_texWidth = width;
	_texHeight = height;
	_texDepth = depth;
	
	_width = width;
	_height = height;
	_depth = depth;

	CalcFormatsAndType(type,&internalFormat,&format,&ptype);

	if(useImages && useGL)
	{
		glEnable(GL_TEXTURE_3D);
		glGenTextures(1,&_glTexture);
		glBindTexture(GL_TEXTURE_3D,_glTexture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, _texWidth, _texHeight, _texDepth, 0, format, ptype, 0);
		GLenum errg = glGetError();
		glBindTexture(GL_TEXTURE_3D,_glTexture);
		errg = glGetError();
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, _width, _height, _depth, format, ptype, reinterpret_cast<unsigned char*>(data));
		errg = glGetError();
		CreateGLTexture3DStore(width,height,depth,data,&err);
	}
	else if(useImages)
	{
		//glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, _texWidth, _texHeight, _texDepth, 0, format, ptype, 0);
		GLToCLFormat(format,ptype,&order,&ctype);
		//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, _width, _height, _depth, format, ptype,data);
		CreateTexture3DStore(width,height,depth,order,ctype,data,&err);
	}
	else if(useGL)
		CreateGLTexture3DBufferStore(width,height,depth,data);
	else
	{
		//glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, _texWidth, _texHeight, _texWidth,0, format, ptype, 0);
		//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0,_width, _height, _width, format, ptype,data);
		CreateTexture3DBufferStore(width,height,depth,data,4); 
	}
	glBindTexture(GL_TEXTURE_3D,0);
}

CLTexture3DObject::~CLTexture3DObject()
{
}

void CLTexture3DObject::SynchronizeFromCL(const cl::CommandQueue& queue)
{
	glBindTexture(GL_TEXTURE_3D,_glTexture);
	size_t bufsize = _width*_height*_depth*4;
	if(!_useGL) 
	{
		unsigned char *tempdata = new unsigned char[bufsize];
		if(!_isImage) //copy from cl buffer to host buffer
			queue.enqueueReadBuffer(buffer,CL_TRUE,0,bufsize,tempdata,0,0);
		else		  //copy from cl image to host buffer
		{
			cl::size_t<3> origin;
			origin.push_back(0); origin.push_back(0); origin.push_back(0);
			cl::size_t<3> size;
			size.push_back(_width); origin.push_back(_height); origin.push_back(_depth);
			queue.enqueueReadImage(image3d,CL_TRUE,origin,size,_width*4,_width*_height*4,tempdata,0,0);
		}
		//copy the host buffer to gl texture
		glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,_width,_height,_depth,GL_RGBA,GL_UNSIGNED_BYTE,tempdata);
		delete [] tempdata;
	}
	else
	{
		if(!_isImage) //copy from gl buffer to gl texture
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _glBuffer); // bind pbo
			glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,_width,_height,_depth,GL_RGBA,GL_UNSIGNED_BYTE,0); //copy from pbo to texture
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0); // unbind pbo
		}
		else		  //no copy required
		{
		}
	}
	glBindTexture(GL_TEXTURE_3D, 0);
}

void CLTexture3DObject::UpdateFromHostData(unsigned int width, unsigned int height, unsigned int depth, void *data)
{
	int err = 0;
	glBindTexture(GL_TEXTURE_3D,_glTexture);

	//update the texture
	/*unsigned int tw = Pow2Roundup(width);
	unsigned int th = Pow2Roundup(height);
	unsigned int td = Pow2Roundup(depth);*/
	unsigned int tw = width;
	unsigned int th = height;
	unsigned int td = depth;
	if(tw != _texWidth || th != _texHeight || td != _texDepth)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,tw,th,td,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
		_texWidth = tw;
		_texHeight = th;
		_texDepth = td;
	}
	_width = width;
	_height = height;
	_depth = depth;

	if(!data) //there's nothing to update
		return;

	if(_isImage && _useGL)
	{
		glTexSubImage3D(GL_TEXTURE_3D, 0,0,0,0, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data);	
		CreateGLTexture3DStore(width,height,depth,data);
	}
	else if(_isImage)
	{
		glTexSubImage3D(GL_TEXTURE_3D, 0,0,0,0, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data);	
		CreateTexture3DStore(width,height,depth,order,ctype,data);
	}
	else if(_useGL)
		CreateGLTexture3DBufferStore(width,height,depth,data,&err);
	else
	{
		glTexSubImage3D(GL_TEXTURE_3D, 0,0,0,0, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data);	
		CreateTexture3DBufferStore(width,height,depth,data); 
	}

	glBindTexture(GL_TEXTURE_3D,0);

}

void CLTexture3DObject::Resize(unsigned int newWidth, unsigned int newHeight, unsigned int newDepth)
{
	UpdateFromHostData(newWidth,newHeight,newDepth,0);
}

void CLTexture3DObject::CopyToHostData(unsigned char *allocated_data)
{
	if(!allocated_data)
		return;

	glBindTexture(GL_TEXTURE_3D,_glTexture);
	glGetTexImage(GL_TEXTURE_3D,0,GL_RGBA,GL_UNSIGNED_BYTE,allocated_data);
	glBindTexture(GL_TEXTURE_3D,0);

}