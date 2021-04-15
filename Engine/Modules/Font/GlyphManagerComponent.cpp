////////////////////////////////////////////////////////////////////////////////
// GlyphManagerComponent.cpp (Leggiero/Modules - Font)
//
// Glyph Resource Manager Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GlyphManagerComponent.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Texture/TextureHelper.h>
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Texture/DynamicTextureResource.h>

// Leggiero.Font
#include "Common/IFontFace.h"
#include "Common/CachedGlyph.h"
#include "Common/SingleFontSet.h"
#include "FreeType/FreeTypeNormalGlyphDrawer.h"


DEFINE_GET_COMPONENT_INTERFACE(Leggiero::Font::GlyphManagerComponent, Leggiero::EngineComponentIdType::kGlyphManager);


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// Code-Level Constants

		namespace _Internal
		{
			constexpr GLsizei kDefaultGlyphAtlasSize = 2048;
		}

		//////////////////////////////////////////////////////////////////////////////// GlyphResourceManagerComponent

		//------------------------------------------------------------------------------
		GlyphManagerComponent::GlyphManagerComponent()
			: m_textureAtlasManager(nullptr)
		{
		}

		//------------------------------------------------------------------------------
		GlyphManagerComponent::~GlyphManagerComponent()
		{
		}

		//------------------------------------------------------------------------------
		// Initialize the Component
		void GlyphManagerComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
		{
			// Initialize Atlas
			m_glyphDefaultAtalsGroupId = m_textureAtlasManager->RegisterTextureAtlasGroup(1, _Internal::kDefaultGlyphAtlasSize);

			// Initialize Glyph Drawers
			m_drawers.insert(std::make_pair(static_cast<int>(FontBackendType::kFreeType), std::array<std::shared_ptr<IGlyphDrawer>, static_cast<size_t>(TextureType::kTextureTypeCount)>()));
			m_drawers[static_cast<int>(FontBackendType::kFreeType)][(size_t)TextureType::kNormal] = std::make_shared<FreeType::NormalGlyphDrawer>();
		}

		//------------------------------------------------------------------------------
		// Safely Shutdown Component
		void GlyphManagerComponent::ShutdownComponent()
		{
			m_drawers.clear();
			m_cache.Clear();
		}

		//------------------------------------------------------------------------------
		// Get type Id list of dependent components needed by this component
		const std::vector<EngineComponentIdType> &GlyphManagerComponent::GetDependentComponents() const
		{
			static std::vector<EngineComponentIdType> dependentComponentList {
				EngineComponentIdType::kGraphicResourceManager
			};
			return dependentComponentList;
		}

		//------------------------------------------------------------------------------
		// Inject Dependency to the Component.
		// All dependency injections will be done before the initialization.
		void GlyphManagerComponent::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
		{
			switch (componentId)
			{
				case EngineComponentIdType::kGraphicResourceManager:
					{
						m_textureAtlasManager = &dynamic_cast<Graphics::GraphicResourceManagerComponent *>(dependentComponent)->GetRuntimeTextureAtlasManager();
					}
					break;

			}
		}

		//------------------------------------------------------------------------------
		// Get glyph drawer for given type of texture and backend
		std::shared_ptr<IGlyphDrawer> GlyphManagerComponent::GetGlyphDrawer(TextureType glypeTextureType, FontBackendType backendType)
		{
			int backendTypeNumber = static_cast<int>(backendType);
			auto backendFindIt = m_drawers.find(backendTypeNumber);
			if (backendFindIt == m_drawers.end())
			{
				// Not-supporting Backend
				return nullptr;
			}
			return backendFindIt->second[static_cast<size_t>(glypeTextureType)];
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphManagerComponent::LoadGlyph(std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			return LoadGlyphInAtlasGroup(m_glyphDefaultAtalsGroupId, font, glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphManagerComponent::LoadGlyph(std::shared_ptr<IFontSet> fontSet, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			return LoadGlyphInAtlasGroup(m_glyphDefaultAtalsGroupId, fontSet, glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphManagerComponent::LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			// Find from cache
			std::shared_ptr<CachedGlyph> cachedEntry = m_cache.ReadGlyph(atlasGroupId, font->GetFontId(), glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
			if (cachedEntry)
			{
				if (!cachedEntry->IsLoadSuccess())
				{
					return nullptr;
				}
				return cachedEntry;
			}

			// Load Glype
			IFontSet::EffectiveFontResult effectiveFont = SingleFontSet(font).GetEffectiveFont(glyph, style);
			effectiveFont.isBoldUnresolved = Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kBold);
			effectiveFont.isItalicUnresolved = Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kItalic);

			GlyphLoadResult loadResult = _LoadGlyphInAtlasGroup(atlasGroupId, effectiveFont, size, horizontalRatio, color, textureType, Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kPremultipliedAlpha), strokeWidth, strokeColor);

			// Register to Cache
			cachedEntry = m_cache.WriteLoadedGlyph(loadResult.isSuccess, loadResult.offsetX, loadResult.offsetY, loadResult.xAdvance, loadResult.yAdvance, loadResult.atlasEntry, effectiveFont.glyphCode, effectiveFont.isBoldUnresolved, effectiveFont.isItalicUnresolved,
				atlasGroupId, font, glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
			if (cachedEntry)
			{
				if (!cachedEntry->IsLoadSuccess())
				{
					// Just Cache Result
					return nullptr;
				}
			}
			return cachedEntry;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphManagerComponent::LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontSet> fontSet, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			// Find from search cache
			std::shared_ptr<CachedGlyph> cachedEntry = m_searchCache.ReadGlyph(atlasGroupId, fontSet->GetSetUniqueId(), glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
			if (cachedEntry)
			{
				if (!cachedEntry->IsLoadSuccess())
				{
					return nullptr;
				}
				return cachedEntry;
			}

			IFontSet::EffectiveFontResult effectiveFont = fontSet->GetEffectiveFont(glyph, style);
			if (!effectiveFont.IsValid())
			{
				return nullptr;
			}

			// Find from Single Font Cache
			GlyphStyleType effectiveStyle = style;
			if (effectiveFont.isBoldUnresolved)
			{
				effectiveStyle |= GlyphStyleType::kBold;
			}
			else
			{
				effectiveStyle &= ~GlyphStyleType::kBold;
			}
			if (effectiveFont.isItalicUnresolved)
			{
				effectiveStyle |= GlyphStyleType::kItalic;
			}
			else
			{
				effectiveStyle &= ~GlyphStyleType::kItalic;
			}
			cachedEntry = m_cache.ReadGlyph(atlasGroupId, effectiveFont.face->GetFontId(), glyph, size, horizontalRatio, effectiveStyle, color, textureType, strokeWidth, strokeColor);
			if (cachedEntry)
			{
				m_searchCache.WriteFindResult(cachedEntry, atlasGroupId, fontSet->GetSetUniqueId(), glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
				if (!cachedEntry->IsLoadSuccess())
				{
					return nullptr;
				}
				return cachedEntry;
			}

			// Load Glype
			GlyphLoadResult loadResult = _LoadGlyphInAtlasGroup(atlasGroupId, effectiveFont, size, horizontalRatio, color, textureType, Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kPremultipliedAlpha), strokeWidth, strokeColor);

			// Register to Cache
			cachedEntry = m_cache.WriteLoadedGlyph(loadResult.isSuccess, loadResult.offsetX, loadResult.offsetY, loadResult.xAdvance, loadResult.yAdvance, loadResult.atlasEntry, effectiveFont.glyphCode, effectiveFont.isBoldUnresolved, effectiveFont.isItalicUnresolved,
				atlasGroupId, effectiveFont.face, glyph, size, horizontalRatio, effectiveStyle, color, textureType, strokeWidth, strokeColor);
			if (cachedEntry)
			{
				m_searchCache.WriteFindResult(cachedEntry, atlasGroupId, fontSet->GetSetUniqueId(), glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
				if (!cachedEntry->IsLoadSuccess())
				{
					// Just Cache Result
					return nullptr;
				}
			}

			return cachedEntry;
		}

		//------------------------------------------------------------------------------
		GlyphManagerComponent::GlyphLoadResult GlyphManagerComponent::_LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, IFontSet::EffectiveFontResult glypeSearchResult, GlyphSizeType size,
			float horizontalRatio, Graphics::GLByteARGB color, TextureType textureType, bool isPremultipliedAlpha, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			if (!glypeSearchResult.IsValid())
			{
				return GlyphLoadResult(false);
			}
			std::shared_ptr<IGlyphDrawer> drawer = GetGlyphDrawer(textureType, glypeSearchResult.face->GetFontBackendType());
			if (!drawer)
			{
				return GlyphLoadResult(false);
			}

			// Calculate Buffer Size
			IGlyphDrawer::GlyphSizeInfo neededTextureSize = drawer->GetGlyphImageSIze(glypeSearchResult.face, glypeSearchResult.glyphCode, size, horizontalRatio, glypeSearchResult.isBoldUnresolved, glypeSearchResult.isItalicUnresolved, strokeWidth);
			if (!neededTextureSize.IsValid())
			{
				// Size Calculation Failed
				return GlyphLoadResult(false);
			}

			// Prepare Buffer
			void *imageBuffer = malloc(neededTextureSize.width * neededTextureSize.height * Graphics::Texture::GetTexelByteSize(GL_RGBA, GL_UNSIGNED_BYTE));
			if (imageBuffer == nullptr)
			{
				// Out of Memory
				return GlyphLoadResult(false);
			}
			{
				auto releaseBufferFunc = [imageBuffer]() mutable { free(imageBuffer); };
				FINALLY_OF_BLOCK(_RELEASE_BUFFER, releaseBufferFunc);

				// Do Draw
				Graphics::GLByteARGB::ColorCodeType colorCode = color.ToColorCodeARGB();
				Graphics::GLByteARGB::ColorCodeType strokeColorCode = strokeColor.ToColorCodeARGB();
				IGlyphDrawer::GlyphDrawResult drawResult = drawer->DrawGlyph(imageBuffer, glypeSearchResult.face, glypeSearchResult.glyphCode, size,
					horizontalRatio, colorCode, glypeSearchResult.isBoldUnresolved, glypeSearchResult.isItalicUnresolved,
					GL_RGBA, GL_UNSIGNED_BYTE, isPremultipliedAlpha,
					strokeWidth, strokeColorCode);
				if (!drawResult.isDrawSuccess)
				{
					// Draw Failed
					return GlyphLoadResult(false);
				}
				if (drawResult.realDrawWidth == 0 || drawResult.realDrawHeight == 0)
				{
					// Empty Image. Can be a whitespace
					return GlyphLoadResult(true, drawResult.bitmapOffsetX, drawResult.bitmapOffsetY, nullptr, drawResult.xAdvance, drawResult.yAdvance);
				}

				// Define Glyph Image Restorer
				Graphics::RuntimeTextureAtlasEntry::RestorerType restorer = [drawer, neededTextureSize, glypeSearchResult, size, horizontalRatio, colorCode, isPremultipliedAlpha, strokeWidth, strokeColorCode](std::shared_ptr<Graphics::DynamicTextureResource> targetTexture, const Graphics::TextureRectSection &allocatedArea) {
					void *restoreImageBuffer = malloc(neededTextureSize.width * neededTextureSize.height * Graphics::Texture::GetTexelByteSize(GL_RGBA, GL_UNSIGNED_BYTE));
					if (restoreImageBuffer == nullptr)
					{
						// Out of Memory
						return false;
					}

					auto releaseRestoreBufferFunc = [restoreImageBuffer]() mutable { free(restoreImageBuffer); };
					FINALLY_OF_BLOCK(_RELEASE_RESTORE_BUFFER, releaseRestoreBufferFunc);

					IGlyphDrawer::GlyphDrawResult restoreDrawResult = drawer->DrawGlyph(restoreImageBuffer, glypeSearchResult.face, glypeSearchResult.glyphCode, size,
						horizontalRatio, colorCode, glypeSearchResult.isBoldUnresolved, glypeSearchResult.isItalicUnresolved,
						GL_RGBA, GL_UNSIGNED_BYTE, isPremultipliedAlpha,
						strokeWidth, strokeColorCode);
					if (!restoreDrawResult.isDrawSuccess)
					{
						// Draw Failed
						return false;
					}

					targetTexture->UploadSubImage(allocatedArea.GetPixelLeft(), allocatedArea.GetPixelTop(), restoreImageBuffer, restoreDrawResult.realDrawWidth, restoreDrawResult.realDrawHeight);
					return true;
				};

				// Save in Texture Atlas
				std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> glyphAtalsEntry = m_textureAtlasManager->InsertSubTexture(atlasGroupId, imageBuffer, drawResult.realDrawWidth, drawResult.realDrawHeight, GL_RGBA, GL_UNSIGNED_BYTE, restorer);
				if (!glyphAtalsEntry)
				{
					// Atlas Upload Failed
					return GlyphLoadResult(false);
				}

				// Success
				return GlyphLoadResult(true, drawResult.bitmapOffsetX, drawResult.bitmapOffsetY, glyphAtalsEntry, drawResult.xAdvance, drawResult.yAdvance);
			}
		}
	}
}
