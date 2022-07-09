#include "MatrixImagePlane.h"

MatrixImagePlane::MatrixImagePlane()
{
}

MatrixImagePlane::~MatrixImagePlane()
{
}

void MatrixImagePlane::SetCorrelationMatrix(BaseMatrix<float> *matrix)
{
	CorrelationMatrix = matrix;
}

