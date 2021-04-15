////////////////////////////////////////////////////////////////////////////////
// Rendering/UIShapeRect.h (Leggiero/Modules - LegacyUI)
//
// UI Renderable Rectangular Shape
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_SHAPE_RECT_H
#define __LM_LUI__RENDERING__UI_SHAPE_RECT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Common/UITransform.h"
#include "../Common/UISizeSettable.h"
#include "IUIRenderingShape.h"
#include "UIClipping.h"


namespace Leggiero
{
	namespace LUI
	{
		// UI Rectangular Shape
		class UIShapeRect
			: public IUIRenderingShape
			, public IRectSizeSettable
		{
		public:
			UIShapeRect(UICoordinateType width, UICoordinateType height, const Graphics::GLByteARGB &color,
				UICoordinateType borderWidth = static_cast<UICoordinateType>(0.0f), const Graphics::GLByteARGB &borderColor = Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType borderAlign = ShapeBorderAlignType::kCenter,
				UICoordinateType offsetX = static_cast<UICoordinateType>(0.0f), UICoordinateType offsetY = static_cast<UICoordinateType>(0.0f));
			virtual ~UIShapeRect();

		public:	// IUIRenderingShape
			virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

			virtual UISimpleRectangular GetBoundingBox() override;

		public:
			void SetColor(const Graphics::GLByteARGB &color) { m_color = color; _UpdatePreCalculatedValues(); }

			void SetSize(UICoordinateType width, UICoordinateType height) { m_width = width; m_height = height; _UpdatePreCalculatedValues(); }
			virtual void SetUIWidgetRectSize(UICoordinateType width, UICoordinateType height) override { SetSize(width, height); }
			void SetOffset(UICoordinateType offsetX, UICoordinateType offsetY) { m_offsetX = offsetX; m_offsetY = offsetY; _UpdatePreCalculatedValues(); }

			void SetBorderWidth(UICoordinateType borderWidth) { m_borderWidth = borderWidth; _UpdatePreCalculatedValues(); }
			void SetBorderColor(const Graphics::GLByteARGB &color) { m_borderColor = color; _UpdatePreCalculatedValues(); }

			UICoordinateType GetWidth() const { return m_width; }
			UICoordinateType GetHeight() const { return m_height; }

		protected:
			virtual void _UpdatePreCalculatedValues();

		protected:
			bool m_hasRect;
			UICoordinateType m_offsetX;
			UICoordinateType m_offsetY;
			UICoordinateType m_width;
			UICoordinateType m_height;
			Graphics::GLByteARGB m_color;

			bool m_hasBorder;
			UICoordinateType m_borderWidth;
			Graphics::GLByteARGB m_borderColor;
			ShapeBorderAlignType m_borderAlign;
			UICoordinateType m_borderOffset;
			UICoordinateType m_borderOffsetDouble;

			std::vector<Graphics::ColoredVertex>	m_vertexBuffer;
			std::vector<GLushort>					m_indexBuffer;
		};
	}
}

#endif
