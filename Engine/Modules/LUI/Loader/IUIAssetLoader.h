////////////////////////////////////////////////////////////////////////////////
// Loader/IUIAssetLoader.h (Leggiero/Modules - LegacyUI)
//
// Asset data loader interface for UI system
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__LOADER__I_UI_ASSET_LOADER_H
#define __LM_LUI__LOADER__I_UI_ASSET_LOADER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <iostream>
#include <memory>
#include <string>


namespace Leggiero
{
	// Forward Declarations
	namespace Graphics
	{
		class GLTextureResource;
		class TextureAtlasTable;
	}

	namespace Font
	{
		class IFontFace;
	}


	namespace LUI
	{
		// Asset Loader Interface
		class IUIAssetLoader
		{
		public:
			virtual ~IUIAssetLoader() { }

		public:
			virtual size_t ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer) = 0;

			virtual std::shared_ptr<Graphics::GLTextureResource> LoadTexture(const std::string &textureName) = 0;
			virtual std::shared_ptr<Graphics::TextureAtlasTable> LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture = nullptr) = 0;

			virtual std::shared_ptr<Font::IFontFace> LoadFontFace(const std::string &fontName) = 0;
		};
	}
}

#endif
