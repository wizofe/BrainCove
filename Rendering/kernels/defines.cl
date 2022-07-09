#define STEP_MULTIPLIER 1.0f

#define PI 3.14159265358f;

#define VOXEL_UNSTRUCTURED 	(1 + (1 << 2))
#define VOXEL_RECTANGLE 	(1 + (2 << 2))
#define LINK_UNSTRUCTURED 	(2 + (1 << 2))
#define LINK_RECTANGLE 		(2 + (2 << 2))

#define NO_FILTER 0
#define CORRELATION_FILTER 1
#define DISTANCE_FILTER 2

#define FILTER_ABOVE 1
#define FILTER_BELOW 2
#define FILTER_BETWEEN 3
#define FILTER_OUTSIDE 4

__constant sampler_t colorTableSampler = CLK_FILTER_LINEAR | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;
__constant sampler_t volumeSampler = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;
//__constant sampler_t volumeSampler2 = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP;
__constant sampler_t volumeSampler2 = CLK_FILTER_LINEAR | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP;
__constant sampler_t volumeSampler3 = CLK_FILTER_LINEAR | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;

typedef struct ViewParameters
{
   float16		viewToWorldMatrix;
   float8		bounds;
   uint2		imageInUseSize;
   uint2		viewPortSize;
   uint2		imageOrigin;
   float		cameraThickness;
   float		dummy;
} ViewParameters;

typedef struct DataFilter
{
	int filterTarget;
	int filterRange;
	float rangeStart;
	float rangeEnd;	
} DataFilter;

