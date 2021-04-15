////////////////////////////////////////////////////////////////////////////////
// Texture/TextureAtlasTable.h (Leggiero/Modules - Graphics)
//
// Simple Texture Atlas Table
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__TEXTURE_ATLAS_TABLE_H
#define __LM_GRAPHICS__TEXTURE__TEXTURE_ATLAS_TABLE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

// Leggiero.Graphics
#include "TextureSection.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declaration
		class GLTextureResource;


		// Rectangular Section based Simple Texture Atlas Table
		class TextureAtlasTable
		{
		public:
			std::unordered_map<GameDataString, std::shared_ptr<TextureRectSection> > atlasTextureSectionTable;

		public:
			TextureAtlasTable() { }
			virtual ~TextureAtlasTable() { }

		public:
			std::shared_ptr<TextureRectSection> GetTextureSection(const GameDataString &sectionName) const;
			std::shared_ptr<TextureRectSection> GetTextureSection(const char *sectionName, size_t nameLength = std::numeric_limits<size_t>::max()) const;

		public:
			static std::shared_ptr<TextureAtlasTable> LoadFromXML(std::string xmlData, std::shared_ptr<GLTextureResource> texture = std::shared_ptr<GLTextureResource>());
		};
	}
}

#endif
