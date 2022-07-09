#include "BrainVoxelSelector.h"

#define vtkVRCMultiplyPointMacro( A, B, M ) \
	B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
	B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
	B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
	B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
	if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

BrainVoxelSelector::BrainVoxelSelector()
{
	this->Data = NULL;
	this->Layout = NULL;
	this->Size   = 2;
	this->AtlasData = NULL;
	this->Type = RenderType::Template3D;

	this->LeftDisplaceMatrix = vtkMatrix4x4::New();
	this->RightDisplaceMatrix = vtkMatrix4x4::New();
	for(int i = 0; i < 4; i++)
	{
		this->PlanePosition[i] = 0;
		this->PlaneNormal[i] = 0;
	}
}

DataSelection* BrainVoxelSelector::SelectWithSeed(int seed)
{
	VoxelSelection *selection = new VoxelSelection();
	BrainLayout* layout = static_cast<BrainLayout*>(this->Layout);

	selection->AddElement(seed,1.0f);

	if(seed == -1)
		return selection;

	if(this->Data == NULL)
	{
		printf("Error: no data!");
		return selection;
	}
	if(this->Layout == NULL)
	{
		printf("Error: no layout!");
		return selection;
	}
	
	int dataExtent[6];
	this->GetPickData()->GetExtent(dataExtent);

	double seedPos[4];
	this->Layout->GetPosition(seed,seedPos);
	seedPos[3] = 1;

	double worldPos[4];
	vtkVRCMultiplyPointMacro(seedPos,worldPos,PickVoxelToWorldMatrix);

	selection->SetPosition(worldPos[0],worldPos[1],worldPos[2]);

	for(int i = -1*this->Size; i <= this->Size; i++)
	{
		if(seedPos[0]+i < dataExtent[0] || seedPos[0]+i >= dataExtent[1])
			continue;
		for(int j = -1*this->Size; j <= this->Size; j++)
		{
			if(seedPos[1]+j < dataExtent[2] || seedPos[1]+j >= dataExtent[3])
				continue;
			for(int k = -1*this->Size; k <= this->Size; k++)
			{
				if(seedPos[2]+k < dataExtent[4] || seedPos[2]+k >= dataExtent[5])
					continue;
				
				double dx = (double)(seedPos[0])-(double)(seedPos[0]+i);
				double dy = (double)(seedPos[1])-(double)(seedPos[1]+j);
				double dz = (double)(seedPos[2])-(double)(seedPos[2]+k);

				if(sqrt(dx*dx+dy*dy+dz*dz) > Size)
					continue;

				short *id = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)seedPos[0]+i,(int)seedPos[1]+j,(int)seedPos[2]+k));
				if(id && *id!=-1)
					selection->AddElement(*id,1.0f);
					//map.insert(SelectionEntry(*id,1.0f));
			}
		}
	}

	return selection;
}

DataSelection* BrainVoxelSelector::SelectWithTemplate(int seed)
{
	VoxelSelection *selection = new VoxelSelection();
	BrainLayout* layout = static_cast<BrainLayout*>(this->Layout);

	selection->AddElement(seed,1.0f);

	if(seed == -1)
		return selection;

	if(this->Data == NULL)
	{
		printf("Error: no data!");
		return selection;
	}
	if(this->Layout == NULL)
	{
		printf("Error: no layout!");
		return selection;
	}
	
	int dataExtent[6];
	int pickDataExtent[6];
	this->GetAtlasData()->GetExtent(dataExtent);
	this->GetPickData()->GetExtent(pickDataExtent);

	double seedPos[4];
	this->Layout->GetPosition(seed,seedPos);
	seedPos[3] = 1;

	double tmplPos1[4];
	double tmplPos2[4];
	vtkVRCMultiplyPointMacro(seedPos,tmplPos1,this->PickVoxelToWorldMatrix); //back to world coordinates
	vtkVRCMultiplyPointMacro(tmplPos1,tmplPos2,this->WorldToAtlasVoxelMatrix); //from world to template coordinates
	short *rgn = static_cast<short*>(this->GetAtlasData()->GetScalarPointer((int)(tmplPos2[0]),(int)(tmplPos2[1]),(int)(tmplPos2[2])));

	for(int i = dataExtent[0]; i < dataExtent[1]; i++)
	{
		for(int j = dataExtent[2]; j < dataExtent[3]; j++)
		{
			for(int k = dataExtent[4]; k < dataExtent[5]; k++)
			{
				short *rgnn = static_cast<short*>(this->GetAtlasData()->GetScalarPointer(i,j,k));
				if(*rgn == *rgnn)
				{
					double rgnPos[4] = {i,j,k,1};
					double rgnTPos[4];
					vtkVRCMultiplyPointMacro(rgnPos,rgnTPos,this->AtlasVoxelToWorldMatrix); //back to world coordinates
					vtkVRCMultiplyPointMacro(rgnTPos,rgnTPos,this->WorldToPickVoxelMatrix); //from world to template coordinates
					if(rgnTPos[0] >= pickDataExtent[0] && rgnTPos[0] <= pickDataExtent[1] &&
						rgnTPos[1] >= pickDataExtent[2] && rgnTPos[1] <= pickDataExtent[3] &&
						rgnTPos[2] >= pickDataExtent[4] && rgnTPos[2] <= pickDataExtent[5])
					{
						short *voxId = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)rgnTPos[0],(int)rgnTPos[1],(int)rgnTPos[2]));
						if(*voxId != -1)
							selection->AddElement((int)*voxId,1);
					}
				}
			}
		}
	}

	return selection;
}

DataSelection* BrainVoxelSelector::PickVolume(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *ViewToWorldMatrix, int imageBounds[4])
{
	switch(this->Type)
	{
	case RenderType::Template3D:
		return PickVolume3D(coords,renderer,ViewToWorldMatrix,imageBounds);
	case RenderType::MappedCortex:
		return PickVolume2DMapped(coords,renderer,ViewToWorldMatrix,imageBounds);
	default:
		return 0;
	}
	//return this->SelectWithTemplate(voxel);
}



DataSelection* BrainVoxelSelector::PickVolume3D(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *ViewToWorldMatrix, int imageBounds[4])
{
	int x = coords[0];
	int y = coords[1];

	float viewRay[4];
	float rayCenter[3];
	float voxelPoint[4];
	float rayStart[4];
	float rayEnd[4];

	//this is the rotation point
	//currently it is fixed, in future, this should be derived from the mouse hover point
	//float brainCenter[4] = {180.5f/2.0f,30,152.5f/2.0f,1};
	float brainCenter[4];
	memcpy(brainCenter,this->PlanePosition,sizeof(float)*4);

	vtkCamera *cam = renderer->GetActiveCamera();

	double viToVoMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			viToVoMat[4*i+j] = ViewToWorldMatrix->GetElement(i,j);

	float leftMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			leftMat[4*i+j] = LeftDisplaceMatrix->GetElement(i,j);

	float rightMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			rightMat[4*i+j] = RightDisplaceMatrix->GetElement(i,j);

	// This is tbe near end of the center ray in view coordinates
	// Convert it to voxel coordinates
	viewRay[0] = viewRay[1] = viewRay[2] = 0.0;  
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );

	// This is the far end of the center ray in view coordinates
	// Convert it to voxel coordiantes
	viewRay[2] = 1.0;  
	vtkVRCMultiplyPointMacro( viewRay, voxelPoint,
							viToVoMat );

	// Turn these two points into a vector
	rayCenter[0] = voxelPoint[0] - rayStart[0];
	rayCenter[1] = voxelPoint[1] - rayStart[1];
	rayCenter[2] = voxelPoint[2] - rayStart[2];

	// normalize the vector based on world coordinate distance
	// This way we can scale by sample distance and it will work
	// out even though we are in voxel coordinates
	rayCenter[0] /= cam->GetThickness();
	rayCenter[1] /= cam->GetThickness();
	rayCenter[2] /= cam->GetThickness();

	float centerScale = sqrt( (rayCenter[0] * rayCenter[0]) +
							(rayCenter[1] * rayCenter[1]) +
							(rayCenter[2] * rayCenter[2]) );

	rayCenter[0] /= centerScale;
	rayCenter[1] /= centerScale;
	rayCenter[2] /= centerScale;

	int viewportSize[2];
	renderer->GetTiledSize(&viewportSize[0], &viewportSize[1]);

	NormalizeCoordinates(x,y,0,1,viewportSize,viewRay);
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );
	
	viewRay[2] = 1;
	vtkVRCMultiplyPointMacro( viewRay, rayEnd,
							viToVoMat );

	double rayDirection[3];
	rayDirection[0] = rayEnd[0] - rayStart[0];
	rayDirection[1] = rayEnd[1] - rayStart[1];
	rayDirection[2] = rayEnd[2] - rayStart[2];

	double norm = sqrt( rayDirection[0] * rayDirection[0] +
                   rayDirection[1] * rayDirection[1] +
                   rayDirection[2] * rayDirection[2] );
          
	double rayStep[3];
	// Normalize this ray into rayStep
	rayStep[0] = rayDirection[0] / norm;
	rayStep[1] = rayDirection[1] / norm;
	rayStep[2] = rayDirection[2] / norm;

	// Correct for perspective in the sample distance. 1.0 over the
	// dot product between this ray and the center ray is the
	// correction factor to allow samples to be taken on parallel
	// planes rather than concentric hemispheres. This factor will
	// compute to be 1.0 for parallel.
	float val =  ( rayStep[0] * rayCenter[0] +
		   rayStep[1] * rayCenter[1] +
		   rayStep[2] * rayCenter[2] );
	norm = (val != 0)?(1.0/val):(1.0);

	// Now multiple the normalized step by the sample distance and this
	// correction factor to find the actual step
	rayStep[0] *= norm * centerScale;
	rayStep[1] *= norm * centerScale;
	rayStep[2] *= norm * centerScale;

	int num_steps = (int)((rayEnd[0] - rayStart[0]) / rayStep[0]);

	int voxel = -1;
	rayStart[3] = 1;

	for(int i = 0; i < 4; i++)
		voxelPoint[i] = rayStart[i];

	float cVoxelPoint[4];
	float voxelPointDisplaced[4];
	int hemisphere = 0;

	//float planeNormal[4] = {-1,0,0,1};
	float planeNormal[4];
	memcpy(planeNormal,this->PlaneNormal,sizeof(float)*4);
	
	double bounds[6];
	int extent[6];
	this->GetVoxelData()->GetBounds(bounds);

	this->GetPickData()->GetExtent(extent);

	for(int i = 0; i < num_steps && voxel == -1; i++)
	{
		float vpdNormal[4];
		Clone(voxelPoint,voxelPointDisplaced);

		//translate to origin before rotation
		Subtract(voxelPointDisplaced, brainCenter, voxelPointDisplaced);
		
		Clone(voxelPointDisplaced,vpdNormal);
		Normalize(vpdNormal,vpdNormal);

		float dot = Dot(planeNormal,vpdNormal);
		if(dot < 0) //left
		{
			float tmp[4];
			hemisphere = -1;
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,tmp,leftMat);
			Clone(tmp,voxelPointDisplaced);
		}
		else if(dot > 0) //right
		{
			float tmp[4];
			hemisphere = 1;
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,tmp,rightMat);
			Clone(tmp,voxelPointDisplaced);
		}
		//else undefined

		//translate back after rotation
		Clone(voxelPointDisplaced,vpdNormal);
		Normalize(vpdNormal,vpdNormal);

		Add(voxelPointDisplaced,brainCenter,voxelPointDisplaced);

		if(Dot(planeNormal,vpdNormal) > 0 && hemisphere == -1)
		{
			for(int j = 0; j < 4; j++)
				voxelPoint[j] += rayStep[j];
			continue;
		}

		Subtract(brainCenter,voxelPointDisplaced,vpdNormal);
		Normalize(vpdNormal,vpdNormal);

		if(Dot(planeNormal,vpdNormal) > 0 && hemisphere == 1)
		{
			for(int j = 0; j < 4; j++)
				voxelPoint[j] += rayStep[j];
			continue;
		}

		if(!(voxelPointDisplaced[0] < bounds[0] || voxelPointDisplaced[0] > bounds[1] ||
			voxelPointDisplaced[1] < bounds[2] || voxelPointDisplaced[1] > bounds[3] ||
			voxelPointDisplaced[2] < bounds[4] || voxelPointDisplaced[2] > bounds[5]))
		{
			//printf("--%.2f, %.2f, %.2f\n",voxelPointDisplaced[0],voxelPointDisplaced[1],voxelPointDisplaced[2]);
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,cVoxelPoint,this->WorldToPickVoxelMatrix);
			if(!(cVoxelPoint[0] < extent[0] || cVoxelPoint[0] > extent[1] ||
				cVoxelPoint[1] < extent[2] || cVoxelPoint[1] > extent[3] ||
				cVoxelPoint[2] < extent[4] || cVoxelPoint[2] > extent[5]))
			{
				short* ptr = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)(cVoxelPoint[0]),(int)(cVoxelPoint[1]),(int)(cVoxelPoint[2])));
				voxel = *ptr;
			}
		}

		for(int j = 0; j < 4; j++)
			voxelPoint[j] += rayStep[j];
	}
	DataSelection *sel = this->SelectWithSeed(voxel);
	sel->SetPosition(voxelPointDisplaced[0],voxelPointDisplaced[1],voxelPointDisplaced[2]);
	return sel;
	//return this->SelectWithTemplate(voxel);
}

DataSelection* BrainVoxelSelector::PickLastVolume3D(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *ViewToWorldMatrix, int imageBounds[4])
{
	int x = coords[0];
	int y = coords[1];

	float viewRay[4];
	float rayCenter[3];
	float voxelPoint[4];
	float rayStart[4];
	float rayEnd[4];

	//this is the rotation point
	//currently it is fixed, in future, this should be derived from the mouse hover point
	//float brainCenter[4] = {180.5f/2.0f,30,152.5f/2.0f,1};
	float brainCenter[4];
	memcpy(brainCenter,this->PlanePosition,sizeof(float)*4);

	vtkCamera *cam = renderer->GetActiveCamera();

	double viToVoMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			viToVoMat[4*i+j] = ViewToWorldMatrix->GetElement(i,j);

	float leftMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			leftMat[4*i+j] = LeftDisplaceMatrix->GetElement(i,j);

	float rightMat[16];
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			rightMat[4*i+j] = RightDisplaceMatrix->GetElement(i,j);

	// This is tbe near end of the center ray in view coordinates
	// Convert it to voxel coordinates
	viewRay[0] = viewRay[1] = viewRay[2] = 0.0;  
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );

	// This is the far end of the center ray in view coordinates
	// Convert it to voxel coordiantes
	viewRay[2] = 1.0;  
	vtkVRCMultiplyPointMacro( viewRay, voxelPoint,
							viToVoMat );

	// Turn these two points into a vector
	rayCenter[0] = voxelPoint[0] - rayStart[0];
	rayCenter[1] = voxelPoint[1] - rayStart[1];
	rayCenter[2] = voxelPoint[2] - rayStart[2];

	// normalize the vector based on world coordinate distance
	// This way we can scale by sample distance and it will work
	// out even though we are in voxel coordinates
	rayCenter[0] /= cam->GetThickness();
	rayCenter[1] /= cam->GetThickness();
	rayCenter[2] /= cam->GetThickness();

	float centerScale = sqrt( (rayCenter[0] * rayCenter[0]) +
							(rayCenter[1] * rayCenter[1]) +
							(rayCenter[2] * rayCenter[2]) );

	rayCenter[0] /= centerScale;
	rayCenter[1] /= centerScale;
	rayCenter[2] /= centerScale;

	int viewportSize[2];
	renderer->GetTiledSize(&viewportSize[0], &viewportSize[1]);

	NormalizeCoordinates(x,y,0,1,viewportSize,viewRay);
	vtkVRCMultiplyPointMacro( viewRay, rayStart,
							viToVoMat );
	
	viewRay[2] = 1;
	vtkVRCMultiplyPointMacro( viewRay, rayEnd,
							viToVoMat );

	double rayDirection[3];
	rayDirection[0] = rayEnd[0] - rayStart[0];
	rayDirection[1] = rayEnd[1] - rayStart[1];
	rayDirection[2] = rayEnd[2] - rayStart[2];

	double norm = sqrt( rayDirection[0] * rayDirection[0] +
                   rayDirection[1] * rayDirection[1] +
                   rayDirection[2] * rayDirection[2] );
          
	double rayStep[3];
	// Normalize this ray into rayStep
	rayStep[0] = rayDirection[0] / norm;
	rayStep[1] = rayDirection[1] / norm;
	rayStep[2] = rayDirection[2] / norm;

	// Correct for perspective in the sample distance. 1.0 over the
	// dot product between this ray and the center ray is the
	// correction factor to allow samples to be taken on parallel
	// planes rather than concentric hemispheres. This factor will
	// compute to be 1.0 for parallel.
	float val =  ( rayStep[0] * rayCenter[0] +
		   rayStep[1] * rayCenter[1] +
		   rayStep[2] * rayCenter[2] );
	norm = (val != 0)?(1.0/val):(1.0);

	// Now multiple the normalized step by the sample distance and this
	// correction factor to find the actual step
	rayStep[0] *= norm * centerScale;
	rayStep[1] *= norm * centerScale;
	rayStep[2] *= norm * centerScale;

	int num_steps = (int)((rayEnd[0] - rayStart[0]) / rayStep[0]);

	int voxel = -1;
	rayStart[3] = 1;

	for(int i = 0; i < 4; i++)
		voxelPoint[i] = rayEnd[i];

	float cVoxelPoint[4];
	float voxelPointDisplaced[4];
	int hemisphere = 0;

	//float planeNormal[4] = {-1,0,0,1};
	float planeNormal[4];
	memcpy(planeNormal,this->PlaneNormal,sizeof(float)*4);

	double bounds[6];
	int extent[6];
	this->GetVoxelData()->GetBounds(bounds);

	this->GetPickData()->GetExtent(extent);

	short lastVoxel = -1;

	for(int i = 0; i < num_steps && voxel == -1; i++)
	{
		float vpdNormal[4];
		Clone(voxelPoint,&voxelPointDisplaced[0]);

		//translate to origin before rotation
		Subtract(voxelPointDisplaced, brainCenter, &voxelPointDisplaced[0]);
		
		Clone(voxelPointDisplaced,&vpdNormal[0]);
		Normalize(vpdNormal,&vpdNormal[0]);

		float dot = Dot(planeNormal,vpdNormal);
		if(dot < 0) //left
		{
			float tmp[4];
			hemisphere = -1;
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,tmp,leftMat);
			Clone(tmp,voxelPointDisplaced);
		}
		else if(dot > 0) //right
		{
			float tmp[4];
			hemisphere = 1;
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,tmp,rightMat);
			Clone(tmp,voxelPointDisplaced);
		}
		//else undefined

		//translate back after rotation
		Clone(voxelPointDisplaced,&vpdNormal[0]);
		Normalize(vpdNormal,&vpdNormal[0]);

		Add(voxelPointDisplaced,brainCenter,&voxelPointDisplaced[0]);

		if(Dot(planeNormal,vpdNormal) >= 0 && hemisphere == -1)
		{
			for(int j = 0; j < 4; j++)
				voxelPoint[j] -= rayStep[j];
			continue;
		}

		Subtract(brainCenter,voxelPointDisplaced,&vpdNormal[0]);
		Normalize(vpdNormal,&vpdNormal[0]);

		if(Dot(planeNormal,vpdNormal) >= 0 && hemisphere == 1)
		{
			for(int j = 0; j < 4; j++)
				voxelPoint[j] -= rayStep[j];
			continue;
		}

		if(!(voxelPointDisplaced[0] < bounds[0] || voxelPointDisplaced[0] > bounds[1] ||
			voxelPointDisplaced[1] < bounds[2] || voxelPointDisplaced[1] > bounds[3] ||
			voxelPointDisplaced[2] < bounds[4] || voxelPointDisplaced[2] > bounds[5]))
		{
			//printf("--%.2f, %.2f, %.2f\n",voxelPointDisplaced[0],voxelPointDisplaced[1],voxelPointDisplaced[2]);
			vtkVRCMultiplyPointMacro(voxelPointDisplaced,cVoxelPoint,this->WorldToPickVoxelMatrix);
			if(!(cVoxelPoint[0] < extent[0] || cVoxelPoint[0] > extent[1] ||
				cVoxelPoint[1] < extent[2] || cVoxelPoint[1] > extent[3] ||
				cVoxelPoint[2] < extent[4] || cVoxelPoint[2] > extent[5]))
			{
				short* ptr = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)(cVoxelPoint[0]),(int)(cVoxelPoint[1]),(int)(cVoxelPoint[2])));
				voxel = *ptr;
			}
		}

		for(int j = 0; j < 4; j++)
			voxelPoint[j] -= rayStep[j];
	}
	DataSelection *sel = this->SelectWithSeed(voxel);
	sel->SetPosition(voxelPointDisplaced[0],voxelPointDisplaced[1],voxelPointDisplaced[2]);
	return sel;
	//return this->SelectWithTemplate(voxel);
}

DataSelection* BrainVoxelSelector::PickVolume2DMapped(int coords[2], vtkRenderer *renderer, vtkMatrix4x4 *ViewToWorldMatrix, int imageBounds[4])
{
	if(coords[0] < imageBounds[0] || coords[0] > imageBounds[1] || coords[1] < imageBounds[2] || coords[1] > imageBounds[3])
		return this->SelectWithSeed(-1);

	float nx, ny;
	nx = ((float)coords[0] - (float)imageBounds[0])/((float)imageBounds[1]-(float)imageBounds[0]);
	ny = ((float)coords[1] - (float)imageBounds[2])/((float)imageBounds[3]-(float)imageBounds[2]);

	float hoek = (nx*360.0f-180.0f)*M_PI/180.0f;
	
	float X = 139.9557965930671983326481822227f*4*sin(hoek);
	float Z = 139.9557965930671983326481822227f*4*cos(hoek);
	//float X = 1600.0f/2.0f*sin(hoek);
	//float Z = 1600.0f/2.0f*cos(hoek);
	
	
	//float4 rayStart = (float4)(viewParams->bounds.s1/2.0f,viewParams->bounds.s3/2.0f,ny*viewParams->bounds.s5,1);
	//float4 rayStart = (float4)(178.5f/2.0f,219.0f/2.0f,ny*152.5f,1);
	float rayStart[4] = {190.5f/2.0f,219.0f/2.0f,ny*152.5f,1};
	float rayEnd[4] = {X,Z,ny*152.5f,1};

	float rayDirection[4];
	for(int i = 0;i<4;i++)
		rayDirection[i] = rayEnd[i] - rayStart[i];

	float norm = sqrt(rayDirection[0]*rayDirection[0]+rayDirection[1]*rayDirection[1]+rayDirection[2]*rayDirection[2]);
	
	float rayStep[4];
	for(int i = 0;i<4;i++)
	{
		rayStep[i] = rayDirection[i]/norm;		
		//rayStep *= STEP_MULTIPLIER;
	}

	int num_steps = (int)((rayEnd[0] - rayStart[0]) / rayStep[0]);

	int voxel = -1;

	float voxelPoint[4];
	for(int i = 0; i < 3; i++)
		voxelPoint[i] = rayStart[i];

	float cVoxelPoint[4];

	double bounds[6];
	int extent[6];
	this->GetVoxelData()->GetBounds(bounds);
	//this->GetPickData()->GetBounds(bounds);
	//bounds[0] = bounds[2] = bounds[4] = 0;
	this->GetPickData()->GetExtent(extent);

	for(int i = 0; i < num_steps; i++)
	{
		if(!(voxelPoint[0] < bounds[0] || voxelPoint[0] > bounds[1] ||
			voxelPoint[1] < bounds[2] || voxelPoint[1] > bounds[3] ||
			voxelPoint[2] < bounds[4] || voxelPoint[2] > bounds[5]))
		{
			vtkVRCMultiplyPointMacro(voxelPoint,cVoxelPoint,this->WorldToPickVoxelMatrix);
			if(!(cVoxelPoint[0] < extent[0] || cVoxelPoint[0] > extent[1] ||
				cVoxelPoint[1] < extent[2] || cVoxelPoint[1] > extent[3] ||
				cVoxelPoint[2] < extent[4] || cVoxelPoint[2] > extent[5]))
			{
				//printf("%.2f, %.2f, %.2f\n",cVoxelPoint[0],cVoxelPoint[1],cVoxelPoint[2]);
				short* ptr = static_cast<short*>(this->GetPickData()->GetScalarPointer((int)(cVoxelPoint[0]),(int)(cVoxelPoint[1]),(int)(cVoxelPoint[2])));
				if(*ptr!=-1)
					voxel = *ptr;
			}
		}

		for(int j = 0; j < 3; j++)
			voxelPoint[j] += rayStep[j];
	}

	return this->SelectWithSeed(voxel);
	//return this->SelectWithTemplate(voxel);
}

void BrainVoxelSelector::SetRenderType(RenderType type)
{
	this->Type = type;
}

void BrainVoxelSelector::SetDisplacementFromAngle(float angleDeg,double viewUp[3])
{
	vtkTransform *transform = vtkTransform::New();

	transform->Identity();
	transform->RotateWXYZ(-angleDeg, viewUp);
	//transform->RotateZ(angleDeg);

	this->LeftDisplaceMatrix->DeepCopy(transform->GetMatrix());

	transform->Identity();
	//transform->RotateZ(-angleDeg);
	transform->RotateWXYZ(angleDeg, viewUp);

	this->RightDisplaceMatrix->DeepCopy(transform->GetMatrix());

	transform->Delete();

}

void BrainVoxelSelector::SetBrainSplit(float planePos[4],float planeNorm[4])
{
	for(int i = 0; i < 4; i++)
	{
		this->PlanePosition[i] = planePos[i];
		this->PlaneNormal[i] = planeNorm[i];
	}
}