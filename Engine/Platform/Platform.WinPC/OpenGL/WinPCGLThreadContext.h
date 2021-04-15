////////////////////////////////////////////////////////////////////////////////
// OpenGL/WinPCGLThreadContext.h (Leggiero - Platform.WinPC)
//
// OpenGL Thread Context related Interfaces
////////////////////////////////////////////////////////////////////////////////

#pragma once


// External Library
#include <EGL/egl.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Graphics
			{
				// Notify Main EGL Context Information after Creation
				void FillMainEGLContextInformation(EGLDisplay eglDisplay, EGLSurface eglSurface, EGLContext eglContext);
			}
		}
	}
}
