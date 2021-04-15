////////////////////////////////////////////////////////////////////////////////
// Common/UICommonTypes.h (Leggiero/Modules - LegacyUI)
//
// Common Type Definitions for Legacy UI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__UI_COMMON_TYPES_H
#define __LM_LUI__COMMON__UI_COMMON_TYPES_H


// Standard Library
#include <cstdint>
#include <string>

// Leggiero.Utility
#include <Utility/Math/Vector.h>


namespace Leggiero
{
	namespace LUI
	{
		// Unique Id of UI Object
		using UIObjectIdType = int64_t;
		constexpr UIObjectIdType kInvalidObjectId = (UIObjectIdType)(~(UIObjectIdType)0);

		// UI Coordinate Type
		using UICoordinateType = float;
		using UICoordinateRatioType = float;

		constexpr UICoordinateRatioType kUICoordinateRatio_Zero = static_cast<Leggiero::LUI::UICoordinateRatioType>(0.0f);
		constexpr UICoordinateRatioType kUICoordinateRatio_Half = static_cast<Leggiero::LUI::UICoordinateRatioType>(0.5f);
		constexpr UICoordinateRatioType kUICoordinateRatio_One = static_cast<Leggiero::LUI::UICoordinateRatioType>(1.0f);

		using UIVector2D = Utility::Math::Vector2D<UICoordinateType>;


		// Size type for UI Elements
		struct UIElementSize
		{
		public:
			UICoordinateType width;
			UICoordinateType height;

		public:
			UIElementSize()
				: width(static_cast<UICoordinateType>(0.0f)), height(static_cast<UICoordinateType>(0.0f))
			{ }

			UIElementSize(UICoordinateType width, UICoordinateType height)
				: width(width), height(height)
			{ }
		};


		// Axis-parallel Rectangular Type
		struct UISimpleRectangular
		{
		public:
			UICoordinateType left;
			UICoordinateType top;
			UICoordinateType right;
			UICoordinateType bottom;

		public:
			UICoordinateType Width() const { return right - left; }
			UICoordinateType Height() const { return bottom - top; }

		public:
			UISimpleRectangular(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom)
				: left(left), top(top), right(right), bottom(bottom)
			{ }

			static UISimpleRectangular FromPositionSize(UICoordinateType x, UICoordinateType y, UICoordinateType width, UICoordinateType height)
			{
				return UISimpleRectangular(x, y, x + width, y + height);
			}
		};


		// Border Alignment Type for Line
		enum class ShapeBorderAlignType
		{
			kInside,
			kCenter,
			kOutside,
		};


		// Common Epsilon Value
		constexpr float kFloatEpsilon = 0.000001f;


		// UI Structural Version Number Type
		using StructuralVersionType = uint64_t;


		// Names
		using UITextureNameType = std::string;
		using UIFontNameType = std::string;
	}
}

#endif
