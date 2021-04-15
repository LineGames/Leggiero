////////////////////////////////////////////////////////////////////////////////
// Common/GraphicsReferenceState.cpp (Leggiero/Modules - Graphics)
//
// Implementation related to Graphics Module's Reference States
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicsReferenceState.h"

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IDeviceInformationProvider.h>

// Leggiero.Graphics
#include "../_Internal/_InternalInitializer.h"
#include "../_Internal/_InternalUpdater.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace _Internal
		{
			//////////////////////////////////////////////////////////////////////////////// State Variables

			namespace ReferenceState
			{
				static GameTimeClockType::time_point s_frameRenderingStartTime;

				static GLuint s_defaultFrameBufferObjectName;

				static GLint s_viewportX;
				static GLint s_viewportY;
				static GLsizei s_viewportWidth;
				static GLsizei s_viewportHeight;

				static BasicCoordType s_approximateScreenXPPI;
				static BasicCoordType s_approximateScreenYPPI;

				static GLint s_maxTextureSize;
			}


			//////////////////////////////////////////////////////////////////////////////// Initializers

			//------------------------------------------------------------------------------
			void _InitializeReferenceState()
			{
				_Internal::ReferenceState::s_frameRenderingStartTime = GameTimeClockType::time_point();

				_Internal::ReferenceState::s_defaultFrameBufferObjectName = 0;

				_Internal::ReferenceState::s_viewportX = 0;
				_Internal::ReferenceState::s_viewportY = 0;
				_Internal::ReferenceState::s_viewportWidth = 0;
				_Internal::ReferenceState::s_viewportHeight = 0;

				_Internal::ReferenceState::s_approximateScreenXPPI = static_cast<BasicCoordType>(0.0f);
				_Internal::ReferenceState::s_approximateScreenYPPI = static_cast<BasicCoordType>(0.0f);

				_Internal::ReferenceState::s_maxTextureSize = static_cast<GLint>(-1);
			}

			//------------------------------------------------------------------------------
			void _InitializeReferenceStateInGraphicThread()
			{
				GLint maxTextureSize;
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
				_Internal::ReferenceState::s_maxTextureSize = maxTextureSize;
			}

			//------------------------------------------------------------------------------
			void _InitializeApplicationReferenceState(Application::ApplicationComponent *appComponent)
			{
				_Internal::ReferenceState::s_approximateScreenXPPI = static_cast<BasicCoordType>(appComponent->DeviceInformation().GetApproximateScreenXPPI());
				_Internal::ReferenceState::s_approximateScreenYPPI = static_cast<BasicCoordType>(appComponent->DeviceInformation().GetApproximateScreenYPPI());
			}

			//------------------------------------------------------------------------------
			void _FinalizeReferenceState()
			{
			}
		}


		namespace ReferenceState
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// Reference States Updater

				//------------------------------------------------------------------------------
				// Update Reference State at the start of Each Frame
				void _UpdateReferenceStateAtFrame()
				{
					Graphics::_Internal::ReferenceState::s_frameRenderingStartTime = GameTimeClockType::now();

					GLint currentBindedFrameBuffer;
					glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBindedFrameBuffer);
					Graphics::_Internal::ReferenceState::s_defaultFrameBufferObjectName = (GLuint)currentBindedFrameBuffer;

					GLint currentViewport[4];
					glGetIntegerv(GL_VIEWPORT, currentViewport);
					Graphics::_Internal::ReferenceState::s_viewportX = (GLint)currentViewport[0];
					Graphics::_Internal::ReferenceState::s_viewportY = (GLint)currentViewport[1];
					Graphics::_Internal::ReferenceState::s_viewportWidth = (GLsizei)currentViewport[2];
					Graphics::_Internal::ReferenceState::s_viewportHeight = (GLsizei)currentViewport[3];
				}
			}


			//////////////////////////////////////////////////////////////////////////////// Reference States

			//------------------------------------------------------------------------------
			// Get frame rendering start time
			GameTimeClockType::time_point GetFrameRenderingStartTime()
			{
				return Graphics::_Internal::ReferenceState::s_frameRenderingStartTime;
			}

			//------------------------------------------------------------------------------
			// Get main frame buffer
			GLuint GetReferenceFrameBufferObjectName()
			{
				return Graphics::_Internal::ReferenceState::s_defaultFrameBufferObjectName;
			}

			//------------------------------------------------------------------------------
			GLint GetReferenceViewportX()
			{
				return Graphics::_Internal::ReferenceState::s_viewportX;
			}

			//------------------------------------------------------------------------------
			GLint GetReferenceViewportY()
			{
				return Graphics::_Internal::ReferenceState::s_viewportY;
			}

			//------------------------------------------------------------------------------
			GLsizei GetReferenceViewportWidth()
			{
				return Graphics::_Internal::ReferenceState::s_viewportWidth;
			}

			//------------------------------------------------------------------------------
			GLsizei GetReferenceViewportHeight()
			{
				return Graphics::_Internal::ReferenceState::s_viewportHeight;
			}

			//------------------------------------------------------------------------------
			BasicCoordType GetApproxScreenXPPI()
			{
				return Graphics::_Internal::ReferenceState::s_approximateScreenXPPI;
			}

			//------------------------------------------------------------------------------
			BasicCoordType GetApproxScreenYPPI()
			{
				return Graphics::_Internal::ReferenceState::s_approximateScreenYPPI;
			}

			//------------------------------------------------------------------------------
			bool IsWholeScreenTextureAvailable()
			{
				return ((static_cast<GLsizei>(Graphics::_Internal::ReferenceState::s_maxTextureSize) >= Graphics::_Internal::ReferenceState::s_viewportWidth) &&
					(static_cast<GLsizei>(Graphics::_Internal::ReferenceState::s_maxTextureSize) >= Graphics::_Internal::ReferenceState::s_viewportHeight));
			}

			//------------------------------------------------------------------------------
			GLint GetMaximumTextureDimension()
			{
				return Graphics::_Internal::ReferenceState::s_maxTextureSize;
			}
		}
	}
}
