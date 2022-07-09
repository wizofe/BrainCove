#include "CLTwoPassRayCaster.h"

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

CLTwoPassRayCaster::CLTwoPassRayCaster(std::string programPath) : CLRayCaster(programPath)
{
	this->CorrelationVolume = NULL;
	this->raySetupMethod	= 0;
	this->LutTexture2		= NULL;
	this->LeftDisplacementMatrix = vtkMatrix4x4::New();
	this->RightDisplacementMatrix = vtkMatrix4x4::New();
}

CLTwoPassRayCaster::~CLTwoPassRayCaster()
{
	if(this->CorrelationVolume)
		this->CorrelationVolume->Delete();
}


void CLTwoPassRayCaster::RecreateVolumeData()
{
	if(this->CorrelationVolume)
		this->CorrelationVolume->Delete();

	this->CorrelationVolume = vtkImageData::New();
	this->CorrelationVolume->SetExtent(this->MaskVolume->GetExtent());
	this->CorrelationVolume->SetScalarTypeToFloat();
	this->CorrelationVolume->SetSpacing(this->MaskVolume->GetSpacing());
	this->CorrelationVolume->AllocateScalars();
	
	float *ptr = (float*)this->CorrelationVolume->GetScalarPointer();
	memset(ptr,0,sizeof(float)*this->CorrelationVolume->GetNumberOfPoints());

	int dims[3];
	this->CorrelationVolume->GetDimensions(dims);
	
	if(CLContext::Instance().IsDeviceNvidia())
	{
		this->IntermediateVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_FLOAT, dims[0],dims[1],dims[2],true,this->CorrelationVolume->GetScalarPointer());
		this->CorrelationVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_FLOAT, dims[0],dims[1],dims[2],false,this->CorrelationVolume->GetScalarPointer());
	}
	else
		this->CorrelationVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_FLOAT, dims[0],dims[1],dims[2],true,this->CorrelationVolume->GetScalarPointer());
}

void CLTwoPassRayCaster::SetMaskVolume(vtkImageData *data, vtkMatrix4x4 *volumeMatrix)
{
	if(!data || data == this->MaskVolume)
		return;

	int dims[3];
	this->MaskVolume = data;
	this->MaskVolume->GetDimensions(dims);

	this->MaskVolumeMatrix = volumeMatrix;

	this->MaskVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_SHORT, dims[0],dims[1],dims[2],false,data->GetScalarPointer());

	RecreateVolumeData();
}

void CLTwoPassRayCaster::SetAtlasVolume(vtkImageData *data, vtkMatrix4x4 *volumeMatrix)
{
	if(!data || data == this->AtlasVolume)
		return;

	int dims[3];
	this->AtlasVolume = data;
	this->AtlasVolume->GetDimensions(dims);

	this->AtlasVolumeMatrix = volumeMatrix;

	this->AtlasVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_UCHAR, dims[0],dims[1],dims[2],false,data->GetScalarPointer());
}

void CLTwoPassRayCaster::SetLayout(BaseLayout* layout)
{
	BrainLayout *lay = dynamic_cast<BrainLayout*>(layout);
	if(!lay)
		return;

	if(this->Layout == layout)
		return;
	
	this->Layout = lay;
}

void CLTwoPassRayCaster::RecreateTexture()
{
	int err = 0;
	int dims[3];
	this->RaycastVolume->GetDimensions(dims);

	this->RaycastVolumeTexture = CLTexture3DObject::New(context,CLMemoryObject::R_UCHAR, dims[0],dims[1],dims[2],false,(unsigned char*)this->RaycastVolume->GetScalarPointer());
}

void CLTwoPassRayCaster::SetAtlasColors(vtkLookupTable* texels)
{
	if(Lut2Texels == texels)
		return;

	Lut2Texels = texels;

	if(this->LutTexture2)
		delete this->LutTexture2;

	this->LutTexture2 = CLTexture2DObject::New(context, CLMemoryObject::RGBA_UCHAR, 256, 1, (unsigned char*)texels->GetTable()->GetPointer(0));
}

void CLTwoPassRayCaster::SetBrainSplit(float pos[4], float normal[4], int direction)
{
	for(int i = 0; i < 3; i++)
	{
		splitPanePos[i] = pos[i];
		splitPaneNormal[i] = normal[i];
	}
	splitPanePos[3] = 1;
	splitPaneNormal[3] = 1;
	splitPaneDir = direction;

	CreateDispMatrices(direction);
}

void CLTwoPassRayCaster::CreateDispMatrices(int direction)
{
	
}

bool CLTwoPassRayCaster::SelectionChanged(DataSelection *selection)
{
/*	if(voxel_selection.size()!=last_voxel_selection.size())
	{
		last_voxel_selection = voxel_selection;
		return true;
	}
	else
	{
	}
*/
	return true;
}

bool IsInRange(std::vector<int> ids, int start, int end, int val)
{
	if(start>end)
	{
		int tmp = start;
		start = end;
		end = tmp;
	}
	for(int i = start;i<=end;i++)
		if(ids[i] == val)
			return true;

	return false;
}

int CLTwoPassRayCaster::PreprocessVolumeWithVoxel(DataSelection *selection, std::vector<DataFilter> filters)
{
	bool nvidia = CLContext::Instance().IsDeviceNvidia();

	//let's not do this with cl.. it requires 2 extra gpu transfers with little speed up in calculation
	if(!selection || selection->size() == 0)
		return 0;

	std::vector<int> positions;

	BrainLayout1D *lay = selection->layout();
	if(lay)
		positions = lay->GetPositions();
	else
		positions.push_back(-1);

	int err, arg, isComparing;
	err = arg = 0;
	isComparing = (int)this->IsComparing;
	int extent[6];
	int dims[3];
	uint input_dim = this->Matrix->Dimension();
	this->CorrelationVolume->GetExtent(extent);
	this->CorrelationVolume->GetDimensions(dims);

	SetKernel("preprocessVolume");
	cl::Kernel kernel = GetKernel(activeKernel,&err); if(err) return err;

	std::vector<CLMemoryObject*> memsGL;
	memsGL.push_back(this->MatrixBufferGPU);
	memsGL.push_back(this->MaskVolumeTexture);
	if(nvidia)
		memsGL.push_back(this->IntermediateVolumeTexture);
	else
		memsGL.push_back(this->CorrelationVolumeTexture);

	this->UpdateSelectionBuffers(selection);
	uint selection_size, selection_type, structure_type;
	if(selection)
	{
		selection_size = selection->size();
		selection_type = selection->contentType();
		structure_type = selection->structureType();
	}
	else
	{
		selection_type = NOSELECTION;
		selection_size = 1;
		structure_type = 1;
	}
	cl::Buffer filterBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(DataFilter)*filters.size(),&filters[0],&err);
	unsigned int num_filters = filters.size();

	float blaat = 0;
	err = kernel.setArg(arg++,this->MatrixBufferGPU->CLObject());
	if(this->IsComparing)
		err |= kernel.setArg(arg++,this->ComparisonMatrixBufferGPU->CLObject());
	else
		err |= kernel.setArg(arg++,this->MatrixBufferGPU->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&input_dim);
	err |= kernel.setArg(arg++,this->MaskVolumeTexture->CLObject());
	if(nvidia)
		err |= kernel.setArg(arg++,this->IntermediateVolumeTexture->CLObject());
	else //amd can write to 3d images directly
		err |= kernel.setArg(arg++,this->CorrelationVolumeTexture->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&selection_size);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&selection_type);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&structure_type);
	err |= kernel.setArg(arg++,this->SelectionElementsBuffer);
	err |= kernel.setArg(arg++,this->SelectionWeightsBuffer);
	err |= kernel.setArg(arg++,this->ElementOrderBuffer);
	err |= kernel.setArg(arg++,filterBuffer);
	err |= kernel.setArg(arg++,sizeof(cl_uint),(void*)&num_filters);
	err |= kernel.setArg(arg++,sizeof(cl_int),(void*)&isComparing);

	if(err) return err;

	//set other arguments
	err = StartUseMemoryObjects(memsGL);
	int t = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[0]);
	err = RunKernel(kernel, cl::NDRange(ToFirstMultipleOf(dims[0],8),ToFirstMultipleOf(dims[1],16),ToFirstMultipleOf(dims[2],8)), cl::NDRange(8,16,8));
	err = EndUseMemoryObjects(memsGL,queue);
	if(err) return err;

	if(nvidia)
	{
		cl::size_t<3> origin;
		origin[0] = origin[1] = origin[2] = 0;
		cl::size_t<3> region;
		region[0] = dims[0];
		region[1] = dims[1];
		region[2] = dims[2];
		err = queue.enqueueCopyBufferToImage(this->IntermediateVolumeTexture->CLBuffer(),this->CorrelationVolumeTexture->CLImage(),0,origin,region,0,0);

		err = queue.finish();
	}

	return err;
}

int CLTwoPassRayCaster::ToFirstMultipleOf(int val, int multiple)
{
	int m = val;	
	while(m%multiple!=0)
		m++;	
	return m;
}

int CLTwoPassRayCaster::CastRays(vtkRenderer *renderer, CLTexture2DObject *texture, vtkMatrix4x4 *viewToVoxelsMatrix, int *imageOrigin, int *viewPortSize, double cameraThickness, int *rowBounds, DataSelection *selection, std::vector<DataFilter> filters)
{
	int err = 0, arg = 0;
	err = PreprocessVolumeWithVoxel(selection,filters); if(err) return err;

	SetKernel("raycastHighResVolume");
	cl::Kernel kernel = GetKernel(activeKernel,&err); if(err) return err;

	cl::Buffer rowBoundsBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(int)*2*texture->TextureHeight(),rowBounds,&err); if(err) return err;	

	std::vector<CLMemoryObject*> memsGL;
	memsGL.push_back(this->LutTexture);
	memsGL.push_back(this->LutTexture2);
	memsGL.push_back(this->RaycastVolumeTexture);
	memsGL.push_back(this->AtlasVolumeTexture);
	memsGL.push_back(texture);
	memsGL.push_back(this->CorrelationVolumeTexture);

	ViewParameters params = CreateViewStruct(texture,viewPortSize,imageOrigin,this->RaycastVolume,viewToVoxelsMatrix,cameraThickness);
	cl::Buffer viewBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(ViewParameters),&params,&err);

	double *aspect = renderer->GetAspect();

	float blaat = 0;
	err  = kernel.setArg(arg++,sizeof(cl_uint),(void*)&raySetupMethod);
	err |= kernel.setArg(arg++,this->RaycastVolumeTexture->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_float16),(void*)&ToCLMat(this->RaycastVolumeMatrix)); //<cl_float16>(arg++,this->RaycastVolumeMatrix);
	err |= kernel.setArg(arg++,this->CorrelationVolumeTexture->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_float16),(void*)&ToCLMat(this->MaskVolumeMatrix));
	err |= kernel.setArg(arg++,this->AtlasVolumeTexture->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_float16),(void*)&ToCLMat(this->AtlasVolumeMatrix));
	err |= kernel.setArg(arg++,viewBuffer);
	err |= kernel.setArg(arg++,rowBoundsBuffer);
	err |= kernel.setArg(arg++,this->LutTexture->CLObject());
	err |= kernel.setArg(arg++,this->LutTexture2->CLObject());
	err |= kernel.setArg(arg++,sizeof(cl_float16),(void*)&ToCLMat(LeftDisplacementMatrix));
	err |= kernel.setArg(arg++,sizeof(cl_float16),(void*)&ToCLMat(RightDisplacementMatrix));
	err |= kernel.setArg(arg++,sizeof(cl_float4),(void*)splitPanePos);
	err |= kernel.setArg(arg++,sizeof(cl_float4),(void*)splitPaneNormal);
	err |= kernel.setArg(arg++,texture->CLObject());
	//err |= kernel.setArg(arg++,debugBuffer);

	if(err) return err;

	err = StartUseMemoryObjects(memsGL);
	err = RunKernel(kernel, cl::NDRange(texture->TextureWidth(), texture->TextureHeight()), cl::NDRange(32,16));
	err = EndUseMemoryObjects(memsGL,queue);

	//err = queue.enqueueReadBuffer(debugBuffer,CL_TRUE,0,sizeof(float)*1024*1024,debugging,0,0);

	err = queue.finish();

	//delete[] debugging;

	return err;
}

void CLTwoPassRayCaster::SaveCorrelationVolume()
{
	int dims[3];
	this->CorrelationVolume->GetDimensions(dims);
	int err = queue.enqueueReadBuffer(this->IntermediateVolumeTexture->CLBuffer(),CL_TRUE,0,dims[0]*dims[1]*dims[2]*sizeof(float),this->CorrelationVolume->GetScalarPointer(),0,0);
	err = queue.finish();
	
	vtkXMLImageDataWriter *writer = vtkXMLImageDataWriter::New();
	writer->SetFileName("C:/andre_temp/Data/voxel_connectivity/corvol.vti");
	writer->SetInput(this->CorrelationVolume);
	writer->Update();
	writer->Write();
	writer->Delete();

	/*writer = vtkXMLImageDataWriter::New();
	writer->SetFileName("B:/Data/voxel_connectivity/maskvol.vti");
	writer->SetInput(this->MaskVolume);
	writer->Update();
	writer->Write();
	writer->Delete();*/
}
