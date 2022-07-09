#ifndef UPPERTRIANGULARMATRIX_HEADER
#define UPPERTRIANGULARMATRIX_HEADER

#include "BaseMatrix.hpp"

template <typename T>
class UpperTriangularMatrix : public BaseMatrix<T>
{
public: 

	UpperTriangularMatrix<T>(size_t dim1) : BaseMatrix((size_t)(dim1*(dim1-1)*0.5))
	{
		dimension = dim1;
	}

	T& operator() (const int idx)
	{
		return rawData[idx];
	}

	T operator() (const int idx) const
	{
		return rawData[idx];
	}

	T& operator() (const int idxX, const int idxY)
	{
		if(idxX == idxY)
		{
			float f = 1.0f;
			return (T)f;
		}
		return rawData[Get1DIndex(idxX,idxY)];
	}

	T operator() (const int idxX, const int idxY) const
	{
		if(idxX == idxY)
			return (T)1.0f;
		return rawData[Get1DIndex(idxX,idxY)];
	}

	virtual float *CreateDownSampled()
	{
		int newdimension = dimension;
		while(newdimension > 3000)
			newdimension/=2;

		float *arrsmall = new float[newdimension*(newdimension-1)*0.5];
		
		int blocksize = dimension / newdimension;

		for(int i = 0; i < newdimension; i++)
		{
			for(int j = i+1; j < newdimension; j++)
			{
				float corr = 0;
				for(int k = 0; k < blocksize; k++)
				{
					for(int l = 0; l < blocksize; l++)
					{
						float current = (*this)(i*blocksize+k,j*blocksize+l);
						if(abs(current) > abs(corr))
							corr = current;
					}
				}
				int N = newdimension-1;
				size_t indx = (size_t)(i*N - (i-1)*((i-1) + 1)/2 + j- 1 - i);
				arrsmall[indx] = corr;
			}
		}

		return arrsmall;

	}

private:
	/*unsigned int row_index( unsigned int i, unsigned int M ){
		double m = M;
		double row = (-2*m - 1 + sqrt( (4*m*(m+1) - 8*(double)i - 7) )) / -2;
		if( row == (double)(int) row ) row -= 1;
		return (unsigned int) row;
	}


	unsigned int column_index( unsigned int i, unsigned int M ){
		unsigned int row = row_index( i, M);
		return  i - M * row + row*(row+1) / 2;
	}*/

	size_t Get1DIndex(const int col, const int row) const
	{
		int N = Dimension()-1;
		//int col = std::min(std::max(idX,0),N);
		//int row = std::min(std::max(idY,0),N);
		size_t indx = 0;
		if (row <= col)
			indx = (size_t)(row*N - (row-1)*((row-1) + 1)/2 + col - 1 - row);
		else// if (col<row)
			indx = (size_t)(col*N - (col-1)*((col-1) + 1)/2 + row - 1 - col);
		return indx;
	}

	/*size_t Get1DIndex(const int idxX, const int idxY) const
	{
		return (size_t)(idxX-idxY + idxY*(Dimension()-1)-idxY*(idxX-1)/2.0);//(col * (col - 3) / 2 + row);//

	}*/

};

#endif