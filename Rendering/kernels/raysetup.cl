inline void setupPerspectiveRay(int2 coord, __constant ViewParameters *viewParams, float4 *rayStartOut, float4 *rayEndOut, float4 *rayStepOut, uint* numStepsOut, float4 *lightPosOut)
{
	float4 planeCoord = (float4)(0,0,0,1);
	float4 rayStart = multiplyPoint(planeCoord,viewParams->viewToWorldMatrix);
	
	planeCoord = (float4)(0,0,1,1);
	float4 rayEnd = multiplyPoint(planeCoord,viewParams->viewToWorldMatrix);
	float4 rayCenter = rayEnd - rayStart;
	
	rayCenter /= viewParams->cameraThickness;
	
	float centerScale = fast_length(rayCenter.xyz);
	
	rayCenter /= centerScale;
	
	float4 lightPosV = (float4)(-1.0f,0,0.0f,1);
	float4 lightPos = multiplyPoint(planeCoord,viewParams->viewToWorldMatrix);
	//float4 lightPos = multiplyPoint((float4)(4.75f,-0.5f,0.75f,1),viewParams->viewToWorldMatrix);	
	
	float4 viewCoord4 = toNormView(coord,viewParams->viewPortSize,viewParams->imageOrigin,0);
	rayStart = multiplyPoint(viewCoord4,viewParams->viewToWorldMatrix);
	viewCoord4.z = 1;
	rayEnd = multiplyPoint(viewCoord4,viewParams->viewToWorldMatrix);
	float4 rayDirection = rayEnd - rayStart;
	
	float norm = fast_length(rayDirection.xyz);
	float4 rayStep = rayDirection.xyzw;
	rayStep /= norm;		

	float val = dot(rayStep, rayCenter);
	if(val != 0)
		norm = 1.0f / val;
	else
		norm = 1.0f;

	rayStep *= norm * centerScale * STEP_MULTIPLIER;

	int num_steps = convert_int((rayEnd.x - rayStart.x) / rayStep.x);
	
	rayStartOut[0] = rayStart;
	rayEndOut[0] = rayEnd;
	rayStepOut[0] = rayStep;
	numStepsOut[0] = num_steps;
	lightPosOut[0] = lightPos/2;
}

inline void setupFlatteningRay(int2 coord, __constant ViewParameters *viewParams, float4 *rayStartOut, float4 *rayEndOut, float4 *rayStepOut, uint* numStepsOut, float4 *lightPosOut)
{
	//float4 nv = toNormView(coord,viewParams->viewPortSize,viewParams->imageOrigin,0);
	float nx = (float)coord.x/(float)viewParams->imageInUseSize.s0;
	float ny = (float)coord.y/(float)viewParams->imageInUseSize.s1;

	float hoek = radians(nx*360.0f-180.0f);
	float hoeky = radians(ny*180.0f-90.0f);
	
	float phi = 2.0f*atan(ny/2.0f);
	float lam = hoek*2.0f;
	
	float x = 110.0f*cos(phi)*sin(lam);
	float y = 110.0f*sin(phi)*sin(lam);
	float z = 110.0f*cos(lam);

	float sx = 110.0f*cos(0.0f)*sin(radians(nx*360.0f));
	float sy = 110.0f*sin(0.0f)*sin(radians(nx*360.0f));
	float sz = 110.0f*cos(radians(nx*360.0f));



	
	//float X = 139.9557965930671983326481822227f*4*sin(hoek);
	//float Z = 139.9557965930671983326481822227f*4*cos(hoek);
	//float X = 1600.0f/2.0f*sin(hoek);
	//float Z = 1600.0f/2.0f*cos(hoek);
	
	
	//float4 rayStart = (float4)(viewParams->bounds.s1/2.0f,viewParams->bounds.s3/2.0f,ny*viewParams->bounds.s5,1);
	//float4 rayStart = (float4)(178.5f/2.0f,219.0f/2.0f,ny*152.5f,1);
	//float4 rayStart = (float4)(190.5f/2.0f,219.0f/2.0f,ny*152.5f,1);
	//float4 rayStart = (float4)(190.5f/2.0f,219.0f/2.0f,0.5f*152.5f,1);
	float4 rayStart = (float4)(sx,sz,sy,1);
	//float4 rayEnd = (float4)(X,Z,ny*viewParams->bounds.s5,1);
	//float4 rayEnd = (float4)(X,Z,ny*152.5f,1);
	float4 rayEnd = (float4)(x,z,y,1);
	float4 rayDirection = rayEnd - rayStart;

	float norm = fast_length(rayDirection.xyz);
	float4 rayStep = rayDirection.xyzw;
	rayStep /= norm;		

	rayStep *= STEP_MULTIPLIER;

	int num_steps = convert_int((rayEnd.x - rayStart.x) / rayStep.x);
	
	rayStartOut[0] = rayStart;
	rayEndOut[0] = rayEnd;
	rayStepOut[0] = rayStep;
	numStepsOut[0] = num_steps;
	lightPosOut[0] = rayStart;
}

inline void setupRay(int2 coord, __constant ViewParameters *viewParams, int method, float4 *rayStart, float4 *rayEnd, float4 *rayStep, uint* numSteps, float4 *lightPos)
{
	switch(method)
	{
		case 0:
			return setupPerspectiveRay(coord,viewParams,rayStart,rayEnd,rayStep,numSteps,lightPos);
		case 1:
			return setupFlatteningRay(coord,viewParams,rayStart,rayEnd,rayStep,numSteps,lightPos);
		break;
	}
}