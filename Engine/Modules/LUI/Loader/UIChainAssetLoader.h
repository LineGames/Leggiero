////////////////////////////////////////////////////////////////////////////////
// Loader/UIChainAssetLoader.h (Leggiero/Modules - LegacyUI)
//
// Asset data loader from chained asset loaders
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__LOADER__UI_CHAIN_ASSET_LOADER_H
#define __LM_LUI__LOADER__UI_CHAIN_ASSET_LOADER_H


// Standard Library
#include <initializer_list>
#include <list>
#include <memory>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.LegacyUI
#include "IUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		// Chained Asset Loader
		class UIChainAssetLoader
			: public IUIAssetLoader
		{
		public:
			UIChainAssetLoader();
			UIChainAssetLoader(std::initializer_list<std::shared_ptr<IUIAssetLoader> > loaders);

			template <class InputIterator>
			UIChainAssetLoader(InputIterator first, InputIterator last)
				: m_loaderChain(first, last)
			{
				_TrimLoaderChain();
			}

			virtual ~UIChainAssetLoader();

		public:	// IUIAssetLoader
			virtual size_t ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer) override;

			virtual std::shared_ptr<Graphics::GLTextureResource> LoadTexture(const std::string &textureName) override;
			virtual std::shared_ptr<Graphics::TextureAtlasTable> LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture = nullptr) override;

			virtual std::shared_ptr<Font::IFontFace> LoadFontFace(const std::string &fontName) override;

		public:
			size_t GetLoaderChainLength();
			std::list<std::shared_ptr<IUIAssetLoader> > GetLoaderChain();

			bool AttachLoaderFront(std::shared_ptr<IUIAssetLoader> loader);
			bool AttachLoaderBack(std::shared_ptr<IUIAssetLoader> loader);
			void EraseLoader(std::shared_ptr<IUIAssetLoader> loader);

			void ClearLoader();

		private:
			// Should called in write lock or non-race condition
			void _TrimLoaderChain();

		protected:
			std::list<std::shared_ptr<IUIAssetLoader> >		m_loaderChain;
			Utility::Threading::SafePthreadRWLock			m_loaderChainLock;
		};
	}
}

#endif
