////////////////////////////////////////////////////////////////////////////////
// Loader/UIBundleAssetLoader.h (Leggiero/Modules - LegacyUI)
//
// Asset data loader from bundle file system
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__LOADER__UI_BUNDLE_ASSET_LOADER_H
#define __LM_LUI__LOADER__UI_BUNDLE_ASSET_LOADER_H


// Leggiero.LegacyUI
#include "IUIAssetLoader.h"


namespace Leggiero
{
	// Forward Declarations
	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}

	namespace Graphics
	{
		class GraphicResourceManagerComponent;
	}


	namespace LUI
	{
		// Bundle Asset Loader
		class UIBundleAssetLoader
			: public IUIAssetLoader
		{
		public:
			static const std::string kDefaultDescriptionSubPath;
			static const std::string kDefaultTextureSubPath;
			static const std::string kDefaultFontSubPath;

		public:
			UIBundleAssetLoader(FileSystem::BundleFileResourceComponent *bundleComponent, Graphics::GraphicResourceManagerComponent *graphicResourceManagerComponent, 
				const std::string &uiBasePath = std::string(), 
				const std::string &descriptionSubPath = kDefaultDescriptionSubPath, const std::string &textureSubPath = kDefaultTextureSubPath, const std::string &fontSubPath = kDefaultFontSubPath);
			virtual ~UIBundleAssetLoader();

		public:	// IUIAssetLoader
			virtual size_t ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer) override;

			virtual std::shared_ptr<Graphics::GLTextureResource> LoadTexture(const std::string &textureName) override;
			virtual std::shared_ptr<Graphics::TextureAtlasTable> LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture = nullptr) override;

			virtual std::shared_ptr<Font::IFontFace> LoadFontFace(const std::string &fontName) override;

		protected:
			FileSystem::BundleFileResourceComponent		*m_bundle;
			Graphics::GraphicResourceManagerComponent	*m_graphicResourceManager;

			std::string m_uiBasePath;
			std::string m_descBasePath;
			std::string m_textureBasePath;
			std::string m_fontBasePath;
		};
	}
}

#endif
