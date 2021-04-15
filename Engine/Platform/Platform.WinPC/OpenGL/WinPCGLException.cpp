////////////////////////////////////////////////////////////////////////////////
// OpenGL/WinPCGLException.cpp (Leggiero - Platform.WinPC)
//
// WinPC Platform GL Error Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCGLException.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Graphics
			{
				//////////////////////////////////////////////////////////////////////////////// EGLViewCreateFailException

				//------------------------------------------------------------------------------
				EGLViewCreateFailException::EGLViewCreateFailException(EGLViewCreateErrorType error, EGLint eglError)
					: std::exception("Failed to create EGL View")
					, error(error)
					, eglError(eglError)
				{
				}
			}
		}
	}
}
