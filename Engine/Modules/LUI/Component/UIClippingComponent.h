////////////////////////////////////////////////////////////////////////////////
// Component/UIClippingComponent.h (Leggiero/Modules - LegacyUI)
//
// Component to process clipping of an UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__UI_CLIPPING_COMPONENT_H
#define __LM_LUI__COMPONENT__UI_CLIPPING_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Rendering/UIClipping.h"
#include "IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class IUITransform;


		// Common Base Interface for Clipping Setting
		class IUIClippingComponent
			: public IUIComponent
		{
		public:
			IUIClippingComponent() : m_isClipping(false) { }
			virtual ~IUIClippingComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kClipping; }

		public:
			bool IsClipping() const { return m_isClipping; }
			void SetIsClipping(bool isClipping) { m_isClipping = isClipping; }

		public:
			// Get real clipping to be applied
			virtual std::shared_ptr<const IUIClipping> GetEffectiveClipping(const std::shared_ptr<const IUIClipping> &parentClipping, std::shared_ptr<IUITransform> transform) = 0;

		protected:
			bool m_isClipping;
		};


		// Transform appliable rectangular clipping
		class UIRectangularClipping
			: public IUIClippingComponent
		{
		public:
			UIRectangularClipping(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom);
			UIRectangularClipping(const UISimpleRectangular &clippingRect);
			virtual ~UIRectangularClipping() { }

		public:
			static std::shared_ptr<UIRectangularClipping> CreateByPositionSize(UICoordinateType left, UICoordinateType top, UICoordinateType width, UICoordinateType height);
			static std::shared_ptr<UIRectangularClipping> CreateByPositionSize(UIVector2D position, UICoordinateType width, UICoordinateType height);

		public:	// IUIComponent
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

		public:	// IUIClippingComponent
			// Get real clipping to be applied
			virtual std::shared_ptr<const IUIClipping> GetEffectiveClipping(const std::shared_ptr<const IUIClipping> &parentClipping, std::shared_ptr<IUITransform> transform) override;

		public:
			void SetRectangle(const UISimpleRectangular &rect) { m_clippingRectArea = rect; }
			void SetRectangle(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom);

			void SetLeft(UICoordinateType left) { m_clippingRectArea.left = left; }
			void SetTop(UICoordinateType top) { m_clippingRectArea.top = top; }
			void SetRight(UICoordinateType right) { m_clippingRectArea.right = right; }
			void SetBottom(UICoordinateType bottom) { m_clippingRectArea.bottom = bottom; }

			void SetWidth(UICoordinateType width) { m_clippingRectArea.right = m_clippingRectArea.left + width; }
			void SetHeight(UICoordinateType height) { m_clippingRectArea.bottom = m_clippingRectArea.top + height; }

			UISimpleRectangular GetRectangle() const { return m_clippingRectArea; }

		protected:
			UISimpleRectangular m_clippingRectArea;
			std::shared_ptr<UIClippingConvexPolygon> m_cachedClippingPolygon;
		};
	}
}

#endif
