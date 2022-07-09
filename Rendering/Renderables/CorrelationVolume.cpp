#include "CorrelationVolume.h"

CorrelationVolume::CorrelationVolume() : BaseRenderable()
{
	this->vtkMatrix = NULL;
}

// ------------------------------------------------------------------------
void CorrelationVolume::Draw() const
{
}

// ------------------------------------------------------------------------
void CorrelationVolume::SetCorrelations(const BaseMatrix<float>& matrix)
{
	this->matrix = const_cast<BaseMatrix<float>*>(&matrix);

	/*
	if(this->vtkMatrix)
		this->vtkMatrix->Delete();

	this->vtkMatrix = vtkCellArray::New();

	this->vtkMatrix->SetNumberOfCells(matrix.Dimension());

	for(int i = 0; i < matrix.Dimension(); i++)
	{
		for(int j = 0; j < matrix.Dimension(); j++)
		{
			this->vtkMatrix->InsertNextCell(
		}
	}
	*/

}