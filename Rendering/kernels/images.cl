
#ifndef cl_khr_3d_image_writes
#define IMAGECLASS_OUTPUT __global float* 
#define IMAGECLASS_INPUT __global float* 
#else
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable
#define IMAGECLASS_OUTPUT __write_only image3d_t
#define IMAGECLASS_INPUT __read_only image3d_t
#endif


inline void writeImagei(IMAGECLASS_OUTPUT image, int4 coord, int4 dimensions, int value)
{
#ifndef cl_khr_3d_image_writes
	image[coord.x*dimensions.x*dimensions.y+coord.y*dimensions.y+coord.z] = value;
#else
	write_imagei(image,coord,(int4)(value,value,value,value));
#endif
}

inline void writeImagef(IMAGECLASS_OUTPUT image, int4 coord, int4 dimensions, float value)
{
#ifndef cl_khr_3d_image_writes
	image[coord.x*1+coord.y*dimensions.x*1+coord.z*dimensions.x*dimensions.y*1] = value;
#else
	write_imagef(image,coord,(float4)(value,value,value,value));
#endif
}

inline int4 readImagei(IMAGECLASS_INPUT image, int4 coord, uint4 dimensions, sampler_t sampler)
{
#ifndef cl_khr_3d_image_writes
	return image[coord.x*dimensions.x*dimensions.y+coord.y*dimensions.y+coord.z];
#else
	return read_imagei(image,sampler,coord);
#endif
}

inline float4 readImagef(IMAGECLASS_INPUT image, int4 coord, uint4 dimensions, sampler_t sampler)
{
#ifndef cl_khr_3d_image_writes
	return image[coord.x*dimensions.x*dimensions.y+coord.y*dimensions.y+coord.z];
#else
	return read_imagef(image,sampler,coord);
#endif
}
