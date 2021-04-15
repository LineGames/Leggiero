////////////////////////////////////////////////////////////////////////////////
// Loader/DummyUIAssetLoader.h (Leggiero/Modules - LegacyUI)
//
// Dummy not-loading asset loader for placeholder
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__LOADER__DUMMY_UI_ASSET_LOADER_H
#define __LM_LUI__LOADER__DUMMY_UI_ASSET_LOADER_H


// Leggiero.LegacyUI
#include "IUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		// Dummy Asset Loader
		class DummyAssetLoader
			: public IUIAssetLoader
		{
		public:
			DummyAssetLoader() { }
			virtual ~DummyAssetLoader() { }

		public:	// IUIAssetLoader
			virtual size_t ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer) override { return 0; }

			virtual std::shared_ptr<Graphics::GLTextureResource> LoadTexture(const std::string &textureName) override { return nullptr; }
			virtual std::shared_ptr<Graphics::TextureAtlasTable> LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture = nullptr) override { return nullptr; }

			virtual std::shared_ptr<Font::IFontFace> LoadFontFace(const std::string &fontName) override { return nullptr; }

		public:
			static DummyAssetLoader &GetInstance();
		};
	}
}

#endif
