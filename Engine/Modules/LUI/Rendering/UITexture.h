////////////////////////////////////////////////////////////////////////////////
// Rendering/UITexture.h (Leggiero/Modules - LegacyUI)
//
// Texture data in UI system
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_TEXTURE_H
#define __LM_LUI__RENDERING__UI_TEXTURE_H


// Standard Library
#include <memory>

// Leggiero.Graphics
#include <Graphics/Texture/TextureSection.h>


namespace Leggiero
{
	// Forward Declarations
	namespace Graphics
	{
		class GLTextureResource;
		class TextureAtlasTable;
	}


	namespace LUI
	{
		// Loaded and Cached UI Texture
		class UICachedTexture
		{
		public:
			UICachedTexture(std::shared_ptr<Graphics::GLTextureResource> texture, std::shared_ptr<Graphics::TextureAtlasTable> atlasTable = nullptr)
				: m_texture(texture), m_atlasTable(atlasTable)
			{ }
			virtual ~UICachedTexture() { }

		public:
			bool IsAtlasTexture() const { return (bool)m_atlasTable; }

			std::shared_ptr<Graphics::GLTextureResource> GetTexture() const { return m_texture; }
			const Graphics::TextureAtlasTable &GetAtlasTable() const { return *m_atlasTable; }

		protected:
			std::shared_ptr<Graphics::GLTextureResource>	m_texture;
			std::shared_ptr<Graphics::TextureAtlasTable>	m_atlasTable;
		};


		// Texture section to render in UI
		struct UIRenderTextureSection
		{
		public:
			std::shared_ptr<Graphics::GLTextureResource>	texture;
			Graphics::TextureRectSection					section;

		public:
			UIRenderTextureSection(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section)
				: texture(texture), section(section)
			{ }
		};
	}
}

#endif
