////////////////////////////////////////////////////////////////////////////////
// OpenGL/WinPCGLException.h (Leggiero - Platform.WinPC)
//
// OepnGL Related Exception in WinPC Platform
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <exception>

// External Library
#include <EGL/egl.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Graphics
			{
				// Error Code for the reason of EGL View Creation Fail
				enum class EGLViewCreateErrorType
				{
					kSuccess = 0,

					kNoEGLDisplay = 1,
					kDisplayInitializeFail = 2,

					kEGLVersionNotSupport = 11,
					kEGLConfigNotExists = 12,

					kSurfaceCreateFail = 21,
					kContextCreateFail = 22,

					kCurrentMakeFail = 31,
				};


				// EGL View Creation Fail
				class EGLViewCreateFailException : public std::exception
				{
				public:
					EGLViewCreateFailException(EGLViewCreateErrorType error, EGLint eglError);
					virtual ~EGLViewCreateFailException() throw() { }

				public:
					EGLViewCreateErrorType	error;
					EGLint					eglError;
				};
			}
		}
	}
}
