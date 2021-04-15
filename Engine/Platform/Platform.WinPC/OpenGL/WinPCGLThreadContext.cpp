////////////////////////////////////////////////////////////////////////////////
// OpenGL/WinPCGLThreadContext.cpp (Leggiero - Platform.WinPC)
//
// WinPC Platform Graphics Thread Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCGLThreadContext.h"

// Leggiero.Graphics
#include <Graphics/GraphicsThreadContext.h>


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Platform Dependent Implementation

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			static bool s_isMainContextInfoInitialzed = false;
			static EGLDisplay s_mainEGLDisplay;
			static EGLSurface s_mainEGLSurface;
			static EGLContext s_mainEGLContext;


			//////////////////////////////////////////////////////////////////////////////// WinPCEGLThreadContextInformation

			// Thread Context Information for WinPC EGL
			class WinPCEGLThreadContextInformation : public IGLThreadContextInformation
			{
			public:
				WinPCEGLThreadContextInformation()
					: eglDisplay(EGL_NO_DISPLAY), eglSurface(EGL_NO_SURFACE), eglContext(EGL_NO_CONTEXT)
				{ }

				virtual ~WinPCEGLThreadContextInformation() { }

			public:
				EGLDisplay eglDisplay;
				EGLSurface eglSurface;
				EGLContext eglContext;
			};
		}


		//////////////////////////////////////////////////////////////////////////////// WinPC Platform Dependent Graphics Thread Functions

		//------------------------------------------------------------------------------
		// Check whether if the initialization by GL context of the main rendering thread have done or not
		bool IsMainGLContextInitialized()
		{
			return _Internal::s_isMainContextInfoInitialzed;
		}

		//------------------------------------------------------------------------------
		// Initialize the main OpenGL Context
		void InitializeMainGLContextInGraphicThread()
		{
			// Nothing to Do
		}

		//------------------------------------------------------------------------------
		// Create called thread's OpenGL Context and return the infromation object
		std::shared_ptr<IGLThreadContextInformation> MakeThreadGLContext()
		{
			if (!_Internal::s_isMainContextInfoInitialzed)
			{
				// Cannot make thread context before main context initialized
				return nullptr;
			}

			std::shared_ptr<_Internal::WinPCEGLThreadContextInformation> info(std::make_shared<_Internal::WinPCEGLThreadContextInformation>());

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
			if (eglChooseConfig(_Internal::s_mainEGLDisplay, attrs, &eglConfig, 1, &numConfig) == EGL_FALSE)
			{
				return nullptr;
			}

			// Create a pbuffer surface
			EGLint surfaceAttrs[] = {
				EGL_WIDTH, 1,
				EGL_HEIGHT, 1,
				EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
				EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
				EGL_NONE
			};
			info->eglSurface = eglCreatePbufferSurface(_Internal::s_mainEGLDisplay, eglConfig, surfaceAttrs);
			if (info->eglSurface == EGL_NO_SURFACE)
			{
				return nullptr;
			}

			// Create an OpenGL ES context
			EGLint contextAttrs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 3,
				EGL_NONE
			};
			info->eglContext = eglCreateContext(_Internal::s_mainEGLDisplay, eglConfig, _Internal::s_mainEGLContext, contextAttrs);
			if (info->eglContext == EGL_NO_CONTEXT)
			{
				eglDestroySurface(_Internal::s_mainEGLDisplay, info->eglSurface);
				return nullptr;
			}

			// Make the context and surface current
			if (!eglMakeCurrent(_Internal::s_mainEGLDisplay, info->eglSurface, info->eglSurface, info->eglContext))
			{
				eglDestroyContext(_Internal::s_mainEGLDisplay, info->eglContext);
				eglDestroySurface(_Internal::s_mainEGLDisplay, info->eglSurface);
				return nullptr;
			}

			info->eglDisplay = _Internal::s_mainEGLDisplay;
			return info;
		}

		//------------------------------------------------------------------------------
		// Clean up called thread's OpenGL Context
		void TerminateThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo)
		{
			std::shared_ptr<_Internal::WinPCEGLThreadContextInformation> concreteInfo(std::dynamic_pointer_cast<_Internal::WinPCEGLThreadContextInformation>(contextInfo));

			eglMakeCurrent(concreteInfo->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if (concreteInfo->eglContext != EGL_NO_CONTEXT)
			{
				eglDestroyContext(concreteInfo->eglDisplay, concreteInfo->eglContext);
			}
			if (concreteInfo->eglSurface != EGL_NO_SURFACE)
			{
				eglDestroySurface(concreteInfo->eglDisplay, concreteInfo->eglSurface);
			}
		}

		//------------------------------------------------------------------------------
		// Check called thread's OpenGL Context's validity and restore it if needed
		void CheckAndRefreshThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo)
		{
			if (eglGetCurrentContext() == EGL_NO_CONTEXT || eglGetCurrentSurface(EGL_DRAW) == EGL_NO_SURFACE)
			{
				std::shared_ptr<_Internal::WinPCEGLThreadContextInformation> concreteInfo(std::dynamic_pointer_cast<_Internal::WinPCEGLThreadContextInformation>(contextInfo));

				if (concreteInfo->eglDisplay != _Internal::s_mainEGLDisplay)
				{
					concreteInfo->eglDisplay = _Internal::s_mainEGLDisplay;
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
				if (eglChooseConfig(concreteInfo->eglDisplay, attrs, &eglConfig, 1, &numConfig) == EGL_FALSE)
				{
					// Error
					return;
				}

				if (eglGetCurrentContext() == EGL_NO_CONTEXT)
				{
					EGLint contextAttrs[] = {
						EGL_CONTEXT_CLIENT_VERSION, 3,
						EGL_NONE
					};
					concreteInfo->eglContext = eglCreateContext(_Internal::s_mainEGLDisplay, eglConfig, _Internal::s_mainEGLContext, contextAttrs);
					if (concreteInfo->eglContext == EGL_NO_CONTEXT)
					{
						return;
					}
				}
				else if (eglGetCurrentContext() != concreteInfo->eglContext)
				{
					concreteInfo->eglContext = eglGetCurrentContext();
				}

				if (eglGetCurrentSurface(EGL_DRAW) == EGL_NO_SURFACE)
				{
					EGLint surfaceAttrs[] = {
						EGL_WIDTH, 1,
						EGL_HEIGHT, 1,
						EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
						EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
						EGL_NONE
					};
					concreteInfo->eglSurface = eglCreatePbufferSurface(_Internal::s_mainEGLDisplay, eglConfig, surfaceAttrs);
					if (concreteInfo->eglSurface == EGL_NO_SURFACE)
					{
						return;
					}
				}
				else if (eglGetCurrentSurface(EGL_DRAW) != concreteInfo->eglSurface)
				{
					concreteInfo->eglSurface = eglGetCurrentSurface(EGL_DRAW);
				}

				eglMakeCurrent(concreteInfo->eglDisplay, concreteInfo->eglSurface, concreteInfo->eglSurface, concreteInfo->eglContext);
			}
		}
	}


	namespace Platform
	{
		namespace Windows
		{
			namespace Graphics
			{
				//////////////////////////////////////////////////////////////////////////////// WinPC Platform EGL/OpenGL ES Stuffs

				//------------------------------------------------------------------------------
				// Notify Main EGL Context Information after Creation
				void FillMainEGLContextInformation(EGLDisplay eglDisplay, EGLSurface eglSurface, EGLContext eglContext)
				{
					Leggiero::Graphics::_Internal::s_mainEGLDisplay = eglDisplay;
					Leggiero::Graphics::_Internal::s_mainEGLSurface = eglSurface;
					Leggiero::Graphics::_Internal::s_mainEGLContext = eglContext;

					Leggiero::Graphics::_Internal::s_isMainContextInfoInitialzed = true;
				}
			}
		}
	}
}
