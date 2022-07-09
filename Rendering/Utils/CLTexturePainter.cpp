#include "CLTexturePainter.h"

CLTexturePainter::CLTexturePainter()
{
	this->PixelScale = 1.0f;
}

CLTexturePainter::~CLTexturePainter()
{
}

void CLTexturePainter::Paint(vtkVolume *vol, vtkRenderer *ren, CLTexture2DObject *texture, int imageViewPortSize[2], int imageOrigin[2], float requestedDepth)
{
	int tSize[] = {texture->TextureWidth(), texture->TextureHeight()};
	int iSize[] = {texture->ImageWidth(), texture->ImageHeight()};
	PaintInternal(vol,ren,tSize,imageViewPortSize,iSize,imageOrigin,requestedDepth,texture);
}

void CLTexturePainter::PaintInternal( vtkVolume *vol, 
                                        vtkRenderer *ren,
                                        int imageMemorySize[2],
                                        int imageViewportSize[2],
                                        int imageInUseSize[2],
                                        int imageOrigin[2],
                                        float requestedDepth,
                                        CLTexture2DObject *texture )
{
  int i;
  float offsetX, offsetY;
  //float xMinOffset, xMaxOffset, yMinOffset, yMaxOffset;
  float tcoords[8];
  
  float depth;
  if ( requestedDepth > 0.0 && requestedDepth <= 1.0 )
    {
    depth = requestedDepth;
    }
  else
    {
    // Pass the center of the volume through the world to view function
    // of the renderer to get the z view coordinate to use for the
    // view to world transformation of the image bounds. This way we
    // will draw the image at the depth of the center of the volume
    ren->SetWorldPoint( vol->GetCenter()[0],
                        vol->GetCenter()[1],
                        vol->GetCenter()[2],
                        1.0 );
    ren->WorldToView();
    depth = ren->GetViewPoint()[2];
    }
  
    // Convert the four corners of the image into world coordinates
  float verts[12];
  vtkMatrix4x4 *viewToWorldMatrix = vtkMatrix4x4::New();
  float in[4], out[4];

  vtkCamera *cam = ren->GetActiveCamera();
  ren->ComputeAspect(); 
  double *aspect = ren->GetAspect();
  
  vtkTransform *perspectiveTransform = vtkTransform::New();
  perspectiveTransform->Identity();
  perspectiveTransform->Concatenate(
    cam->GetProjectionTransformMatrix(aspect[0]/aspect[1], 
                                      0.0, 1.0 ));
  perspectiveTransform->Concatenate(cam->GetViewTransformMatrix());
  
  // get the perspective transformation from the active camera 
  viewToWorldMatrix->DeepCopy( perspectiveTransform->GetMatrix() );
  perspectiveTransform->Delete();
  
  // use the inverse matrix 
  viewToWorldMatrix->Invert();

  // These two values never change
  in[2] = depth;
  in[3] = 1.0;
  
  // This is the lower left corner
  in[0] = (float)imageOrigin[0]/imageViewportSize[0] * 2.0 - 1.0; 
  in[1] = (float)imageOrigin[1]/imageViewportSize[1] * 2.0 - 1.0; 

  viewToWorldMatrix->MultiplyPoint( in, out );
  verts[0] = out[0] / out[3];
  verts[1] = out[1] / out[3];
  verts[2] = out[2] / out[3];
  
  // This is the lower right corner
  in[0] = (float)(imageOrigin[0]+imageInUseSize[0]) / 
    imageViewportSize[0] * 2.0 - 1.0;
  in[1] = (float)imageOrigin[1]/imageViewportSize[1] * 2.0 - 1.0;

  viewToWorldMatrix->MultiplyPoint( in, out );
  verts[3] = out[0] / out[3];
  verts[4] = out[1] / out[3];
  verts[5] = out[2] / out[3];

  // This is the upper right corner
  in[0] = (float)(imageOrigin[0]+imageInUseSize[0]) / 
    imageViewportSize[0] * 2.0 - 1.0;
  in[1] = (float)(imageOrigin[1]+imageInUseSize[1]) / 
    imageViewportSize[1] * 2.0 - 1.0;

  viewToWorldMatrix->MultiplyPoint( in, out );
  verts[6] = out[0] / out[3];
  verts[7] = out[1] / out[3];
  verts[8] = out[2] / out[3];

  // This is the upper left corner
  in[0] = (float)imageOrigin[0]/imageViewportSize[0] * 2.0 - 1.0;
  in[1] = (float)(imageOrigin[1]+imageInUseSize[1]) / 
    imageViewportSize[1] * 2.0 - 1.0;

  viewToWorldMatrix->MultiplyPoint( in, out );
  verts[9]  = out[0] / out[3];
  verts[10] = out[1] / out[3];
  verts[11] = out[2] / out[3];
  
  viewToWorldMatrix->Delete();

  // Save state
  glPushAttrib(GL_ENABLE_BIT         | 
               GL_COLOR_BUFFER_BIT   |
               GL_STENCIL_BUFFER_BIT |
               GL_DEPTH_BUFFER_BIT   | 
               GL_POLYGON_BIT        | 
               GL_PIXEL_MODE_BIT     |
               GL_TEXTURE_BIT);
  
  glPixelTransferf( GL_RED_SCALE,    this->PixelScale );
  glPixelTransferf( GL_GREEN_SCALE,  this->PixelScale );
  glPixelTransferf( GL_BLUE_SCALE,   this->PixelScale );
  glPixelTransferf( GL_ALPHA_SCALE,  this->PixelScale );
  
  glEnable (GL_BLEND);

  glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);

  // Turn lighting off - the texture already has illumination in it
  glDisable( GL_LIGHTING );

  // Turn texturing on so that we can draw the textured hexagon
  glEnable( GL_TEXTURE_2D );
  
  texture->Bind();

  // Don't write into the Zbuffer - just use it for comparisons
  glDepthMask( 0 );
  
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // Specify the texture
  glColor3f(1.0,1.0,1.0);
  
  offsetX = .5 / static_cast<float>(imageMemorySize[0]);
  offsetY = .5 / static_cast<float>(imageMemorySize[1]);
  
  tcoords[0]  = 0.0 + offsetX;
  tcoords[1]  = 0.0 + offsetY;
  tcoords[2]  = 
    (float)imageInUseSize[0]/(float)imageMemorySize[0] - offsetX;
  tcoords[3]  = offsetY;
  tcoords[4]  = 
    (float)imageInUseSize[0]/(float)imageMemorySize[0] - offsetX;
  tcoords[5]  = 
    (float)imageInUseSize[1]/(float)imageMemorySize[1] - offsetY;
  tcoords[6]  = offsetX;
  tcoords[7]  = 
    (float)imageInUseSize[1]/(float)imageMemorySize[1] - offsetY;
  
  // Render the polygon
  glBegin( GL_POLYGON );
  
  for ( i = 0; i < 4; i++ )
    {
    glTexCoord2fv( tcoords+i*2 );
    glVertex3fv( verts+i*3 );
    }
  glEnd();

  
#ifdef GL_VERSION_1_1
  glFlush();
  glFinish();
  //glDeleteTextures(1, &tempIndex);
#endif
  
  // Restore state
  glPopAttrib();
}
