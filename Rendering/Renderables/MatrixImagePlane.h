#ifndef __MatrixImagePlane_h
#define __MatrixImagePlane_h

#include "../DLLDefines.h"
#include "vtkActor.h"
#include "BaseMatrix.hpp"

class GPUPlaneRayCastMapper;

class Rendering_EXPORT MatrixImagePlane : public vtkActor
{

public:
	MatrixImagePlane();
	~MatrixImagePlane();

	virtual GPUPlaneRayCastMapper* GetMapper();
	virtual void SetMapper(GPUPlaneRayCastMapper *mapper);
	
	void SetCorrelationMatrix(BaseMatrix<float> *matrix);
	
	vtkGetMacro( CorrelationMatrix, BaseMatrix<float> *);

private:
	BaseMatrix<float> *CorrelationMatrix;
	GPUPlaneRayCastMapper *Mapper;
};

#endif //__MatrixImagePlane_h