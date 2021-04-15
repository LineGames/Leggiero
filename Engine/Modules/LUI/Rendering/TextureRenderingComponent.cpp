////////////////////////////////////////////////////////////////////////////////
// Rendering/TextureRenderingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Texture Rendering Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureRenderingComponent.h"

// Leggiero.Graphics
#include <Graphics/Texture/GLTextureResource.h>

// Leggiero.LegacyUI
#include "../Common/UITransform.h"
#include "UIShaders.h"
#include "UIRenderer.h"
#include "UIRenderingUtility.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			//////////////////////////////////////////////////////////////////////////////// Internal Utility

			namespace _UITextureRenderingUtil
			{
				inline void PrepareRenderClippedRect(UISimpleRectangular &renderingRect, IUITransform &effectiveTransform,
					const Graphics::TextureRectSection &textureSection, const Graphics::GLByteARGB &color, const IUIClipping &clipping,
					bool isFlipHorizontal, bool isFlipVertical,
					std::vector<Graphics::TextureColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
				{
					RenderingUtility::PrepareRenderTextureRect(
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.bottom),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.bottom),
						textureSection, color, clipping, isFlipHorizontal, isFlipVertical, false, vertexBuffer, indexBuffer);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// TextureRenderingComponent

			//------------------------------------------------------------------------------
			TextureRenderingComponent::TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, Graphics::GLByteARGB color)
				: m_texture(texture), m_color(color)
				, m_section(0, 0, texture ? texture->GetTextureInfo().contentWidth : 0, texture ? texture->GetTextureInfo().contentHeight : 0, texture)
				, m_width(texture ? static_cast<UICoordinateType>(texture->GetTextureInfo().contentWidth) : static_cast<UICoordinateType>(0.0f)), m_height(texture ? static_cast<UICoordinateType>(texture->GetTextureInfo().contentHeight) : static_cast<UICoordinateType>(0.0f))
				, m_isFlipHorizontal(false), m_isFlipVertical(false)
			{
				m_vertexBuffer.reserve(4);
				m_indexBuffer.reserve(6);
			}

			//------------------------------------------------------------------------------
			TextureRenderingComponent::TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, Graphics::GLByteARGB color)
				: m_texture(texture), m_section(section), m_color(color)
				, m_width((UICoordinateType)section.GetPixelWidth()), m_height((UICoordinateType)section.GetPixelHeight())
				, m_isFlipHorizontal(false), m_isFlipVertical(false)
			{
				m_vertexBuffer.reserve(4);
				m_indexBuffer.reserve(6);
			}

			//------------------------------------------------------------------------------
			TextureRenderingComponent::TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color)
				: m_texture(texture), m_section(section), m_color(color)
				, m_width(width), m_height(height)
				, m_isFlipHorizontal(false), m_isFlipVertical(false)
			{
				m_vertexBuffer.reserve(4);
				m_indexBuffer.reserve(6);
			}

			//------------------------------------------------------------------------------
			TextureRenderingComponent::~TextureRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> TextureRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				return std::make_shared<TextureRenderingComponent>(m_texture, m_section, m_width, m_height, m_color);
			}

			//------------------------------------------------------------------------------
			void TextureRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				std::shared_ptr<Graphics::GLTextureResource> textureCopy(m_texture);
				if (!textureCopy)
				{
					return;
				}

				UISimpleRectangular effectiveTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), m_width, m_height);
				Graphics::GLByteARGB effectiveColor(m_color);
				if (alpha < 1.0f)
				{
					effectiveColor.alpha = static_cast<GLubyte>((float)effectiveColor.alpha * alpha);
				}
				_UITextureRenderingUtil::PrepareRenderClippedRect(effectiveTextureRectArea, effectiveTransform, m_section, effectiveColor, effectiveClipping, m_isFlipHorizontal, m_isFlipVertical, m_vertexBuffer, m_indexBuffer);

				if (m_indexBuffer.size() > 3)
				{
					std::shared_ptr<Shaders::UITextureColorShader> shader = renderer.GetTextureColorShader();
					shader->Use();

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);

					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glUniform1i(shader->GetTextureLocation(), 0);
					textureCopy->Bind(GL_TEXTURE0);

					auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
					Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
					glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
				}
				m_vertexBuffer.clear();
				m_indexBuffer.clear();
			}

			//------------------------------------------------------------------------------
			void TextureRenderingComponent::SetTexture(std::shared_ptr<Graphics::GLTextureResource> texture)
			{
				m_texture = texture;
				if (!texture)
				{
					return;
				}
				m_section = Graphics::TextureRectSection(0, 0, texture->GetTextureInfo().contentWidth, texture->GetTextureInfo().contentHeight, texture);
			}

			//------------------------------------------------------------------------------
			void TextureRenderingComponent::SetTexture(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section)
			{
				m_texture = texture;
				m_section = section;
			}

			//------------------------------------------------------------------------------
			void TextureRenderingComponent::SetSize(UICoordinateType width, UICoordinateType height)
			{
				m_width = width;
				m_height = height;
			}

			//------------------------------------------------------------------------------
			void TextureRenderingComponent::SetSizeRatio(UICoordinateRatioType widthRatio, UICoordinateRatioType heightRatio)
			{
				if (!m_texture)
				{
					return;
				}
				m_width = static_cast<UICoordinateType>(m_section.GetPixelWidth() * widthRatio);
				m_height = static_cast<UICoordinateType>(m_section.GetPixelHeight() * heightRatio);
			}


			//////////////////////////////////////////////////////////////////////////////// BlurredTextureRenderingComponent

			//------------------------------------------------------------------------------
			BlurredTextureRenderingComponent::BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, Graphics::GLByteARGB color)
				: TextureRenderingComponent(texture, color)
				, m_widthTexelUVRatio(1.0f), m_heightTexelUVRatio(1.0f)
			{
			}

			//------------------------------------------------------------------------------
			BlurredTextureRenderingComponent::BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, Graphics::GLByteARGB color)
				: TextureRenderingComponent(texture, section, color)
				, m_widthTexelUVRatio(1.0f), m_heightTexelUVRatio(1.0f)
			{
			}

			//------------------------------------------------------------------------------
			BlurredTextureRenderingComponent::BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color)
				: TextureRenderingComponent(texture, section, width, height, color)
				, m_widthTexelUVRatio(1.0f), m_heightTexelUVRatio(1.0f)
			{
			}

			//------------------------------------------------------------------------------
			BlurredTextureRenderingComponent::BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, float widthBlurRatio, float heightBlurRatio, Graphics::GLByteARGB color)
				: TextureRenderingComponent(texture, section, width, height, color)
				, m_widthTexelUVRatio(widthBlurRatio), m_heightTexelUVRatio(heightBlurRatio)
			{
			}

			//------------------------------------------------------------------------------
			BlurredTextureRenderingComponent::~BlurredTextureRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> BlurredTextureRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				return std::make_shared<BlurredTextureRenderingComponent>(m_texture, m_section, m_width, m_height, m_widthTexelUVRatio, m_heightTexelUVRatio, m_color);
			}

			//------------------------------------------------------------------------------
			void BlurredTextureRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				std::shared_ptr<Graphics::GLTextureResource> textureCopy(m_texture);
				if (!textureCopy)
				{
					return;
				}

				UISimpleRectangular effectiveTextureRectArea = UISimpleRectangular::FromPositionSize(0.0f, 0.0f, m_width, m_height);
				Graphics::GLByteARGB effectiveColor(m_color);
				if (alpha < 1.0f)
				{
					effectiveColor.alpha = static_cast<GLubyte>((float)effectiveColor.alpha * alpha);
				}
				_UITextureRenderingUtil::PrepareRenderClippedRect(effectiveTextureRectArea, effectiveTransform, m_section, effectiveColor, effectiveClipping, m_isFlipHorizontal, m_isFlipVertical, m_vertexBuffer, m_indexBuffer);

				float texelWidthParameter = 0.0f;
				if (textureCopy->GetTextureInfo().width > kFloatEpsilon)
				{
					texelWidthParameter = 1.0f / textureCopy->GetTextureInfo().width;
				}
				texelWidthParameter *= m_widthTexelUVRatio;
				float texelHeightParameter = 0.0f;
				if (textureCopy->GetTextureInfo().height > kFloatEpsilon)
				{
					texelHeightParameter = 1.0f / textureCopy->GetTextureInfo().height;
				}
				texelHeightParameter *= m_heightTexelUVRatio;

				if (m_indexBuffer.size() > 3)
				{
					std::shared_ptr<Shaders::UITextureBlurShader> shader = renderer.GetTextureBlurShader();
					shader->Use();

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);

					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glUniform1i(shader->GetTextureLocation(), 0);
					textureCopy->Bind(GL_TEXTURE0);

					glUniform1f(shader->GetTexelWidthOffsetLocation(), texelWidthParameter);
					glUniform1f(shader->GetTexelHeightOffsetLocation(), texelHeightParameter);

					auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
					Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
					glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
				}
				m_vertexBuffer.clear();
				m_indexBuffer.clear();
			}

			//------------------------------------------------------------------------------
			void BlurredTextureRenderingComponent::SetLinearBlurMultiplier(float widthRatio, float heightRatio)
			{
				m_widthTexelUVRatio = widthRatio;
				m_heightTexelUVRatio = heightRatio;
			}
		}
	}
}
