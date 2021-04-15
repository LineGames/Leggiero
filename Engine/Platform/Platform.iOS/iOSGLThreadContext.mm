////////////////////////////////////////////////////////////////////////////////
// iOSGLThreadContext.mm (Leggiero - Platform.iOS)
//
// iOS Platform Graphics Thread Context Implementation
////////////////////////////////////////////////////////////////////////////////

// Reference Header
#include "iOSPlatformUtility.h"

// External Library
#import <Foundation/Foundation.h>
#import <MetalANGLE/MGLKit.h>

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
			static MGLContext *s_mainMGLContext;


			//////////////////////////////////////////////////////////////////////////////// IOSEGLThreadContextInformation

			// Thread Context Information for iOS EGL
			class IOSEGLThreadContextInformation : public IGLThreadContextInformation
			{
			public:
                IOSEGLThreadContextInformation()
					: mglContext(nullptr)
				{ }

				virtual ~IOSEGLThreadContextInformation()
                {
                    mglContext = nullptr;
                }

			public:
                MGLContext *mglContext;
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

            std::shared_ptr<_Internal::IOSEGLThreadContextInformation> info(std::make_shared<_Internal::IOSEGLThreadContextInformation>());
            
            info->mglContext = [[MGLContext alloc] initWithAPI:[_Internal::s_mainMGLContext API] sharegroup: [_Internal::s_mainMGLContext sharegroup]];
            if (info->mglContext == nil)
            {
                return nullptr;
            }
            
            [MGLContext setCurrentContext:info->mglContext];
            
            return info;
		}

		//------------------------------------------------------------------------------
		// Clean up called thread's OpenGL Context
		void TerminateThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo)
		{
            std::shared_ptr<_Internal::IOSEGLThreadContextInformation> concreteInfo(std::dynamic_pointer_cast<_Internal::IOSEGLThreadContextInformation>(contextInfo));

            [MGLContext setCurrentContext:nil];
            concreteInfo->mglContext = nil;
		}

		//------------------------------------------------------------------------------
		// Check called thread's OpenGL Context's validity and restore it if needed
		void CheckAndRefreshThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo)
		{
            // Nothing to do
		}
	}


	namespace Platform
	{
		namespace iOS
		{
			namespace Graphics
			{
				//////////////////////////////////////////////////////////////////////////////// iOS Platform OpenGL ES Stuffs

				//------------------------------------------------------------------------------
				// Notify Main EGL Context Information after Creation
				void SetMainGLContextInformation(id mainContext)
				{
					Leggiero::Graphics::_Internal::s_mainMGLContext = (MGLContext *)mainContext;
					Leggiero::Graphics::_Internal::s_isMainContextInfoInitialzed = true;
				}
			}
		}
	}
}
