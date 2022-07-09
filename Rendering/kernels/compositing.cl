__constant float colors[21] = {127, 201, 127,    190, 174, 212,     253, 192, 134,     255, 255, 153,     56, 108, 176,     240, 2, 127,     191, 91, 23 };

inline float4 CalculateGradient(__read_only  image3d_t imagedata,float4 voxel,float8 bounds,float *mult)
{
	float size = 1;//1.75f;
	float4 minX = (float4)(voxel.x-size,voxel.y,voxel.z,voxel.w);
	float4 maxX = (float4)(voxel.x+size,voxel.y,voxel.z,voxel.w);
	float4 minY = (float4)(voxel.x,voxel.y-size,voxel.z,voxel.w);
	float4 maxY = (float4)(voxel.x,voxel.y+size,voxel.z,voxel.w);
	float4 minZ = (float4)(voxel.x,voxel.y,voxel.z-size,voxel.w);
	float4 maxZ = (float4)(voxel.x,voxel.y,voxel.z+size,voxel.w);
	float x1 = (float)read_imageui(imagedata,volumeSampler,minX).x;
	float x2 = (float)read_imageui(imagedata,volumeSampler,maxX).x;
	float y1 = (float)read_imageui(imagedata,volumeSampler,minY).x;
	float y2 = (float)read_imageui(imagedata,volumeSampler,maxY).x;
	float z1 = (float)read_imageui(imagedata,volumeSampler,minZ).x;
	float z2 = (float)read_imageui(imagedata,volumeSampler,maxZ).x;

	float mag = fast_length((float3)(x2-x1,y2-y1,z2-z1));
	float4 gradient = (float4)((x2-x1),(y2-y1),(z2-z1),1);
	float4 gradientn = fast_normalize(gradient);
	//return (float8)(gradient.x,gradient.y,gradient.z,gradient.w,mag,0,0,0);
	mult[0] = mag;
	return gradientn;
}

inline float4 GetColor(float value)
{
	float mmval = min(max(value,0.0f),1.0f);
	int indx = (int)floor(mmval*6.0f);
	return (float4)(colors[indx*3+0]/256.0f,colors[indx*3+1]/256.0f,colors[indx*3+2]/256.0f,1);
}

inline float4 RayCompositeFunction3D(__read_only image3d_t raycastVolume, __read_only image3d_t atlasVolume, float8 bounds, float4 voxel, float4 rvVoxel, float4 cvVoxel, float4 avVoxel,float4 lightPos, float4 rayStart, float4 ambientColor, float4 diffuseColor, float4 specularColor, float4 currentColor,uint4 pixeldata,float4 correlationdata,uint4 atlasdata,uint4 atlasdata_nn,__read_only image2d_t colorTable,__read_only image2d_t colorTable2, float* remainingOpacity, int step, int num_steps, float4 *contextColor)
{
	float4 objectColor = (float4)(0);
	float4 tempColor = (float4)(0);
	
	if(correlationdata.x >= 99.0f)
		objectColor = (float4)(0,0,1,0.2f);
	else if(correlationdata.x < -1.0f)
		objectColor = (float4)(1,0,0,0.2f);
	else if(pixeldata.x)
	{
		float gradmag;
		float4 Nr = CalculateGradient(raycastVolume,rvVoxel,bounds,&gradmag);
		float4 L = fast_normalize(lightPos - voxel);
		float4 V = fast_normalize(rayStart - voxel);
		float NdotV = fabs(dot(Nr, V));
		float4 myColor = (float4)(pixeldata.x/256.0f,pixeldata.x/256.0f,pixeldata.x/256.0f,0.03f);
		contextColor[0] += myColor * myColor.w * (1-contextColor[0].w);

		if(correlationdata.x >= -1 && atlasdata.x>=1)
		{
			float gradmag;
			float4 Nr = CalculateGradient(raycastVolume,rvVoxel,bounds,&gradmag);
			float4 L = fast_normalize(lightPos - voxel);
			float4 V = fast_normalize(rayStart - voxel);
			float4 H = fast_normalize(V+L);
			float NdotL = max(dot(Nr, L), 0.0f);
			float NdotH = pow(max(dot(Nr, H), 0.0f),1.5f);

			if(fabs(correlationdata.x) > 0.0f)
			{
				float corrected_correlation = correlationdata.x;// (correlationdata.x-0.8f)/(1-0.8f);
				objectColor = lookupColor(corrected_correlation,colorTable,-1024.0f);
				//objectColor = lookupColor(pixeldata.x/256.0f,colorTable,-1024.0f);
				objectColor.w = fabs(corrected_correlation) /10.0f;// / 50.0f;
			}
			else
			{			
				objectColor = myColor;
				objectColor.w = 0.002f;
			}
			tempColor.xyz += objectColor.xyz * ambientColor.xyz; //ambient 
			tempColor.xyz += objectColor.xyz * NdotL * diffuseColor.xyz; //diffuse
			tempColor.xyz += objectColor.xyz * specularColor.xyz * NdotH; //specular
			
			objectColor.xyz = tempColor.xyz;
		}	
	}

	currentColor += objectColor * objectColor.w * remainingOpacity[0];
	remainingOpacity[0] *= (1 - currentColor.w);			
	
	return currentColor;
}

inline float sigmoid(float x)
{
	//return 1.0f/(1.0f+exp(-20.0f*x+4.0f));
	if(x < 0.2f)
		return 0.0f;
	if(x > 0.6f)
		return 0.8f;
	//return x;
}

inline float4 RayCompositeFunctionCortex(__read_only image3d_t raycastVolume, __read_only image3d_t atlasVolume, float8 bounds, float4 voxel, float4 rvVoxel, float4 cvVoxel, float4 avVoxel,float4 lightPos, float4 rayStart, float4 ambientColor, float4 diffuseColor, float4 specularColor, float4 currentColor,uint4 pixeldata,float4 correlationdata,uint4 atlasdata,uint4 atlasdata_nn,__read_only image2d_t colorTable,__read_only image2d_t colorTable2, float* remainingOpacity, int step, int num_steps)
{
	float4 objectColor = (float4)(0);
	
	if(correlationdata.x >= 99.0f)
	{
		objectColor = (float4)(0,1,0,0.1f);
		currentColor += objectColor*objectColor.w*remainingOpacity[0];
		remainingOpacity[0] *= (1-currentColor.w);
	}
	else if(correlationdata.x < -99.0f)
	{
		objectColor = (float4)(1,0,0,0.1f);
		currentColor += objectColor*objectColor.w*remainingOpacity[0];
		remainingOpacity[0] *= (1-currentColor.w);
	}
	else if(correlationdata.x >= -1 && pixeldata.x >= 0 && atlasdata.x >= 1)
	{
		if(fabs(correlationdata.x) > 0.0f)
		{
			float corrected_correlation = correlationdata.x;// (correlationdata.x-0.8f)/(1-0.8f);
			objectColor = lookupColor(corrected_correlation,colorTable,-1024.0f);
			objectColor.w *= fabs(corrected_correlation) / 10.0f;// / 50.0f;
		}
		else if(pixeldata.x < 130.0f)
		{
			float gradmag;
			float4 Nr = CalculateGradient(raycastVolume,rvVoxel,bounds,&gradmag);
			float4 L = fast_normalize(lightPos - voxel);
			float4 V = fast_normalize(rayStart - voxel);
			float4 H = fast_normalize(V+L);
			float NdotL = max(dot(Nr, L), 0.0f);
			float NdotH = pow(max(dot(Nr, H), 0.0f),1.5f);
			float z = convert_float(step)/convert_float(num_steps);
			objectColor = (float4)(pixeldata.x/256.0f,pixeldata.x/256.0f,pixeldata.x/256.0f,0.1f*z);
			//objectColor += (float4)(1,1,1,0.01f)*NdotH;
		}
		currentColor += objectColor * objectColor.w * remainingOpacity[0];
		remainingOpacity[0] *= (1-currentColor.w);
	} 
	
	return currentColor;
}
