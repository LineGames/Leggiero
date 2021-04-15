////////////////////////////////////////////////////////////////////////////////
// Rendering/UIRenderingUtility.h (Leggiero/Modules - LegacyUI)
//
// UI Rendering Utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_RENDERING_UTILITY_H
#define __LM_LUI__RENDERING__UI_RENDERING_UTILITY_H


// Standard Library
#include <memory>
#include <vector>

// External Library
#include <glm/glm.hpp>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "UIClipping.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace RenderingUtility
		{
			//------------------------------------------------------------------------------
			template <typename VertexT>
			inline void PrepareRenderPolygon(const std::vector<VertexT> &vertexPolygon, const IUIClipping &clipping, std::vector<VertexT> &vertexBuffer, std::vector<GLushort> &indexBuffer)
			{
				std::vector<VertexT> clippedPolygon(clipping.ClipPolygon(vertexPolygon));

				size_t startIndex = vertexBuffer.size();
				vertexBuffer.insert(vertexBuffer.end(), clippedPolygon.begin(), clippedPolygon.end());

				size_t vertexCount = clippedPolygon.size();
				for (size_t i = 2; i < vertexCount; ++i)
				{
					indexBuffer.push_back((GLushort)(startIndex));
					indexBuffer.push_back((GLushort)(startIndex + i - 1));
					indexBuffer.push_back((GLushort)(startIndex + i));
				}
			}

			//------------------------------------------------------------------------------
			inline void PrepareRenderTextureRect(UIVector2D lt, UIVector2D rt, UIVector2D lb, UIVector2D rb,
				const Graphics::TextureRectSection &textureSection, const Graphics::GLByteARGB &color,
				const IUIClipping &clipping,
				bool isFlipHorizontal, bool isFlipVertical, bool isGridRounding,
				std::vector<Graphics::TextureColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
			{
				float leftU = textureSection.GetTexelLeft();
				float rightU = textureSection.GetTexelRight();
				float topV = textureSection.GetTexelTop();
				float bottomV = textureSection.GetTexelBottom();

				if (isFlipHorizontal)
				{
					float tmp = leftU;
					leftU = rightU;
					rightU = tmp;
				}
				if (isFlipVertical)
				{
					float tmp = topV;
					topV = bottomV;
					bottomV = tmp;
				}

				if (isGridRounding)
				{
					if (fabs(lt.x - rt.x) < kFloatEpsilon && fabs(lb.x - rb.x) < kFloatEpsilon)
					{
						lt.x = round(lt.x);
						rt.x = round(rt.x);
						lb.x = round(lb.x);
						rb.x = round(rb.x);
					}
					else if (fabs(lt.x - lb.x) < kFloatEpsilon && fabs(rt.x - rb.x) < kFloatEpsilon)
					{
						lt.x = round(lt.x);
						rt.x = round(rt.x);
						lb.x = round(lb.x);
						rb.x = round(rb.x);
					}

					if (fabs(lt.y - rt.y) < kFloatEpsilon && fabs(lb.y - rb.y) < kFloatEpsilon)
					{
						lt.y = round(lt.y);
						rt.y = round(rt.y);
						lb.y = round(lb.y);
						rb.y = round(rb.y);
					}
					else if (fabs(lt.y - lb.y) < kFloatEpsilon && fabs(rt.y - rb.y) < kFloatEpsilon)
					{
						lt.y = round(lt.y);
						rt.y = round(rt.y);
						lb.y = round(lb.y);
						rb.y = round(rb.y);
					}
				}

				std::vector<Graphics::TextureColoredVertex> polygon({
					{ { (float)lt.x, (float)lt.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha },{ leftU, topV } },
					{ { (float)rt.x, (float)rt.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha },{ rightU, topV } },
					{ { (float)rb.x, (float)rb.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha },{ rightU, bottomV } },
					{ { (float)lb.x, (float)lb.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha },{ leftU, bottomV } }
					});

				PrepareRenderPolygon(polygon, clipping, vertexBuffer, indexBuffer);
			}

			//------------------------------------------------------------------------------
			inline void PrepareRenderColoredRect(UIVector2D lt, UIVector2D rt, UIVector2D lb, UIVector2D rb,
				const Graphics::GLByteARGB &color,
				const IUIClipping &clipping, bool isGridRounding,
				std::vector<Graphics::ColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
			{
				if (isGridRounding)
				{
					if (fabs(lt.x - rt.x) < kFloatEpsilon && fabs(lb.x - rb.x) < kFloatEpsilon)
					{
						lt.x = round(lt.x);
						rt.x = round(rt.x);
						lb.x = round(lb.x);
						rb.x = round(rb.x);
					}
					else if (fabs(lt.x - lb.x) < kFloatEpsilon && fabs(rt.x - rb.x) < kFloatEpsilon)
					{
						lt.x = round(lt.x);
						rt.x = round(rt.x);
						lb.x = round(lb.x);
						rb.x = round(rb.x);
					}

					if (fabs(lt.y - rt.y) < kFloatEpsilon && fabs(lb.y - rb.y) < kFloatEpsilon)
					{
						lt.y = round(lt.y);
						rt.y = round(rt.y);
						lb.y = round(lb.y);
						rb.y = round(rb.y);
					}
					else if (fabs(lt.y - lb.y) < kFloatEpsilon && fabs(rt.y - rb.y) < kFloatEpsilon)
					{
						lt.y = round(lt.y);
						rt.y = round(rt.y);
						lb.y = round(lb.y);
						rb.y = round(rb.y);
					}
				}

				std::vector<Graphics::ColoredVertex> polygon({
					{ { (float)lt.x, (float)lt.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha } },
					{ { (float)rt.x, (float)rt.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha } },
					{ { (float)rb.x, (float)rb.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha } },
					{ { (float)lb.x, (float)lb.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha } }
					});

				PrepareRenderPolygon(polygon, clipping, vertexBuffer, indexBuffer);
			}

			//------------------------------------------------------------------------------
			inline void PrepareRenderColoredPolygon(const std::vector<UIVector2D> &polygonVector,
				const Graphics::GLByteARGB &color,
				const IUIClipping &clipping, bool isGridRounding,
				std::vector<Graphics::ColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
			{
				std::vector<Graphics::ColoredVertex> polygon;
				polygon.reserve(polygonVector.size());

				Graphics::ColoredVertex tempVertex;
				for (const UIVector2D &point : polygonVector)
				{
					tempVertex = { { (float)point.x, (float)point.y, 0.0f, 1.0f },{ color.red, color.green, color.blue, color.alpha } };
					polygon.push_back(tempVertex);
				}

				PrepareRenderPolygon(polygon, clipping, vertexBuffer, indexBuffer);
			}
		}
	}
}

#endif
