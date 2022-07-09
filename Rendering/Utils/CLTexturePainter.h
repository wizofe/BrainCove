#ifndef CLTEXTUREPAINTER_HEADER
#define CLTEXTUREPAINTER_HEADER

#include "../DLLDefines.h"
#include "clext.h"
#include <vtkOpenGL.h>
#include <CLWrappers/CLProgram.h>
#include <CLWrappers/CLTexture2DObject.h>
#include <BaseMatrix.hpp>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT CLTexturePainter
{
public:
	CLTexturePainter();
	~CLTexturePainter();

	void Paint(vtkVolume *vol, vtkRenderer *ren, CLTexture2DObject *texture, int imageViewPortSize[2], int imageOrigin[2], float requestedDepth);
	void PaintInternal( vtkVolume *vol, 
                        vtkRenderer *ren,
                        int imageMemorySize[2],
                        int imageViewportSize[2],
                        int imageInUseSize[2],
                        int imageOrigin[2],
                        float requestedDepth,
                        CLTexture2DObject *texture );

private:
	float PixelScale;

};
#endif //CLTEXTUREPAINTER_HEADER