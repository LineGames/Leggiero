////////////////////////////////////////////////////////////////////////////////
// Rendering/UIClipping.h (Leggiero/Modules - LegacyUI)
//
// Interface for clipping in UI visual things
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_CLIPPING_H
#define __LM_LUI__RENDERING__UI_CLIPPING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <utility>
#include <vector>

// Leggiero.Graphics
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// UI Clipping Interface
		class IUIClipping
		{
		public:
			IUIClipping() { }
			virtual ~IUIClipping() { }

		public:
			virtual std::shared_ptr<const IUIClipping> Combine(const std::shared_ptr<const IUIClipping> &rhs) const = 0;

		public:
			virtual bool IsPointPass(UICoordinateType x, UICoordinateType y) const { return IsPointPass(UIVector2D(x, y)); }
			virtual bool IsPointPass(const UIVector2D &position) const = 0;

			virtual std::vector<Graphics::ColoredVertex> ClipPolygon(const std::vector<Graphics::ColoredVertex> &polygon) const = 0;
			virtual std::vector<Graphics::TextureColoredVertex> ClipPolygon(const std::vector<Graphics::TextureColoredVertex> &polygon) const = 0;
			virtual std::vector<Graphics::PositionVertex> ClipPolygon(const std::vector<Graphics::PositionVertex> &polygon) const = 0;

			virtual bool IsAllClipping() const { return false; }

		public:
			static const std::shared_ptr<const IUIClipping> kAlwaysPass;
			static const std::shared_ptr<const IUIClipping> kAlwaysReject;
		};


		// Convex Polygon Clipping in the global positioning coordinates
		class UIClippingConvexPolygon
			: public IUIClipping
			, public std::enable_shared_from_this<UIClippingConvexPolygon>
		{
		public:
			UIClippingConvexPolygon() : m_isValid(false) { }

			UIClippingConvexPolygon(std::vector<UIVector2D> initializingPolygon);
			UIClippingConvexPolygon(std::initializer_list<UIVector2D> initializingPolygon);

			template <typename InputIterator>
			UIClippingConvexPolygon(InputIterator first, InputIterator last)
				: m_points(first, last)
			{
				_InitializePolygon();
			}

			UIClippingConvexPolygon(const UIClippingConvexPolygon &other);
			UIClippingConvexPolygon(UIClippingConvexPolygon &&other);

			virtual ~UIClippingConvexPolygon() { }

		public:
			UIClippingConvexPolygon &operator=(const UIClippingConvexPolygon &rhs);
			UIClippingConvexPolygon &operator=(UIClippingConvexPolygon &&rhs);

		public:
			virtual std::shared_ptr<const IUIClipping> Combine(const std::shared_ptr<const IUIClipping> &rhs) const override;

		public:
			virtual bool IsPointPass(const UIVector2D &position) const override;

			virtual std::vector<Graphics::ColoredVertex> ClipPolygon(const std::vector<Graphics::ColoredVertex> &polygon) const override;
			virtual std::vector<Graphics::TextureColoredVertex> ClipPolygon(const std::vector<Graphics::TextureColoredVertex> &polygon) const override;
			virtual std::vector<Graphics::PositionVertex> ClipPolygon(const std::vector<Graphics::PositionVertex> &polygon) const override;

		public:
			bool IsClockwiseWinding() const { return m_isClockwiseWinding; }
			bool IsValid() const { return m_isValid; }

		public:
			// Access polygon vertex directly. Please use when winding and validity are consistent.
			std::vector<UIVector2D> &AccessRawPolygon() { return m_points; }
			void ForceUpdatePolygonStatus();

		protected:
			void _InitializePolygon();

		protected:
			std::vector<UIVector2D> m_points;

			bool m_isValid;
			bool m_isClockwiseWinding;
		};
	}
}

#endif
