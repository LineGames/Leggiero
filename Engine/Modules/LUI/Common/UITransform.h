////////////////////////////////////////////////////////////////////////////////
// Common/UITransform.h (Leggiero/Modules - LegacyUI)
//
// 2D Transform that can applied to UI things
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__UI_TRANSFORM_H
#define __LM_LUI__COMMON__UI_TRANSFORM_H


// Standard Library
#include <memory>

// External Library
#include <concurrentqueue/concurrentqueue.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// Leggiero.LegacyUI
#include "UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Common Interface for 2D Transform
		class IUITransform
			: public std::enable_shared_from_this<IUITransform>
		{
		public:
			virtual ~IUITransform() { }

		public:
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) = 0;
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) = 0;

		public:
			virtual std::shared_ptr<IUITransform> Combine(std::shared_ptr<IUITransform> rhs);

		public:
			virtual bool _IsMatrixTransform() const { return false; }
		};


		// Not-changing transform
		class IdentityTransform
			: public IUITransform
		{
		public:
			virtual ~IdentityTransform() { }

		public:
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) override { return UIVector2D(x, y); }
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) override { return UIVector2D(x, y); }

		public:
			virtual std::shared_ptr<IUITransform> Combine(std::shared_ptr<IUITransform> rhs) override;

		public:	// Pooled Transform Allocation
			static std::shared_ptr<IdentityTransform> AllocateTransform();
			static void ReleaseTransform(std::shared_ptr<IdentityTransform> transformToRelease);

		protected:
			static moodycamel::ConcurrentQueue<std::shared_ptr<IdentityTransform> > ms_transformPool;
		};


		// Offset transform
		class OffsetTransform
			: public IUITransform
		{
		public:
			OffsetTransform(UICoordinateType xOffset, UICoordinateType yOffset)
				: m_xOffset(xOffset), m_yOffset(yOffset)
			{ }
			virtual ~OffsetTransform() { }

		public:
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) override { return UIVector2D(x + m_xOffset, y + m_yOffset); }
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) override { return UIVector2D(x - m_xOffset, y - m_yOffset); }

		public:
			virtual std::shared_ptr<IUITransform> Combine(std::shared_ptr<IUITransform> rhs) override;

		public:
			UIVector2D GetOffset() const { return UIVector2D((UICoordinateType)m_xOffset, (UICoordinateType)m_yOffset); }
			UICoordinateType GetOffsetX() const { return (UICoordinateType)m_xOffset; }
			UICoordinateType GetOffsetY() const { return (UICoordinateType)m_yOffset; }
			void SetOffset(UICoordinateType xOffset, UICoordinateType yOffset) { m_xOffset = xOffset; m_yOffset = yOffset; }
			void SetOffsetX(UICoordinateType xOffset) { m_xOffset = xOffset; }
			void SetOffsetY(UICoordinateType yOffset) { m_yOffset = yOffset; }

		protected:
			UICoordinateType m_xOffset;
			UICoordinateType m_yOffset;

		public:	// Pooled Transform Allocation
			static std::shared_ptr<OffsetTransform> AllocateTransform(UICoordinateType xOffset, UICoordinateType yOffset);
			static void ReleaseTransform(std::shared_ptr<OffsetTransform> transformToRelease);

		protected:
			static moodycamel::ConcurrentQueue<std::shared_ptr<OffsetTransform> > ms_transformPool;
		};


		// Combine 2 transforms
		class UICombinedTransform
			: public IUITransform
		{
		public:
			UICombinedTransform(std::shared_ptr<IUITransform> pre, std::shared_ptr<IUITransform> post);
			virtual ~UICombinedTransform() { }

		public:
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) override;
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) override;

		public:
			std::shared_ptr<IUITransform> GetPre() { return m_pre; }
			std::shared_ptr<IUITransform> GetPost() { return m_post; }

		protected:
			std::shared_ptr<IUITransform> m_pre;
			std::shared_ptr<IUITransform> m_post;

		public:	// Pooled Transform Allocation
			static std::shared_ptr<UICombinedTransform> AllocateTransform(std::shared_ptr<IUITransform> pre, std::shared_ptr<IUITransform> post);
			static void ReleaseTransform(std::shared_ptr<UICombinedTransform> transformToRelease);

		protected:
			static moodycamel::ConcurrentQueue<std::shared_ptr<UICombinedTransform> > ms_transformPool;
		};


		// Matrix based transform
		class UITransform
			: public IUITransform
		{
		public:
			UITransform();
			UITransform(UITransform &rhs);
			UITransform(UITransform &&rhs);
			virtual ~UITransform();

		public:	// IUITransform
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) override;
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) override;

			virtual std::shared_ptr<IUITransform> Combine(std::shared_ptr<IUITransform> rhs) override;

		public:
			UITransform operator*(UITransform &rhs);
			UITransform &operator=(UITransform &rhs);
			UITransform &operator*=(UITransform &rhs);
			UITransform operator*(UITransform &&rhs);
			UITransform &operator=(UITransform &&rhs);
			UITransform &operator*=(UITransform &&rhs);

		protected:
			virtual void _UpdateTransform();
			virtual void _UpdateInverseTransform();

		public:	// IUITransform
			virtual bool _IsMatrixTransform() const override { return true; }

		protected:
			bool		m_isTransformDirty;
			glm::mat3	m_transform;

			bool		m_isInverseDirty;
			glm::mat3	m_inverseTransform;
		};


		// Controllable parameter based UI transform
		class UIParameterTransform
			: public UITransform
		{
		public:
			UIParameterTransform();
			virtual ~UIParameterTransform();

		protected:	// UITransform
			virtual void _UpdateTransform() override;

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

			UIVector2D GetRoughPerspective() const { return UIVector2D((UICoordinateType)m_roughPerspective.x, (UICoordinateType)m_roughPerspective.y); }
			void SetRoughPerspective(UICoordinateType x, UICoordinateType y) { if (m_roughPerspective.x != (float)x || m_roughPerspective.y != (float)y) { m_roughPerspective.x = (float)x; m_roughPerspective.y = (float)y; _Invalidate(); } }

		protected:
			void _Invalidate() { m_isTransformDirty = true; m_isInverseDirty = true; }

		protected:
			glm::vec2	m_translate;
			glm::vec2	m_anchor;
			float		m_rotate;
			glm::vec2	m_scale;
			glm::vec2	m_skew;

			// Rough pseudo-perspective transform
			// for optimization purpose. please use UIPerspectiveTransform or direct calculation to get accurate result
			glm::vec2	m_roughPerspective;
		};


		// 2D Perspective Transform
		class UIPerspectiveTransform
			: public IUITransform
		{
		public:
			UIPerspectiveTransform();
			UIPerspectiveTransform(const UIPerspectiveTransform &rhs);
			virtual ~UIPerspectiveTransform();

		public:	// IUITransform
			virtual UIVector2D ApplyTransform(UICoordinateType x, UICoordinateType y) override;
			virtual UIVector2D ApplyInverseTransform(UICoordinateType x, UICoordinateType y) override;

		public:
			UIPerspectiveTransform &operator=(const UIPerspectiveTransform &rhs);

		public:
			UIVector2D GetPerspective() const { return UIVector2D((UICoordinateType)m_perspective.x, (UICoordinateType)m_perspective.y); }
			UICoordinateType GetPerspectiveX() const { return (UICoordinateType)m_perspective.x; }
			UICoordinateType GetPerspectiveY() const { return (UICoordinateType)m_perspective.y; }
			void SetPerspective(UICoordinateType x, UICoordinateType y) { m_perspective.x = (float)x; m_perspective.y = (float)y; }
			void SetPerspectiveX(UICoordinateType x) { m_perspective.x = (float)x; }
			void SetPerspectiveY(UICoordinateType y) { m_perspective.y = (float)y; }

			UIVector2D GetPerspectiveFocus() const;
			UICoordinateType GetPerspectiveFocusX() const;
			UICoordinateType GetPerspectiveFocusY() const;
			void SetPerspectiveFocus(UICoordinateType x, UICoordinateType y);

			UIVector2D GetAnchor() const { return UIVector2D((UICoordinateType)m_anchor.x, (UICoordinateType)m_anchor.y); }
			UICoordinateType GetAnchorX() const { return (UICoordinateType)m_anchor.x; }
			UICoordinateType GetAnchorY() const { return (UICoordinateType)m_anchor.y; }
			void SetAnchor(UICoordinateType x, UICoordinateType y) { m_anchor.x = (float)x; m_anchor.y = (float)y; }
			void SetAnchorX(UICoordinateType x) { m_anchor.x = (float)x; }
			void SetAnchorY(UICoordinateType y) { m_anchor.y = (float)y; }

		protected:
			glm::vec2	m_perspective;
			glm::vec2	m_anchor;
		};
	}
}

#endif
