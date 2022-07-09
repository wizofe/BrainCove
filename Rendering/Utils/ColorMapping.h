#ifndef COLORMAPPING_HEADER
#define COLORMAPPING_HEADER

#include "../DLLDefines.h"
#include <vtkLookupTable.h>
#include <vtkOpenGL.h>
#include <vtkUnsignedCharArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <math.h>

/*
	exports for static functions:
	{
	  public:
		static Rendering_EXPORT void MyExportedFunction(int i);
	};
*/

class Rendering_EXPORT ColorMapping
{
public:
	ColorMapping();
	~ColorMapping();

	vtkLookupTable *BlueToYellowColormap();
	void makeAlpha(vtkLookupTable *table);

	static vtkLookupTable *ImportFromDevideTF(std::string filename, int size = 1024, vtkColorTransferFunction *ext_ctf = 0, vtkPiecewiseFunction *ext_otf = 0);

	static vtkLookupTable *BlueToYellow()
	{
		return Instance()->BlueToYellowColormap();
	}

	static unsigned char *BlueToYellowCharArray()
	{
		//return lut->GetTable()->GetPointer(0);
		return Instance()->BlueToYellowColormap()->GetTable()->GetPointer(0);
	}

	static ColorMapping *Instance()
    {
		if(s_instance == NULL)
			s_instance = new ColorMapping();
        return s_instance;
    }

private:
	vtkLookupTable *lut;
	static ColorMapping *s_instance;
};
#endif //COLORMAPPING_HEADER