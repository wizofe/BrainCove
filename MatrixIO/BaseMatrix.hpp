#ifndef BASEMATRIX_HEADER
#define BASEMATRIX_HEADER

template <typename T>
class BaseMatrix
{
protected: 
	BaseMatrix<T>(size_t numElms)
	{
		numElements = numElms;
		rawData = new T[numElements];
		dimensions = NULL;
		spacing = NULL;
		bounds = NULL;
	}

public:
	//1D access
	virtual T& operator() (const int idx) = 0;
	virtual T operator() (const int idx) const = 0;

	//2D access
	virtual T& operator() (const int idxX, const int idxY) = 0;
	virtual T operator() (const int idxX, const int idxY) const = 0;
	virtual float *CreateDownSampled() = 0;

	~BaseMatrix<T>()
	{
		delete[] rawData;
	}

	T* RawData() const
	{
		return rawData;
	}

	size_t NumElements() const
	{
		return numElements;
	}

	size_t Dimension() const
	{
		return dimension;
	}

	int * GetDimensions()
	{
		if(!dimensions)
		{
			dimensions = new int[3];
			dimensions[0] = (int)Dimension();
			dimensions[1] = (int)Dimension();
			dimensions[2] = 1;
		}
		return dimensions;
	}

	void GetDimensions(int dim[3])
	{
		GetDimensions();
		for(int i = 0;i<3;i++)
			dim[i] = dimensions[i];
	}

	double * GetSpacing()
	{
		if(!spacing)
		{
			spacing = new double[3];
			spacing[0] = 1;
			spacing[1] = 1;
			spacing[2] = 1;
		}
		return spacing;
	}

	void GetSpacing(double spac[3])
	{
		GetSpacing();
		for(int i = 0;i<3;i++)
			spac[i] = spacing[i];
	}

	double * GetBounds()
	{
		if(!bounds)
		{
			GetDimensions();
			bounds = new double[6];
			bounds[0] = 0;
			bounds[1] = (double)(dimensions[0]);
			bounds[2] = 0;
			bounds[3] = (double)(dimensions[1]);
			bounds[4] = 0;
			bounds[5] = (double)(dimensions[2]);
		}
		return bounds;
	}

	void GetBounds(double bnds[6])
	{
		GetBounds();
		for(int i = 0;i<6;i++)
			bnds[i] = bounds[i];
	}

protected:
	T* rawData;
	size_t numElements;
	size_t dimension;
	int* dimensions;
	double* spacing;
	double* bounds;
};

#endif