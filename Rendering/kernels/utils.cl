
inline float length3(float4 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline uint get1DIndex(int col, int row, uint N)
{
	//return (uint)(idX + (idY+1)*idY/2);
	if (row <= col)
	{
		col = col - 1;
		return row*N - (row-1)*((row-1) + 1)/2 + col - row;
	}
	else //if (col<row)
	{
		row = row - 1;
		return col*N - (col-1)*((col-1) + 1)/2 + row - col;
	}
}

inline float get1DIndexValue(__global float* input, int idX, int idY, uint width)
{
	idX = min(max(idX,0),(int)width);
	idY = min(max(idY,0),(int)width);
	if(idX == idY)
		return 1.0f;
	return input[get1DIndex(idY, idX,width)];
}

inline int2 lookupMatrixIndices(__global float* input,float4 coord,uint matDim,float2 size)
{
	float2 c = (float2)(coord.x / size.x,coord.y / size.y);
	return convert_int2(c*convert_float(matDim-1));
}

inline float4 lookupColor(float correlation, __read_only 	image2d_t 	colorTable, float threshold)
{
	if(threshold < -1)
	{
		float2 colorIdx = (float2)(clamp(correlation,-1.0f,1.0f)*128.0f+128,1);
		float4 color = read_imagef(colorTable,colorTableSampler,colorIdx);
		color.w = 1.0f;
		return color;
	}
	else
	{
		float4 color = (float4)(1.0f,1.0f,1.0f,0.002f*STEP_MULTIPLIER);
		//correlation = clamp(correlation,-1.0f,1.0f);
		if(fabs(correlation) > fabs(threshold) && correlation >= -1)
		{
			float2 colorIdx = (float2)(clamp(correlation,-1.0f,1.0f)*128.0f,1);
			color = read_imagef(colorTable,colorTableSampler,colorIdx);
			color.w = fabs(correlation);
		}
		return color;
	}
}

inline float4 toNormView(int2 coord, uint2 viewPortSize, uint2 imageOrigin, int z)
{
	float offsetX = 1.0f / convert_float(viewPortSize.x);
	float offsetY = 1.0f / convert_float(viewPortSize.y);
	float x = (convert_float(coord.x) + convert_float(imageOrigin.x)) / convert_float(viewPortSize.x) * 2.0f - 1.0f + offsetX;
	float y = (convert_float(coord.y) + convert_float(imageOrigin.y)) / convert_float(viewPortSize.y) * 2.0f - 1.0f + offsetY;

	return (float4)(x,y,z,0);
}

inline float4 multiplyPoint(float4 viewCoord, const float16 matrix)
{
	float4 worldCoord;
	worldCoord.x = viewCoord.x * matrix.s0 + viewCoord.y * matrix.s1 + viewCoord.z * matrix.s2 + matrix.s3;
	worldCoord.y = viewCoord.x * matrix.s4 + viewCoord.y * matrix.s5 + viewCoord.z * matrix.s6 + matrix.s7;
	worldCoord.z = viewCoord.x * matrix.s8 + viewCoord.y * matrix.s9 + viewCoord.z * matrix.sa + matrix.sb;
	worldCoord.w = viewCoord.x * matrix.sc + viewCoord.y * matrix.sd + viewCoord.z * matrix.se + matrix.sf;	
	if(worldCoord.w != 1.0f)
	{
		worldCoord.x /= worldCoord.w;
		worldCoord.y /= worldCoord.w;
		worldCoord.z /= worldCoord.w;
	}
	return worldCoord;
}

inline bool isElementInRange(__global int *elementReorder, const uint low_idx, const uint high_idx, int element)
{
	for(uint i = low_idx; i <= high_idx; i++)
	{
		int list_element = elementReorder[i];
		if(list_element == element)
			return 1;
	}
	return 0;
}

inline bool isVoxelInBounds(float4 rvVoxel,int4 volumeBounds)
{
	return rvVoxel.x < volumeBounds.s0 &&
		   rvVoxel.y < volumeBounds.s1 &&
		   rvVoxel.z < volumeBounds.s2;
}
