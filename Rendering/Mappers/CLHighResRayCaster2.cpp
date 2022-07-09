#include "CLHighResRayCaster.h"

CLHighResRayCaster::CLHighResRayCaster(std::string kernel) : CLRayCaster(kernel)
{
	CreateColorMap();
	MaskData = NULL;
	MaskTexture = NULL;
}

CLHighResRayCaster::~CLHighResRayCaster()
{
}

void CLHighResRayCaster::CreateColorMap()
{
	std::string dvtfFile = std::string(DATA_DIR);
	dvtfFile = dvtfFile.append("/brain.dvtf");
	vtkLookupTable *lut = ColorMapping::ImportFromDevideTF(dvtfFile.c_str());
	lutTexture = CLTexture2DObject::New(*CLContext::Instance().Context(),CLMemoryObject::RGBA_UCHAR, lut->GetNumberOfColors(), 1, lut->GetTable()->GetPointer(0));
}

void CLHighResRayCaster::RecreateTexture()
{
	int err = 0;
	int dims[3];
	this->VolumeData->GetDimensions(dims);

	if(this->volumeTexture)
		delete this->volumeTexture;

	unsigned char *voxel = reinterpret_cast<unsigned char*>(this->VolumeData->GetScalarPointer(128,238,87));
	this->volumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_UCHAR, dims[0],dims[1],dims[2],this->VolumeData->GetScalarPointer());
}

void CLHighResRayCaster::SetMaskData(vtkImageData *mask)
{
	int err = 0;

	if(mask == this->MaskData)
		return;

	if(this->MaskData)
		this->MaskData->Delete();

	MaskData = mask;

	int dims[3];
	this->MaskData->GetDimensions(dims);
	double origin[3];
	this->MaskData->GetOrigin(origin);

	if(this->MaskTexture)
		delete this->MaskTexture;

	int *voxel = reinterpret_cast<int*>(this->MaskData->GetScalarPointer(7-7,24-7,17-4));
	MaskTexture = CLTexture3DObject::New(context,CLMemoryObject::R_INT,dims[0],dims[1],dims[2],MaskData->GetScalarPointer());
}

bool CLHighResRayCaster::SelectionChanged(std::map<int,float> voxel_selection)
{
	if(voxel_selection.size()!=last_voxel_selection.size())
	{
		last_voxel_selection = voxel_selection;
		return true;
	}
	else
	{
	}
	return true;
}

int CLHighResRayCaster::ToFirstMultipleOf(int val, int multiple)
{
	int m = val;	
	while(m%multiple!=0)
		m++;	
	return m;
}

int CLHighResRayCaster::CastRays(CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, std::map<int,float> voxel_selection, double correlation)
{
	int err = 0, arg = 0;

	cl::Kernel kernel = GetKernel("raycastHighResVolume", &err); if(err) return err;	
	unsigned int matDim = Matrix.size;
	float corr = (float)correlation;
	int method = GetRaycastingMethod();
	uint numVoxels = voxel_selection.size();
	uint imsize[2];
	imsize[0] = texture->ImageWidth();
	imsize[1] = texture->ImageHeight();
	
	cl::Buffer rowBoundsBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(int)*2*texture->TextureHeight(),rowBounds,&err); if(err) return err;	
	//cl::Buffer selectBuffer1(context,CL_MEM_READ_ONLY,sizeof(int)*numVoxels,0,&err); if(err) return err;	
	//cl::Buffer selectBuffer2(context,CL_MEM_READ_ONLY,sizeof(float)*numVoxels,0,&err); if(err) return err;	
	this->UpdateSelectionBuffers(voxel_selection);
	int dims[3];
	this->VolumeData->GetDimensions(dims);

	//cl::Buffer volumeBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(cl_uchar)*dims[0]*dims[1]*dims[2],this->VolumeData->GetScalarPointer(),&err); if(err) return err;	
	//unsigned char *data = (unsigned char*)this->VolumeData->GetScalarPointer();
	
	std::vector<CLMemoryObject*> memsGL;
	memsGL.push_back(lutTexture);
	memsGL.push_back(texture);
	memsGL.push_back(matrixBufferGPU);
	memsGL.push_back(volumeTexture);
	memsGL.push_back(MaskTexture);

	CameraParameters params = CreateCameraStruct(texture,viewPortSize,imageOrigin,VolumeData,viewToVoxelsMatrix,cameraThickness);
	cl::Buffer camBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(CameraParameters),&params,&err);

	err |= kernel.setArg(arg++,matrixBufferGPU->CLObject());
	err |= kernel.setArg(arg++,texture->CLObject());
	err |= kernel.setArg(arg++,lutTexture->CLObject());
	err |= kernel.setArg(arg++,volumeTexture->CLObject());
	err |= kernel.setArg(arg++,MaskTexture->CLObject());
	//err |= kernel.setArg(arg++,volumeBuffer);
	err |= kernel.setArg(arg++,camBuffer);
	err |= kernel.setArg(arg++,selectionIdsBuffer);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&numVoxels);
	err |= kernel.setArg(arg++,rowBoundsBuffer);
	err |= kernel.setArg(arg++,sizeof(cl_uint2),(void*)imsize);
	if(err) return err;

	err = StartUseMemoryObjects(memsGL);
	err = RunKernel(kernel, cl::NDRange(texture->TextureWidth(), texture->TextureHeight()), cl::NDRange(16,16));
	err = EndUseMemoryObjects(memsGL,queue);

	err = queue.finish();
	return err;
}
