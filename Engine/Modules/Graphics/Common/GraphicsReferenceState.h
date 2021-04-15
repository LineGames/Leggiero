////////////////////////////////////////////////////////////////////////////////
// Common/GraphicsReferenceState.h (Leggiero/Modules - Graphics)
//
// Shared Reference States of Graphics Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__COMMON__GRAPHICS_REFERENCE_STATE_H
#define __LM_GRAPHICS__COMMON__GRAPHICS_REFERENCE_STATE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "GraphicsTypes.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace ReferenceState
		{
			// Get frame rendering start time
			GameTimeClockType::time_point GetFrameRenderingStartTime();

			// Get main frame buffer
			GLuint GetReferenceFrameBufferObjectName();

			// Get vieport related values
			GLint GetReferenceViewportX();
			GLint GetReferenceViewportY();
			GLsizei GetReferenceViewportWidth();
			GLsizei GetReferenceViewportHeight();

			// Get screen related values
			BasicCoordType GetApproxScreenXPPI();
			BasicCoordType GetApproxScreenYPPI();

			// Get texture related values
			bool IsWholeScreenTextureAvailable();
			GLint GetMaximumTextureDimension();
		}
	}
}

#endif
