////////////////////////////////////////////////////////////////////////////////
// Component/UITransformComponent.h (Leggiero/Modules - LegacyUI)
//
// Component for transform applied to an UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__UI_TRANSFORM_COMPONENT_H
#define __LM_LUI__COMPONENT__UI_TRANSFORM_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Common/UITransform.h"
#include "IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Base interface for transform component
		class IUITransformComponent
			: public IUIComponent
		{
		public:
			virtual ~IUITransformComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kTransform; }

		public:
			// Get real transform to be applied
			virtual std::shared_ptr<IUITransform> GetEffectiveTransform() = 0;
		};


		// Fixed Transform
		class UIFixedTransformComponent
			: public IUITransformComponent
		{
		public:
			UIFixedTransformComponent(std::shared_ptr<IUITransform> baseTransform);
			virtual ~UIFixedTransformComponent() { }
			
		public:	// IUIComponent
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

		public:	// IUITransformComponent
			// Get real transform to be applied
			virtual std::shared_ptr<IUITransform> GetEffectiveTransform() override;

		public:
			void SetTransform(std::shared_ptr<IUITransform> transform) { m_baseTransform = transform; }

		protected:
			std::shared_ptr<IUITransform> m_baseTransform;
		};


		// Controllable parameter based 2D transform component
		class UIParametericTransformComponent
			: public IUITransformComponent
		{
		public:
			UIParametericTransformComponent();
			virtual ~UIParametericTransformComponent() { }

		public:	// IUIComponent
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

		public:	// IUITransformComponent
			// Get real transform to be applied
			virtual std::shared_ptr<IUITransform> GetEffectiveTransform() override;

		public:
			UIVector2D GetTranslate() const { return UIVector2D((UICoordinateType)m_translate.x, (UICoordinateType)m_translate.y); }
			UICoordinateType GetTranslateX() const { return (UICoordinateType)m_translate.x; }
			UICoordinateType GetTranslateY() const { return (UICoordinateType)m_translate.y; }
			void SetTranslate(UICoordinateType x, UICoordinateType y) { if (m_translate.x != (float)x || m_translate.y != (float)y) { m_translate.x = (float)x; m_translate.y = (float)y; _Invalidate(); } }
			void SetTranslateX(UICoordinateType x) { if (m_translate.x != (float)x) { m_translate.x = (float)x; _Invalidate(); } }
			void SetTranslateY(UICoordinateType y) { if (m_translate.y != (float)y) { m_translate.y = (float)y; _Invalidate(); } }

			UIVector2D GetAnchor() const { return UIVector2D((UICoordinateType)m_anchor.x, (UICoordinateType)m_anchor.y); }
			UICoordinateType GetAnchorX() const { return (UICoordinateType)m_anchor.x; }
			UICoordinateType GetAnchorY() const { return (UICoordinateType)m_anchor.y; }
			void SetAnchor(UICoordinateType x, UICoordinateType y) { if (m_anchor.x != (float)x || m_anchor.y != (float)y) { m_anchor.x = (float)x; m_anchor.y = (float)y; _Invalidate(); } }
			void SetAnchorX(UICoordinateType x) { if (m_anchor.x != (float)x) { m_anchor.x = (float)x; _Invalidate(); } }
			void SetAnchorY(UICoordinateType y) { if (m_anchor.y != (float)y) { m_anchor.y = (float)y; _Invalidate(); } }

			float GetRotate() const { return m_rotate; }
			void SetRotate(float rotation) { if (m_rotate != rotation) { m_rotate = rotation; _Invalidate(); } }
			void SetRotateInDegree(float rotationInDeg) { float rotationInRad = rotationInDeg * 0.017453278f; if (m_rotate != rotationInRad) { m_rotate = rotationInRad; _Invalidate(); } }

			UIVector2D GetScale() const { return UIVector2D((UICoordinateType)m_scale.x, (UICoordinateType)m_scale.y); }
			UICoordinateType GetScaleX() const { return (UICoordinateType)m_scale.x; }
			UICoordinateType GetScaleY() const { return (UICoordinateType)m_scale.y; }
			void SetScale(UICoordinateType x, UICoordinateType y) { if (m_scale.x != (float)x || m_scale.y != (float)y) { m_scale.x = (float)x; m_scale.y = (float)y; _Invalidate(); } }
			void SetScaleX(UICoordinateType x) { if (m_scale.x != (float)x) { m_scale.x = (float)x; _Invalidate(); } }
			void SetScaleY(UICoordinateType y) { if (m_scale.y != (float)y) { m_scale.y = (float)y; _Invalidate(); } }

			UIVector2D GetSkew() const { return UIVector2D((UICoordinateType)m_skew.x, (UICoordinateType)m_skew.y); }
			UICoordinateType GetSkewX() const { return (UICoordinateType)m_skew.x; }
			UICoordinateType GetSkewY() const { return (UICoordinateType)m_skew.y; }
			void SetSkew(UICoordinateType x, UICoordinateType y) { if (m_skew.x != (float)x || m_skew.y != (float)y) { m_skew.x = (float)x; m_skew.y = (float)y; _Invalidate(); } }
			void SetSkewX(UICoordinateType x) { if (m_skew.x != (float)x) { m_skew.x = (float)x; _Invalidate(); } }
			void SetSkewY(UICoordinateType y) { if (m_skew.y != (float)y) { m_skew.y = (float)y; _Invalidate(); } }

			UIVector2D GetPerspective() const { return UIVector2D((UICoordinateType)m_perspective.x, (UICoordinateType)m_perspective.y); }
			UICoordinateType GetPerspectiveX() const { return (UICoordinateType)m_perspective.x; }
			UICoordinateType GetPerspectiveY() const { return (UICoordinateType)m_perspective.y; }
			void SetPerspective(UICoordinateType x, UICoordinateType y) { if (m_perspective.x != (float)x || m_perspective.y != (float)y) { m_perspective.x = (float)x; m_perspective.y = (float)y; _Invalidate(); } }
			void SetPerspectiveX(UICoordinateType x) { if (m_perspective.x != (float)x) { m_perspective.x = (float)x; _Invalidate(); } }
			void SetPerspectiveY(UICoordinateType y) { if (m_perspective.y != (float)y) { m_perspective.y = (float)y; _Invalidate(); } }

			UIVector2D GetPerspectiveFocus() const;
			UICoordinateType GetPerspectiveFocusX() const;
			UICoordinateType GetPerspectiveFocusY() const;
			void SetPerspectiveFocus(UICoordinateType x, UICoordinateType y);

		protected:
			void _Invalidate() { m_isTransformDirty = true; }
			void _BakeCachedTransform();

		protected:
			bool		m_isTransformDirty;

			UIVector2D	m_translate;
			UIVector2D	m_anchor;
			float		m_rotate;
			UIVector2D	m_scale;
			UIVector2D	m_skew;
			UIVector2D	m_perspective;

			std::shared_ptr<IUITransform> m_bakedTransform;
		};
	}
}

#endif
