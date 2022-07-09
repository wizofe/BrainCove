#ifndef SQUAREMATRIX_HEADER
#define SQUAREMATRIX_HEADER

#include "BaseMatrix.hpp"

template <typename T>
class SquareMatrix : public BaseMatrix<T>
{
public: 

	SquareMatrix<T>(size_t dim1, size_t dim2) : BaseMatrix(dim1*dim2)
	{
		dimension1 = dim1;
		dimension2 = dim2;
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
		return rawData[idxY*dimension1+idxX];
	}

	T operator() (const int idxX, const int idxY) const
	{
		return rawData[idxY*dimension1+idxX];
	}

	virtual float *CreateDownSampled()
	{
		int newdimension = dimension;
		while(newdimension > 3000)
			newdimension/=2;

		float *arrsmall = new float[newdimension*newdimension];
		
		int blocksize = dimension / newdimension;

		for(int i = 0; i < newdimension; i++)
		{
			for(int j = 0; j < newdimension; j++)
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
				arrsmall[i+newdimension*j] = corr;
			}
		}

		return arrsmall;

	}


private:
	size_t dimension1;
	size_t dimension2;
};

#endif