#ifndef GPUPLANERAYCASTMAPPER_HEADER
#define GPUPLANERAYCASTMAPPER_HEADER

#include "../DLLDefines.h"
#include <vtkObjectFactory.h>
#include "GPUGenericRayCastMapper.h"
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkImageData.h>
#include <vtkPlaneCollection.h>
#include <vtkFastNumericConversion.h>
#include <vtkInformation.h>
#include <BaseMatrix.hpp>
#include <CLWrappers/CLContext.h>
#include <CLWrappers/CLTexture2DObject.h>
#include "CLRayCaster.h"
#include <Utils/CLTexturePainter.h>
#include <Layouts/BrainLayout1D.h>
#include <Utils/PointsToVolumeData.h>
#include <Layouts/BaseVoxelSelector.h>
#include <QTime>

class Rendering_EXPORT GPUPlaneRayCastMapper : public GPUGenericRayCastMapper
{
public:
  static GPUPlaneRayCastMapper *New();
  vtkTypeMacro(GPUPlaneRayCastMapper,vtkVolumeMapper);

  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void SetConnectivityMatrix( BaseMatrix<float> * );
  BaseMatrix<float> *GetConnectivityMatrix();
  //virtual void SetInput( vtkDataSet * );

  virtual void SetLayout(BaseLayout *layout) {}
  virtual void SetLayout(BrainLayout1D *layout);
  virtual BaseLayout* GetLayout() const { return this->Layout; }

  virtual void SetMouseCoordinates(int x, int y);
  vtkGetMacro(MouseCoordinates, int*);

  virtual void SetCorrelation(double corr);
  vtkGetMacro(Correlation, double);

  // Description:
  // Sampling distance in the XY image dimensions. Default value of 1 meaning
  // 1 ray cast per pixel. If set to 0.5, 4 rays will be cast per pixel. If
  // set to 2.0, 1 ray will be cast for every 4 (2 by 2) pixels.
  vtkSetClampMacro( ImageSampleDistance, double, 0.1, 100.0 );
  vtkGetMacro( ImageSampleDistance, double );

  // Description:
  // This is the minimum image sample distance allow when the image
  // sample distance is being automatically adjusted
  vtkSetClampMacro( MinimumImageSampleDistance, double, 0.1, 100.0 );
  vtkGetMacro( MinimumImageSampleDistance, double );

  // Description:
  // This is the maximum image sample distance allow when the image
  // sample distance is being automatically adjusted
  vtkSetClampMacro( MaximumImageSampleDistance, double, 0.1, 100.0 );
  vtkGetMacro( MaximumImageSampleDistance, double );

  // Description:
  // This is the actual program that will run to draw pixels on the texture
  void SetRayCaster(CLRayCaster* rayCaster);
  vtkGetMacro(RayCaster, CLRayCaster*);

  // Description:
  // Return bounding box (array of six doubles) of data expressed as
  // (xmin,xmax, ymin,ymax, zmin,zmax).
  virtual double *GetBounds();
  virtual void GetBounds(double bounds[6]) 
    {this->vtkAbstractMapper3D::GetBounds(bounds);};

  // Description:
  // Computes the bounding box (array of six doubles) of data expressed as
  // (xmin,xmax, ymin,ymax, zmin,zmax).
  void ComputeBounds();

  int ComputeRowBounds(vtkVolume   *vol, vtkRenderer *ren);

  void Render( vtkRenderer *, vtkVolume * );
  void ReleaseGraphicsResources(vtkWindow *);

protected:
  GPUPlaneRayCastMapper();
  ~GPUPlaneRayCastMapper();

  vtkMatrix4x4 *PerspectiveMatrix;
  vtkMatrix4x4 *ViewToWorldMatrix;
  vtkMatrix4x4 *ViewToVoxelsMatrix;
  vtkMatrix4x4 *VoxelsToViewMatrix;
  vtkMatrix4x4 *WorldToVoxelsMatrix;
  vtkMatrix4x4 *VoxelsToWorldMatrix;

  vtkMatrix4x4 *VolumeMatrix;
  
  vtkTransform *PerspectiveTransform;
  vtkTransform *VoxelsTransform;
  vtkTransform *VoxelsToViewTransform;

  double                       ImageSampleDistance;
  double                       MinimumImageSampleDistance;
  double                       MaximumImageSampleDistance;
  double					   MinimumViewDistance;

  // This is how big the image would be if it covered the entire viewport
  int            ImageViewportSize[2];
  
  // This is how big the allocated memory for image is. This may be bigger
  // or smaller than ImageFullSize - it will be bigger if necessary to 
  // ensure a power of 2, it will be smaller if the volume only covers a
  // small region of the viewport
  int            ImageMemorySize[2];
  
  // This is the size of subregion in ImageSize image that we are using for
  // the current image. Since ImageSize is a power of 2, there is likely
  // wasted space in it. This number will be used for things such as clearing
  // the image if necessary.
  int            ImageInUseSize[2];
  
  // This is the location in ImageFullSize image where our ImageSize image
  // is located.
  int            ImageOrigin[2];

  int  *RowBounds;
  int  *OldRowBounds;

  float        *ZBuffer;
  int           ZBufferSize[2];
  int           ZBufferOrigin[2];

  double GetZBufferValue(int x, int y);

  void ComputeMatrices( vtkImageData *data, vtkVolume *act );

  void ComputeVolume();

  int PickVolume(int x, int y, vtkRenderer *render);

  void NormalizeCoordinates(int x, int y, float z, float w, int *viewPortSize, float outCoord[4]);

  int MouseCoordinates[2];

  bool NewVoxelPicked;

  int VoxelPicked;

  double Correlation;
  //virtual int FillInputPortInformation(int, vtkInformation*);


private:
  GPUPlaneRayCastMapper(const GPUPlaneRayCastMapper&);  // Not implemented.
  void operator=(const GPUPlaneRayCastMapper&);  // Not implemented.

  BaseMatrix<float> *inputMatrix;

  vtkImageData *ImageData;
  //PointsToVolumeData *PointToVolume;

  CLTexture2DObject *Texture;
  CLRayCaster *RayCaster;

  CLTexturePainter *Painter;

  BaseLayout *Layout;
};

#endif //GPUPLANERAYCASTMAPPER_HEADER