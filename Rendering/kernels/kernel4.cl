#include "defines.cl"
#include "utils.cl"
#include "raysetup.cl"
#include "images.cl"
#include "compositing.cl"              
                                                                     
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
 
__constant char mexicanHat[9][9] = { 	{ 0, 0, 0,-1,-1,-1, 0, 0, 0},
										{ 0,-1,-1,-3,-3,-3,-1,-1, 0},
										{ 0,-1,-3,-3,-1,-3,-3,-1, 0},
										{-1,-3,-3, 6,13, 6,-3,-3,-1},
										{-1,-3,-1,13,24,13,-1,-3,-1},
										{-1,-3,-3, 6,13, 6,-3,-3,-1},
										{ 0,-1,-3,-3,-1,-3,-3,-1, 0},
										{ 0,-1,-1,-3,-3,-3,-1,-1, 0},
										{ 0, 0, 0,-1,-1,-1, 0, 0, 0} };
				    			     			
__inline void SplatPixel(__write_only image2d_t splatput, int2 coord, int2 size)
{
	for(int i = -4; i < 5; i++)
	{
		if(coord.x+i < 0) continue;
		if(coord.x+i >= size.x) continue;
		for(int j = -4; j < 5; j++)
		{
			if(coord.y+j < 0) continue;
			if(coord.y+j >= size.y) continue;
			//float4 value = (float4)(1.0f+(convert_float(mexicanHat[i][j])/24.0f),1.0f+(convert_float(mexicanHat[i][j])/24.0f),1.0f+(convert_float(mexicanHat[i][j])/24.0f),1.0f+(convert_float(mexicanHat[i][j])/24.0f));
 			float4 value = (float4)(1,1,1,1);
			write_imagef(splatput,(int2)(coord.x+i,coord.y+j),value);
		}
	}
}                                
 
inline void ApplyFilters(__constant DataFilter *filters, uint num_filters, float *correlation)
{   
	//if(fabs(correlation[0]) == 0)
	//	correlation[0] = 0.001f;
	for(int i = 0; i < num_filters; i++)
	{
		DataFilter filter = filters[i];
		switch(filter.filterTarget)
		{
			case NO_FILTER:
				break;
			case CORRELATION_FILTER:
				switch(filter.filterRange)
				{
					case FILTER_ABOVE:
						if(fabs(correlation[0]) < filter.rangeStart)
							correlation[0] = 0;
						break;
					case FILTER_BELOW:
						if(correlation[0] > filter.rangeStart)
							correlation[0] = 0;
						break;
					case FILTER_BETWEEN:
						if(correlation[0] < filter.rangeStart ||
							correlation[0] > filter.rangeEnd)
							correlation[0] = 0;
						break;
					case FILTER_OUTSIDE:
						if(correlation[0] > filter.rangeStart &&
							correlation[0] < filter.rangeEnd)
							correlation[0] = 0;
						break;
				}
				break;
			case DISTANCE_FILTER:
				correlation[0] = 0.0f;
				break;
		} 
	}
}          

__kernel void preprocessVolume(	__global 	float		*corrMatrix,
								__global 	float		*corrMatrix2,
								const uint				input_dim ,
								__read_only image3d_t 	maskVolume,
								IMAGECLASS_OUTPUT		correlationVolume,
								const uint				selection_size,
								const uint				selection_type,
								const uint				structure_type,
								__global int			*voxel_ids,
								__global float			*voxel_weights,
								__global int			*elementReorder,
								__constant DataFilter	*filters,
								const uint				num_filters,
								const int				is_comparing)
{
	int4 coord = (int4)(get_global_id(0),get_global_id(1),get_global_id(2),0);   
	int4 size = get_image_dim(maskVolume);

	if(coord.x >= size.x || coord.y >= size.y || coord.z >= size.z)
		return;
	
	int4 voxel = read_imagei(maskVolume,volumeSampler,coord);
	float final_correlation = 0.0f;
	if(voxel.x >= 0) 
	{
		switch(selection_type | (structure_type << 2))
		{
			case VOXEL_UNSTRUCTURED: //voxel + unstructured
				for(int voxel_idx = 0; voxel_idx < selection_size; voxel_idx++)
				{
					int selectedVoxel = voxel_ids[voxel_idx];
					if(selectedVoxel == -1) continue;
					float correlation = get1DIndexValue(corrMatrix,voxel.x,selectedVoxel,input_dim-1);
					if(is_comparing)
					{
						correlation -= get1DIndexValue(corrMatrix2,voxel.x,selectedVoxel,input_dim-1);
						correlation /= 2.0f;
					}
					if(fabs(correlation) > fabs(final_correlation))
						final_correlation = correlation;
				}  
				break;
			case VOXEL_RECTANGLE: //voxel + rectangle
				//NOT IMPLEMENTED
				break;
			case LINK_UNSTRUCTURED: //link + unstructured
				for(int voxel_idx = 0; voxel_idx < selection_size*2; voxel_idx++) 
				{
					int selectedVoxel = voxel_ids[voxel_idx];
					if(selectedVoxel == voxel.x) 
						final_correlation = 100;
				}
				break;
			case LINK_RECTANGLE: //link + rectangle
				if(isElementInRange(elementReorder, voxel_ids[0], voxel_ids[1], voxel.x))
					final_correlation = 100;
				else if(isElementInRange(elementReorder, voxel_ids[2], voxel_ids[3], voxel.x))
					final_correlation = -100;
				break;
		}
		ApplyFilters(filters,num_filters,&final_correlation);
		writeImagef(correlationVolume,coord,size,final_correlation);
	}
	else
		writeImagef(correlationVolume,coord,size,0.0f);

}

     
__kernel void raycastHighResVolume (const uint 					raySetupMethod,
									__read_only image3d_t 		raycastVolume,
									const float16				raycastVolumeMatrix,
									__read_only image3d_t 		correlationVolume,
									const float16				correlationVolumeMatrix,
									__read_only image3d_t 		atlasVolume,
									const float16				atlasVolumeMatrix,
									__constant ViewParameters 	*viewParams,
									__global int				*rowBounds,
									__read_only image2d_t 		colorTable,
									__read_only image2d_t 		colorTable2,
									const float16 				leftLobeDisplaceMatrix,
									const float16 				rightLobeDisplaceMatrix,
									const float4 				splitPlanePos,
									const float4 				splitPlaneNormal,
									//__read_only image2d_t 		depthMap,
									__write_only image2d_t 		output)
									//const float					angleDegrees)
									//__global float				*debugging)
{
    int2 coord = (int2)(get_global_id(0),get_global_id(1)); 
	 
	if(coord.x > viewParams->imageInUseSize.s0 || coord.y > viewParams->imageInUseSize.s1)
	{
		//we are out of bounds
		write_imagef(output,coord,(float4)(1,0,0,1));
		return;
	}
	
	float4 resultColor = (float4)(0,0,0,0);
	float4 contextColor = (float4)(0,0,0,0);
	
	int4 raycastVolumeBounds = get_image_dim(raycastVolume);
	int4 correlationVolumeBounds = get_image_dim(correlationVolume);
	int4 atlasVolumeBounds = get_image_dim(atlasVolume);
	
	float4 brainCenter = splitPlanePos;//(float4)(180.5f/2.0f,30,152.5f/2.0f,1);
	         
	float4 planeNormal1 = splitPlaneNormal;//(float4)(-1,0,0,0);//
	planeNormal1 = fast_normalize(planeNormal1);
	     
	float4 rayStart, rayEnd, rayStep, lightPos;
	uint numsteps; 
	         
	setupRay(coord,viewParams,raySetupMethod,&rayStart,&rayEnd,&rayStep,&numsteps,&lightPos);
	     
	float4 ambientColor = (float4)(1.0f,1.0f,1.0f,1.0f);
	float4 diffuseColor = (float4)(1.0f,1.0f,1.0f,1.0f);
	float4 specularColor = (float4)(1.0f,1.0f,1.0f,1.0f);
  
	float4 voxel = rayStart.xyzw;
 
	float remainingOpacity = 1.0f;     
	int lobe = 0;
                       
	for(int loop = 0; loop < numsteps && remainingOpacity > 0.02f; loop++)
	{
		float4 lvoxel = voxel.xyzw;
		lvoxel = lvoxel - brainCenter;
		if(dot(planeNormal1,fast_normalize(lvoxel)) < 0)
		{	
			lobe = -1; //left
			lvoxel = multiplyPoint(lvoxel, leftLobeDisplaceMatrix); 
		}
		else  
		{
			lobe = 1; //right
			lvoxel = multiplyPoint(lvoxel, rightLobeDisplaceMatrix);
		}
		lvoxel = lvoxel + brainCenter;

		if(dot(planeNormal1,fast_normalize(lvoxel-brainCenter)) > 0 && lobe == -1)
		{
			//resultColor = (float4)(0.1f,0,0,0.0001f);
			voxel += rayStep;
			continue;
		}
		if(dot(planeNormal1,fast_normalize(brainCenter-lvoxel)) > 0 && lobe == 1)
		{
			//resultColor = (float4)(0,0.1f,0,0.001f);
			voxel += rayStep;
			continue;
		}
		 
		float4 rvVoxel = multiplyPoint(lvoxel,raycastVolumeMatrix);
		    
		if(isVoxelInBounds(rvVoxel,raycastVolumeBounds))// && isVoxelInBounds(cvVoxel,correlationVolumeBounds) && isVoxelInBounds(avVoxel,atlasVolumeBounds))
		{
			float4 cvVoxel = multiplyPoint(lvoxel,correlationVolumeMatrix);
			float4 avVoxel = multiplyPoint(lvoxel,atlasVolumeMatrix);
		
			uint4 pixeldata = read_imageui(raycastVolume,volumeSampler2,rvVoxel);
			float4 correlationdata = read_imagef(correlationVolume,volumeSampler2,cvVoxel);
			uint4 atlasdata = read_imageui(atlasVolume,volumeSampler2,avVoxel);
			uint4 atlasdata_nn = read_imageui(atlasVolume,volumeSampler,avVoxel);
             
			switch(raySetupMethod)           
			{ 
				case 0: 
					resultColor = RayCompositeFunction3D(raycastVolume,atlasVolume,viewParams->bounds,voxel,rvVoxel,cvVoxel,avVoxel,lightPos,rayStart,ambientColor,diffuseColor,specularColor,resultColor,pixeldata,correlationdata,atlasdata,atlasdata_nn,colorTable,colorTable2,&remainingOpacity,loop,numsteps,&contextColor);
					break;            
				case 1: 
					resultColor = RayCompositeFunctionCortex(raycastVolume,atlasVolume,viewParams->bounds,voxel,rvVoxel,cvVoxel,avVoxel,lightPos,rayStart,ambientColor,diffuseColor,specularColor,resultColor,pixeldata,correlationdata,atlasdata,atlasdata_nn,colorTable,colorTable2,&remainingOpacity,loop,numsteps); 
					break;
			}
		}    
		voxel += rayStep;
	}       
 	
	if(remainingOpacity < 0.005f)                     
		remainingOpacity = 0;

	if(raySetupMethod == 0)
	{
		resultColor += contextColor * contextColor.w * remainingOpacity;
		resultColor.w = (1 - (remainingOpacity - contextColor.w));
	}
	else
		resultColor.w = (1 - remainingOpacity);

	
	write_imagef(output,coord,resultColor);
} 

// ============================================================================
// 								MATRIX RAYCASTER 2D
// ============================================================================
// Shoot a ray for each output pixel which computes its color by sampling through the 
// 3D volume and accumulating color
__kernel void raycastPlane(	__global float				*input,
							__global float				*input2,
							const uint					input_dim,
							__read_only image3d_t 		raycastVolume,
							__constant ViewParameters 	*viewParams,
							__global int				*rowBounds,
							const uint					selection_size,
							const uint					selection_type,
							const uint					structure_type,
							__global int				*voxel_ids,
							__global float				*voxel_weights,
							__global int				*elementReorder,
							__constant DataFilter		*filters,
							const uint					num_filters,
							__read_only image2d_t 		colorTable,
							__write_only image2d_t 		output,
							const int					is_comparing
							)
{
    int2 coord = (int2)(get_global_id(0),get_global_id(1));
	int2 imbounds = get_image_dim(output);
	
	int4 size4 = get_image_dim(raycastVolume);
	float2 size2 = (float2)(convert_float(size4.x), convert_float(size4.y));
	
	if(coord.x < rowBounds[coord.y*2] || coord.x > rowBounds[coord.y*2+1])
	{
		//we are out of bounds
		write_imagef(output,coord,(float4)(0,0,0,0));
		return;
	}
	
	float stepMultiplier = 1.0f;

	float4 planeCoord = (float4)(0,0,0,1);
	float4 rayStart = multiplyPoint(planeCoord,viewParams->viewToWorldMatrix);
	
	planeCoord = (float4)(0,0,1,1);
	float4 rayEnd = multiplyPoint(planeCoord,viewParams->viewToWorldMatrix);
	float4 rayCenter = rayEnd - rayStart;
	
	rayCenter /= viewParams->cameraThickness;
	
	float centerScale = length3(rayCenter);
	
	rayCenter /= centerScale;
	
	float4 viewCoord4 = toNormView(coord,viewParams->viewPortSize,viewParams->imageOrigin,-0.5f);
	float4 voxel1 = multiplyPoint(viewCoord4,viewParams->viewToWorldMatrix);

	//use this to calculate the 'world size' of a (square) pixel
	//ideally, we would take multiple samples for the complete range of the pixel size
	//unfortunately, this is too heavy, so we're introducing aliasing. 
	//to prevent aliasing during voxel selection, we 
	int2 coord2 = (int2)(coord.x+1,coord.y+1);
	viewCoord4 = toNormView(coord2,viewParams->viewPortSize,viewParams->imageOrigin,-0.5f);
	float4 voxel2 = multiplyPoint(viewCoord4,viewParams->viewToWorldMatrix);
	
	float4 color = (float4)(0.0f,0.0f,0.0f,0.0f);
	
	if(!(voxel1.x < viewParams->bounds.s0 || voxel1.x > viewParams->bounds.s1 || voxel1.y < viewParams->bounds.s2 || voxel1.y > viewParams->bounds.s3))
	{
		int2 idcs1 = lookupMatrixIndices(input,voxel1,input_dim,size2);
		int2 idcs2 = lookupMatrixIndices(input,voxel2,input_dim,size2);
		/*float correlation = 0;
		int count = 0;
		for(int i = idcs1.x;i <= idcs2.x;i++)
		{
			for(int j = idcs1.y;j <= idcs2.y;j++)
			{
				float current = get1DIndexValue(input,elementReorder[i],elementReorder[j],input_dim-1);

				if(fabs(current) > fabs(correlation))
					correlation = current;
				//correlation += current;
				//count++;
			}
		}*/
		//correlation/=convert_float(count);
		float correlation = get1DIndexValue(input,elementReorder[idcs1.x],elementReorder[idcs1.y],input_dim-1);
		if(is_comparing)
		{
			correlation -= get1DIndexValue(input2,elementReorder[idcs1.x],elementReorder[idcs1.y],input_dim-1);
			correlation /= 2.0f; //scale back to (-1, 1)
		}

		if(correlation == 0.0f)
			correlation = 1.0f;
		//ApplyFilters(filters,num_filters,&correlation);
		color = lookupColor(correlation,colorTable,-1024.0f);
		if(fabs(correlation)>0.001f /* && voxel1.x <= voxel1.y */)
			color.w = 0.5f;
		else
		{
			//correlation = 0.0f;
			color.w = 1.0f;
		} 
 
		switch(structure_type)
		{
			case 1: //unstructured
				switch(selection_type)
				{
					case 1: //voxel type
						for(int i = 0; i < selection_size; i++)
						{
							if(isElementInRange(elementReorder,idcs1.x,idcs2.x,voxel_ids[i]) || isElementInRange(elementReorder,idcs1.y,idcs2.y,voxel_ids[i]))
								color.w = 1.0f;
						}
						break;
					case 2: //link type
						for(int i = 0; i < selection_size*2; i+=2)
							if((elementReorder[idcs1.x] == voxel_ids[i] && elementReorder[idcs1.y] == voxel_ids[i+1]) || 
								(elementReorder[idcs1.x] == voxel_ids[i+1] && elementReorder[idcs1.y] == voxel_ids[i]))
								color.w = 1.0f;
								break;
					default: //no selection
						break;
				}
				break;
			case 2: //rectangle
				switch(selection_type)
				{   //only link type is supported
					case 2: //link type
						if(selection_size < 2)
							break;     
						if(((idcs1.x >= voxel_ids[0] && idcs1.x <= voxel_ids[1]) && (idcs1.y >= voxel_ids[2] && idcs1.y <= voxel_ids[3]))
							|| ((idcs1.y >= voxel_ids[0] && idcs1.y <= voxel_ids[1]) && (idcs1.x >= voxel_ids[2] && idcs1.x <= voxel_ids[3])))
								color.w = 1.0f;
						break;
					default: //no selection
						break;
				}
				break;
			default:
				break;
			
		}
	}      
	
	//if(color.w == 1.0f)
	//	SplatPixel(splatput,coord,imbounds);
	
	//float4 splatpixel = read_imagef(splatput,volumeSampler,coord);
	
	color *= color.w;// * splatpixel.x;
	
	write_imagef(output,coord,color);
	
	//write_imagef(output,(int2)(rowBounds[coord.y*2+1]-coord.x-1,coord.y),color);
}                                                                              
