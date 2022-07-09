#ifndef _Rendering_DLLDEFINES_H_
#define _Rendering_DLLDEFINES_H_

/* Cmake will define MatrixIO_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define MatrixIO_EXPORTS when
building a DLL on windows.
*/
// We are using the Visual Studio Compiler and building Shared libraries

#if defined (_WIN32) 
  #if defined(Rendering_EXPORTS)
    #define  Rendering_EXPORT __declspec(dllexport)
  #else
    #define  Rendering_EXPORT __declspec(dllimport)
  #endif /* MyLibrary_EXPORTS */
#else /* defined (_WIN32) */
 #define Rendering_EXPORT
#endif

#endif /* _MyLibrary_DLLDEFINES_H_ */
