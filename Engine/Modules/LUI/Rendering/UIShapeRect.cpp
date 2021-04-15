////////////////////////////////////////////////////////////////////////////////
// Rendering/UIShapeRect.cpp (Leggiero/Modules - LegacyUI)
//
// UI Rectangular Shape Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIShapeRect.h"

// Standard Library
#include <vector>

// Leggiero.LegacyUI
#include "UIShaders.h"
#include "UIRenderer.h"
#include "UIRenderingUtility.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// Utility

		namespace _UIShapeRectUtil
		{
			inline void PrepareRenderClippedRect(UISimpleRectangular &renderingRect, IUITransform &effectiveTransform,
				const Graphics::GLByteARGB &color, const IUIClipping &effectiveClipping, std::vector<Graphics::ColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
			{
				RenderingUtility::PrepareRenderColoredRect(
					effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.top),
					effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.top),
					effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.bottom),
					effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.bottom),
					color, effectiveClipping, false, vertexBuffer, indexBuffer);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// UIShapeRect

		//------------------------------------------------------------------------------
		UIShapeRect::UIShapeRect(UICoordinateType width, UICoordinateType height, const Graphics::GLByteARGB &color,
			UICoordinateType borderWidth, const Graphics::GLByteARGB &borderColor, ShapeBorderAlignType borderAlign,
			UICoordinateType offsetX, UICoordinateType offsetY)
			: m_offsetX(offsetX), m_offsetY(offsetY), m_width(width), m_height(height), m_color(color)
			, m_borderWidth(borderWidth), m_borderColor(borderColor), m_borderAlign(borderAlign), m_borderOffset(static_cast<UICoordinateType>(0.0f)), m_borderOffsetDouble(static_cast<UICoordinateType>(0.0f))
		{
			_UpdatePreCalculatedValues();
		}

		//------------------------------------------------------------------------------
		UIShapeRect::~UIShapeRect()
		{

		}

		//------------------------------------------------------------------------------
		// Do Rendering of a frame
		void UIShapeRect::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
		{
			if (alpha <= 0.0f)
			{
				return;
			}

			if (m_hasRect)
			{
				Graphics::GLByteARGB effectiveRectColor(m_color);
				if (alpha < 1.0f)
				{
					effectiveRectColor.alpha = static_cast<GLubyte>((float)effectiveRectColor.alpha * alpha);
				}

				UISimpleRectangular effectiveRectArea = UISimpleRectangular::FromPositionSize(m_offsetX + m_borderOffset, m_offsetY + m_borderOffset, m_width - m_borderOffsetDouble, m_height - m_borderOffsetDouble);
				_UIShapeRectUtil::PrepareRenderClippedRect(effectiveRectArea, effectiveTransform, effectiveRectColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
			}
			if (m_hasBorder)
			{
				Graphics::GLByteARGB effectiveBorderColor(m_borderColor);
				if (alpha < 1.0f)
				{
					effectiveBorderColor.alpha = static_cast<GLubyte>((float)effectiveBorderColor.alpha * alpha);
				}

				UISimpleRectangular offsetedRect = UISimpleRectangular::FromPositionSize(m_offsetX, m_offsetY, m_width, m_height);
				UISimpleRectangular leftLineRect = UISimpleRectangular::FromPositionSize(offsetedRect.left + m_borderOffset - m_borderWidth, offsetedRect.top + m_borderOffset - m_borderWidth, m_borderWidth, m_height + m_borderWidth - m_borderOffsetDouble);
				UISimpleRectangular topLineRect = UISimpleRectangular::FromPositionSize(offsetedRect.left + m_borderOffset, offsetedRect.top + m_borderOffset - m_borderWidth, m_width + m_borderWidth - m_borderOffsetDouble, m_borderWidth);
				UISimpleRectangular rightLineRect = UISimpleRectangular::FromPositionSize(offsetedRect.right - m_borderOffset, offsetedRect.top + m_borderOffset, m_borderWidth, m_height + m_borderWidth - m_borderOffsetDouble);
				UISimpleRectangular bottomLineRect = UISimpleRectangular::FromPositionSize(offsetedRect.left + m_borderOffset - m_borderWidth, offsetedRect.bottom - m_borderOffset, m_width + m_borderWidth - m_borderOffsetDouble, m_borderWidth);
				_UIShapeRectUtil::PrepareRenderClippedRect(leftLineRect, effectiveTransform, effectiveBorderColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
				_UIShapeRectUtil::PrepareRenderClippedRect(topLineRect, effectiveTransform, effectiveBorderColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
				_UIShapeRectUtil::PrepareRenderClippedRect(rightLineRect, effectiveTransform, effectiveBorderColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
				_UIShapeRectUtil::PrepareRenderClippedRect(bottomLineRect, effectiveTransform, effectiveBorderColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
			}

			if (m_indexBuffer.size() > 3)
			{
				//note: can be optimized by batch rendering by the renderer
				std::shared_ptr<Shaders::UIColorPrimitiveShader> shader = renderer.GetColorSimpleShader();
				shader->Use();

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
				Graphics::ColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), &m_vertexBuffer[0]);
				glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
			}
			m_vertexBuffer.clear();
			m_indexBuffer.clear();
		}

		//------------------------------------------------------------------------------
		// Calculate bounding box of the shape
		UISimpleRectangular UIShapeRect::GetBoundingBox()
		{
			if (m_hasRect)
			{
				UISimpleRectangular effectiveRectArea = UISimpleRectangular::FromPositionSize(m_offsetX + m_borderOffset, m_offsetY + m_borderOffset, m_width - m_borderOffsetDouble, m_height - m_borderOffsetDouble);
				return effectiveRectArea;
			}

			return UISimpleRectangular(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f));
		}

		//------------------------------------------------------------------------------
		// Update common pre-calculated values
		void UIShapeRect::_UpdatePreCalculatedValues()
		{
			m_hasRect = (m_color.alpha > 0 && m_width > kFloatEpsilon && m_height > kFloatEpsilon);
			m_hasBorder = (m_borderWidth > kFloatEpsilon && m_borderColor.alpha > 0);
			if (m_hasBorder)
			{
				if (m_borderAlign == ShapeBorderAlignType::kCenter)
				{
					m_borderOffset = static_cast<UICoordinateType>(m_borderWidth * 0.5f);
					m_borderOffsetDouble = m_borderWidth;
				}
				else if (m_borderAlign == ShapeBorderAlignType::kInside)
				{
					m_borderOffset = m_borderWidth;
					m_borderOffsetDouble = static_cast<UICoordinateType>(m_borderWidth * 2.0f);
				}
			}

			if (m_hasRect && m_hasBorder)
			{
				if (m_borderAlign == ShapeBorderAlignType::kCenter)
				{
					if (m_width <= m_borderWidth || m_height <= m_borderWidth)
					{
						m_hasRect = false;
					}
				}
				else if (m_borderAlign == ShapeBorderAlignType::kInside)
				{
					if (m_width <= m_borderWidth * static_cast<UICoordinateType>(2.0f) || m_height <= m_borderWidth * static_cast<UICoordinateType>(2.0f))
					{
						m_hasRect = false;
					}
				}
			}
		}
	}
}
