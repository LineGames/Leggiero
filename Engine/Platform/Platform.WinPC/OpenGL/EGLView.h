////////////////////////////////////////////////////////////////////////////////
// OpenGL/EGLView.h (Leggiero - Platform.WinPC)
//
// EGL based OpenGL ES 3.0 View
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
				// Graphic Device via EGL
				class EGLView
				{
				public:
					EGLView(const HWND &windowHWnd);
					virtual ~EGLView();

				public:
					void SwapBuffers();

				private:
					EGLDisplay m_eglDisplay;
					EGLSurface m_eglSurface;
					EGLContext m_eglContext;
				};
			}
		}
	}
}
