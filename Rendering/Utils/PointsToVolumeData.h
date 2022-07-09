#ifndef POINTSTOVOLUMEDATA_HEADER
#define POINTSTOVOLUMEDATA_HEADER

#include "../DLLDefines.h"
#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <Layouts/BaseLayout.h>
/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT PointsToVolumeData : public vtkImageAlgorithm
{
public:
	static PointsToVolumeData *New();
	vtkTypeMacro(PointsToVolumeData,vtkImageAlgorithm);


	// Description:
  // Set/Get the Data mask.
	vtkGetMacro(Layout,BaseLayout*);

	void Save();

	void SetLayout(BaseLayout* lay)
    {
	  double dims[3];
	  lay->GetDimensions(dims);
	  SetLayout(lay,dims);
    }

	void SetLayout(BaseLayout* lay, double dims[3])
    {
      if (lay == this->Layout)
        {
        return;
        }
      this->Layout = lay;
	  /*this->WholeExtent[0] = this->WholeExtent[2] = this->WholeExtent[4] = 0;
	  this->WholeExtent[1] = (int)dims[0];
	  this->WholeExtent[3] = (int)dims[1];
	  this->WholeExtent[5] = (int)dims[2];
	  */
	  double bounds[6];
	  this->Layout->GetBounds(bounds);
	  this->WholeExtent[0] = 0;//(int)bounds[0];
	  this->WholeExtent[2] = 0;//(int)bounds[2];
	  this->WholeExtent[4] = 0;//(int)bounds[4];
	  this->WholeExtent[1] = (int)bounds[1];
	  this->WholeExtent[3] = (int)bounds[3];
	  this->WholeExtent[5] = (int)bounds[5];
	  
      this->Modified();
    }

protected: 
	PointsToVolumeData();
	~PointsToVolumeData();

	virtual int RequestInformation (vtkInformation *, vtkInformationVector**, vtkInformationVector *);
	void ExecuteData(vtkDataObject *data);

	BaseLayout *Layout;
	int WholeExtent[6];

private:
	PointsToVolumeData(const PointsToVolumeData&);	// Not implemented.
	void operator=(const PointsToVolumeData&);		// Not implemented.
};

#endif //POINTSTOVOLUMEDATA_HEADER