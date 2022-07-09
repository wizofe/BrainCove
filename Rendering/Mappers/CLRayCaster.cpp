#include "CLRayCaster.h"


CLRayCaster::CLRayCaster(std::string programFile) : CLProgram(programFile.c_str())
{
	BuildProgram();
	int err = 0;
	this->RaycastVolumeTexture = NULL;
	this->RaycastVolume = NULL;
	this->LutTexture = NULL;
	this->SplatTexture = NULL;
	this->Layout = NULL;
	this->IsComparing = false;

	this->Matrix = NULL;
	this->ComparisonMatrix = NULL;
}

CLRayCaster::~CLRayCaster()
{
	if(this->RaycastVolumeTexture)
		delete this->RaycastVolumeTexture;
	if(this->RaycastVolume)
		this->RaycastVolume->Delete();
	if(this->LutTexture)
		delete this->LutTexture;
	if(this->SplatTexture)
		delete this->SplatTexture;
}

void CLRayCaster::CreateDefaultColorMap()
{
	SetLookupTable(ColorMapping::BlueToYellow());
}

void CLRayCaster::CreateDefaultLayout()
{
	BrainLayout1D *brlay = new BrainLayout1D();

	std::vector<int> pos;
	for(int i = 0; i < this->Matrix->Dimension(); i++)
		pos.push_back(i);
	brlay->SetPositions(pos);

	SetLayout(brlay);
}

void CLRayCaster::ReCreateSplatTexture(int w, int h)
{
	if(this->SplatTexture)
	{
		if(!this->SplatTexture->ImageWidth() == w || !this->SplatTexture->ImageHeight() == h)
			this->SplatTexture->Resize(w,h);
	}
	else
		this->SplatTexture = CLTexture2DObject::New(*CLContext::Instance().Context(),CLMemoryObject::R_FLOAT,w,h,0);
}

void CLRayCaster::SetLookupTable(vtkLookupTable *lut)
{
		//create clgl texture from transfer function
	if(LutTexture)
		delete LutTexture;
	LutTexture = CLTexture2DObject::New(*CLContext::Instance().Context(),CLMemoryObject::RGBA_UCHAR,256,1,lut->GetTable()->GetPointer(0));
}

void CLRayCaster::SetDataFile(DataFile *file)
{
	this->Matrix = file->matrix();
	//create cl buffer from float matrix
	if(CLContext::Instance().ContainsSharedMemory(file))
		this->MatrixBufferGPU = (CLBufferObject*)CLContext::Instance().GetSharedMemory(file);
	else
	{
		size_t sz = file->matrix()->NumElements()*sizeof(cl_float);
		CLBufferObject *cl_buff = CLBufferObject::New(this->context,sz,file->matrix()->RawData());
		this->MatrixBufferGPU = cl_buff;
		CLContext::Instance().SetSharedMemory(file,cl_buff);
	}
	if(!this->Layout)
		CreateDefaultLayout();
	if(!this->LutTexture)
		CreateDefaultColorMap();
}

void CLRayCaster::SetComparisonDataFile(DataFile* file)
{
	if(!file)
		return;

	this->ComparisonMatrix = file->matrix();
	//create cl buffer from float matrix
	if(CLContext::Instance().ContainsSharedMemory(file))
		this->ComparisonMatrixBufferGPU = (CLBufferObject*)CLContext::Instance().GetSharedMemory(file);
	else
	{
		size_t sz = file->matrix()->NumElements()*sizeof(cl_float);
		CLBufferObject *cl_buff = CLBufferObject::New(this->context,sz,file->matrix()->RawData());
		this->ComparisonMatrixBufferGPU = cl_buff;
		CLContext::Instance().SetSharedMemory(file,cl_buff);
	}
}

void CLRayCaster::SetLayout(BaseLayout* layout)
{
	BrainLayout1D *lay = dynamic_cast<BrainLayout1D*>(layout);
	if(!lay)
		return;

	if(this->Layout == layout)
		return;
	
	this->Layout = lay;

}

cl_float16 CLRayCaster::ToCLMat(vtkMatrix4x4* mat)
{
	cl_float16 output;

	float VoxMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			VoxMat[4*i+j] = (float)mat->GetElement(i,j);

	memcpy(output.s,VoxMat,sizeof(float)*16);
	return output;
}

void CLRayCaster::SetRaycastVolume(vtkImageData *data, vtkMatrix4x4 *volumeMatrix)
{
	if(this->RaycastVolume == data)
		return;

	this->RaycastVolume = data;
	this->RaycastVolumeMatrix = volumeMatrix; //ToCLMat(volumeMatrix);

	RecreateTexture();
}

void CLRayCaster::UpdateSelectionBuffers(DataSelection* selection)
{
	int err = 0;

	if(selection && selection->size() > 0)
	{
		BrainLayout1D *lay = dynamic_cast<BrainLayout1D*>(selection->layout());
		std::vector<int> positions;
		if(lay)
			positions = lay->GetPositions();
		else if(lay = dynamic_cast<BrainLayout1D*>(this->Layout))
			positions = lay->GetPositions();
		else
			positions.push_back(-1);

		int *keys = selection->elements();
		float *weights = selection->weights();

		size_t element_size = 0;
		switch(selection->contentType() | (selection->structureType() << 2))
		{
		case VOXELTYPE + (UNSTRUCTUREDSELECTION << 2): 
			element_size += sizeof(cl_uint) * selection->size();
			break;
		case LINKTYPE + (UNSTRUCTUREDSELECTION << 2):
			element_size += sizeof(cl_uint) * selection->size() * 2;
			break;
		case VOXELTYPE + (RECTANGLESELECTION << 2):
			//not implemented
			break;
		case LINKTYPE + (RECTANGLESELECTION << 2):
			element_size += sizeof(cl_uint) * 4;
			break;
		default:
			break;
		}
		this->SelectionElementsBuffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,element_size,keys,&err);
		this->SelectionWeightsBuffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_float)*selection->size(),weights,&err);
		this->ElementOrderBuffer = cl::Buffer(this->context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*positions.size(),&positions[0],0);
	}
	else
	{
		int key = -1;
		float weight = 1;
		this->SelectionElementsBuffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int),&key,&err);
		this->SelectionWeightsBuffer = cl::Buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_float),&weight,&err);
		
		std::vector<int> positions;
		BrainLayout1D *lay;
		if(selection == NULL)
			lay = dynamic_cast<BrainLayout1D*>(this->Layout);
		else
			lay = dynamic_cast<BrainLayout1D*>(selection->layout());

		positions = lay->GetPositions();

		this->ElementOrderBuffer = cl::Buffer(this->context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*positions.size(),&positions[0],0);
	}
}
void CLRayCaster::RecreateTexture()
{
	int dims[3];
	this->RaycastVolume->GetDimensions(dims);

	if(this->RaycastVolumeTexture)
		delete this->RaycastVolumeTexture;

	int err;
	this->RaycastVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_INT,dims[0],dims[1],dims[2],false,(unsigned char*)this->RaycastVolume->GetScalarPointer());
}

ViewParameters CLRayCaster::CreateViewStruct(CLTexture2DObject* texture, int viewPortSize[2],int imageOrigin[2],vtkImageData *data,vtkMatrix4x4 *viewToVoxelsMatrix,double camThickness)
{
	float viewToVoxMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			viewToVoxMat[4*i+j] = (float)viewToVoxelsMatrix->GetElement(i,j);

	float voxelBounds[8];
	double *vBnds = data->GetBounds();
	for(int i = 0;i < 6;i++)
		voxelBounds[i] = (float)vBnds[i];
	voxelBounds[6] = -1;
	voxelBounds[7] = -1;

	uint imageInUseSize[2];
	imageInUseSize[0] = texture->ImageWidth();
	imageInUseSize[1] = texture->ImageHeight();

	ViewParameters params;
	size_t sz = sizeof(ViewParameters);
	memset(&params,0,sz);
	memcpy(params.imageInUseSize.s,imageInUseSize,sizeof(cl_uint2));
	memcpy(params.viewPortSize.s,viewPortSize,sizeof(cl_uint2));
	memcpy(params.imageOrigin.s,imageOrigin,sizeof(cl_uint2));
	memcpy(params.bounds.s,voxelBounds,sizeof(cl_float8));
	memcpy(params.viewToWorldMatrix.s,viewToVoxMat,sizeof(cl_float16));
	params.cameraThickness = (float)camThickness;
	
	return params;
}

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

int CLRayCaster::CastRays(vtkRenderer *renderer, CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, DataSelection* voxel_selection, std::vector<DataFilter> filters)
{
	int arg = 0, err = 0, isComparing = (int)this->IsComparing;
	SetKernel("raycastPlane");
	cl::Kernel kernel = GetKernel(activeKernel,&err); if(err) return err;

	this->UpdateSelectionBuffers(voxel_selection);
	ViewParameters params = CreateViewStruct(texture,viewPortSize,imageOrigin,RaycastVolume,viewToVoxelsMatrix,cameraThickness);
	cl::Buffer viewBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(ViewParameters),&params,&err);
	cl::Buffer boundsBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int2)*texture->TextureHeight(),rowBounds,&err);
	cl::Buffer filterBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(DataFilter)*filters.size(),&filters[0],&err);
	unsigned int matDim = Matrix->Dimension();
	unsigned int num_filters = filters.size();

	uint selection_size, selection_type, structure_type;
	if(voxel_selection)
	{
		selection_size = voxel_selection->size();
		selection_type = voxel_selection->contentType();
		structure_type = voxel_selection->structureType();
	}
	else
	{
		selection_type = NOSELECTION;
 		selection_size = 1;
		structure_type = 1;
	}
	std::vector<CLMemoryObject*> memsGL;
	memsGL.push_back(this->LutTexture);
	memsGL.push_back(texture);
	memsGL.push_back(this->MatrixBufferGPU);
	if(this->IsComparing)
		memsGL.push_back(this->ComparisonMatrixBufferGPU);
	memsGL.push_back(this->RaycastVolumeTexture);

	err |= kernel.setArg(arg++,this->MatrixBufferGPU->CLObject());

	if(this->IsComparing)
		err |= kernel.setArg(arg++,this->ComparisonMatrixBufferGPU->CLObject());
	else
		err |= kernel.setArg(arg++,this->MatrixBufferGPU->CLObject());
	
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&matDim);
	err |= kernel.setArg(arg++,this->RaycastVolumeTexture->CLObject());
	err |= kernel.setArg(arg++,viewBuffer);
	err |= kernel.setArg(arg++,boundsBuffer);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&selection_size);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&selection_type);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&structure_type);
	err |= kernel.setArg(arg++,this->SelectionElementsBuffer);
	err |= kernel.setArg(arg++,this->SelectionWeightsBuffer);
	err |= kernel.setArg(arg++,this->ElementOrderBuffer);
	err |= kernel.setArg(arg++,filterBuffer);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&num_filters);
	err |= kernel.setArg(arg++,this->LutTexture->CLObject());
	err |= kernel.setArg(arg++,texture->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_int),(void*)&isComparing);
	
	if(err) return err;

	err = StartUseMemoryObjects(memsGL);
	err = RunKernel(kernel, cl::NDRange(texture->TextureWidth(), texture->TextureHeight()), cl::NDRange(32,32));
	err = EndUseMemoryObjects(memsGL,queue);
	if(err) return err;

	err = queue.finish();
	return err;
}
