#include "PointsToVolumeData.h"

vtkStandardNewMacro(PointsToVolumeData);

PointsToVolumeData::PointsToVolumeData() : Layout(NULL)
{
	this->SetNumberOfInputPorts(0);
}

PointsToVolumeData::~PointsToVolumeData()
{
	//if(Layout)
	//	delete Layout;
}

//----------------------------------------------------------------------------
int PointsToVolumeData::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               this->WholeExtent,6);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_SHORT, 1);
  return 1;
}


void PointsToVolumeData::ExecuteData(vtkDataObject *genericData)
{
	vtkImageData *data = this->AllocateOutputData(genericData);

	data->SetScalarTypeToShort();

	data->SetExtent(this->WholeExtent);

	double spacing[3];
	Layout->GetSpacing(spacing);
	data->SetSpacing(spacing[0],spacing[1],spacing[2]);

	short* dataPtr = static_cast<short*>(data->GetScalarPointer());

	for(int i = 0;i < data->GetNumberOfPoints(); i++)
		dataPtr[i] = -1;

	bool is15k = Layout->GetNumberOfPositions() <= 15445;

	for(int i = 0; i < Layout->GetNumberOfPositions(); i++)
	{
		double pos[3];
		Layout->GetPosition(i,pos);
		double orig[3];
		Layout->GetOrigin(orig);
		int posi[3];
		for(int j = 0;j<3;j++)
			posi[j] = pos[j]-(is15k?1:0);//-orig[j];
		short *voxel = static_cast<short*>(data->GetScalarPointer(posi));
		voxel[0] = i;	
	}
}

void PointsToVolumeData::Save()
{
	this->Update();

	vtkXMLImageDataWriter *writer = vtkXMLImageDataWriter::New();
	writer->SetInput(this->GetOutput());
	writer->SetFileName("pp03_preproc_MNI_4mm.nii_masked_filtered_res.vtk");
	writer->Update();
	writer->Write();
}