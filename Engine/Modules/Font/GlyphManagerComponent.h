////////////////////////////////////////////////////////////////////////////////
// GlyphManagerComponent.h (Leggiero/Modules - Font)
//
// Rendered Glyph Resource Manager
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__GLYPH_RESOURCE_MANAGER_COMPONENT_H
#define __LM_FONT__GLYPH_RESOURCE_MANAGER_COMPONENT_H


// Standard Library
#include <array>
#include <memory>
#include <unordered_map>

// External Library
#include <GLES3.h>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Texture/RuntimeTextureAtlasManager.h>

// Leggiero.Font
#include "Common/FontTypes.h"
#include "Common/FontBackend.h"
#include "Common/IFontSet.h"
#include "Common/GlyphCache.h"
#include "Common/GlyphSearchCache.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class RuntimeTextureAtlasEntry;
	}


	namespace Font
	{
		// Forward Declaration
		class IGlyphDrawer;
		class IFontFace;
		class CachedGlyph;


		// Glyph Resource Manager Component
		class GlyphManagerComponent
			: public EngineComponent
		{
		public:
			GlyphManagerComponent();
			virtual ~GlyphManagerComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kGlyphManager; };

			// Initialize the Component
			virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

			// Safely Shutdown Component
			virtual void ShutdownComponent() override;

			// Get type Id list of dependent components needed by this component
			virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

		public:
			// Get glyph drawer for given type of texture and backend
			std::shared_ptr<IGlyphDrawer> GetGlyphDrawer(TextureType glypeTextureType, FontBackendType backendType = FontBackendType::kFreeType);

		public:	// Glyph Loaders
			std::shared_ptr<CachedGlyph> LoadGlyph(std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio = 1.0f, GlyphStyleType style = GlyphStyleType::kNone, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite,
				TextureType textureType = TextureType::kNormal,
				float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent);

			std::shared_ptr<CachedGlyph> LoadGlyph(std::shared_ptr<IFontSet> fontSet, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio = 1.0f, GlyphStyleType style = GlyphStyleType::kNone, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite,
				TextureType textureType = TextureType::kNormal,
				float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent);

			std::shared_ptr<CachedGlyph> LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio = 1.0f, GlyphStyleType style = GlyphStyleType::kNone, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite,
				TextureType textureType = TextureType::kNormal,
				float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent);

			std::shared_ptr<CachedGlyph> LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontSet> fontSet, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio = 1.0f, GlyphStyleType style = GlyphStyleType::kNone, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite,
				TextureType textureType = TextureType::kNormal,
				float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent);

		protected:
			struct GlyphLoadResult
			{
			public:
				bool isSuccess;
				GLint offsetX;
				GLint offsetY;
				int xAdvance;
				int yAdvance;
				std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> atlasEntry;

			public:
				GlyphLoadResult(bool isSuccess, GLint offsetX = 0, GLint offsetY = 0, std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> atlasEntry = nullptr, int xAdvance = 0, int yAdvance = 0)
					: isSuccess(isSuccess), offsetX(offsetX), offsetY(offsetY), atlasEntry(atlasEntry), xAdvance(xAdvance), yAdvance(yAdvance)
				{ }
			};

			GlyphLoadResult _LoadGlyphInAtlasGroup(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, IFontSet::EffectiveFontResult glypeSearchResult, GlyphSizeType size,
				float horizontalRatio, Graphics::GLByteARGB color, TextureType textureType, bool isPremultipliedAlpha, float strokeWidth, Graphics::GLByteARGB strokeColor);

		protected:
			Graphics::RuntimeTextureAtlasManager					*m_textureAtlasManager;
			Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType	m_glyphDefaultAtalsGroupId;

			std::unordered_map<int, std::array<std::shared_ptr<IGlyphDrawer>, static_cast<size_t>(TextureType::kTextureTypeCount)> > m_drawers;

			GlyphCache			m_cache;
			GlyphSearchCache	m_searchCache;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Font::GlyphManagerComponent, Leggiero::EngineComponentIdType::kGlyphManager);

#endif
