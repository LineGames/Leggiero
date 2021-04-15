////////////////////////////////////////////////////////////////////////////////
// OpenGL/EGLView.cpp (Leggiero - Platform.WinPC)
//
// EGLView Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "EGLView.h"

// Leggiero.Platform.WinPC
#include "WinPCGLException.h"
#include "WinPCGLThreadContext.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Graphics
			{
				//////////////////////////////////////////////////////////////////////////////// EGLView

				//------------------------------------------------------------------------------
				EGLView::EGLView(const HWND &windowHWnd)
				{
					// get egl display handle
					m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
					if (m_eglDisplay == EGL_NO_DISPLAY)
					{
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kNoEGLDisplay, eglGetError());
					}

					// Initialize the display
					EGLint major = 0;
					EGLint minor = 0;
					if (eglInitialize(m_eglDisplay, &major, &minor) == EGL_FALSE)
					{
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kDisplayInitializeFail, eglGetError());
					}

					// Check Supported Version
					if (major < 1 || (major == 1 && minor < 5))
					{
						// Does not support EGL 1.5
						eglTerminate(m_eglDisplay);
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kEGLVersionNotSupport, eglGetError());
					}

					// Obtain the first configuration match
					EGLint attrs[] = {
						EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
						EGL_CONFORMANT, EGL_OPENGL_ES3_BIT,

						EGL_RED_SIZE, 8,
						EGL_GREEN_SIZE, 8,
						EGL_BLUE_SIZE, 8,
						EGL_ALPHA_SIZE, 8,

						EGL_DEPTH_SIZE, 24,
						EGL_STENCIL_SIZE, 8,

						EGL_NONE
					};
					EGLint numConfig = 0;
					EGLConfig eglConfig = 0;
					if (eglChooseConfig(m_eglDisplay, attrs, &eglConfig, 1, &numConfig) == EGL_FALSE)
					{
						eglTerminate(m_eglDisplay);
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kEGLConfigNotExists, eglGetError());
					}

					// Create a surface for the main window
					m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, windowHWnd, NULL);
					if (m_eglSurface == EGL_NO_SURFACE)
					{
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kSurfaceCreateFail, eglGetError());
					}

					// Create an OpenGL ES context
					EGLint contextAttrs[] = {
						EGL_CONTEXT_CLIENT_VERSION, 3,
						EGL_NONE
					};
					m_eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttrs);
					if (m_eglContext == EGL_NO_CONTEXT)
					{
						eglDestroySurface(m_eglDisplay, m_eglSurface);
						eglTerminate(m_eglDisplay);
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kContextCreateFail, eglGetError());
					}

					// Make the context and surface current
					if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
					{
						eglDestroyContext(m_eglDisplay, m_eglContext);
						eglDestroySurface(m_eglDisplay, m_eglSurface);
						eglTerminate(m_eglDisplay);
						throw EGLViewCreateFailException(EGLViewCreateErrorType::kCurrentMakeFail, eglGetError());
					}

					FillMainEGLContextInformation(m_eglDisplay, m_eglSurface, m_eglContext);
				}

				//------------------------------------------------------------------------------
				EGLView::~EGLView()
				{
					eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
					eglDestroyContext(m_eglDisplay, m_eglContext);
					eglDestroySurface(m_eglDisplay, m_eglSurface);
					eglTerminate(m_eglDisplay);
				}

				//------------------------------------------------------------------------------
				void EGLView::SwapBuffers()
				{
					eglSwapBuffers(m_eglDisplay, m_eglSurface);
				}
			}
		}
	}
}
