////////////////////////////////////////////////////////////////////////////////
// Common/GraphicsTypes.h (Leggiero/Modules - Graphics)
//
// Common Type Definitions for OpenGL Graphics
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__COMMON__GRAPHICS_TYPES_H
#define __LM_GRAPHICS__COMMON__GRAPHICS_TYPES_H


// Leggiero.Utility
#include <Utility/Math/BasicRect.h>
#include <Utility/Math/Vector.h>


namespace Leggiero
{
	namespace Graphics
	{
		using BasicCoordType = float;

		using CoordVector2D = Utility::Math::Vector2D<BasicCoordType>;
		using CoordVector3D = Utility::Math::Vector3D<BasicCoordType>;

		using CoordBasicRect = Utility::Math::BasicRect<BasicCoordType>;


		// Type for Horizontal Align
		enum class HorizontalAlignType
		{
			kNone, 
			kLeft,
			kCenter,
			kRight,
		};

		// Type for Vertical Align
		enum class VerticalAlignType
		{
			kNone, 
			kTop,
			kCenter,
			kBottom,
		};
	}
}

#endif
