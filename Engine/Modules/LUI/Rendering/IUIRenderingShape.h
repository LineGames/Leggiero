////////////////////////////////////////////////////////////////////////////////
// Rendering/IUIRenderingShape.h (Leggiero/Modules - LegacyUI)
//
// Interface for basic shapes can be rendered in UI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__I_UI_RENDERING_SHAPE_H
#define __LM_LUI__RENDERING__I_UI_RENDERING_SHAPE_H


// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIRenderer;
		class IUITransform;
		class IUIClipping;


		// Interface for UI Shapes
		class IUIRenderingShape
		{
		public:
			IUIRenderingShape() { }
			virtual ~IUIRenderingShape() { }

		public:
			virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) = 0;

			virtual UISimpleRectangular GetBoundingBox() = 0;
		};
	}
}

#endif
