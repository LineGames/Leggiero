////////////////////////////////////////////////////////////////////////////////
// Rendering/UITextureManager.h (Leggiero/Modules - LegacyUI)
//
// Manager class to manage texture resources in UI system
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_TEXTURE_MANAGER_H
#define __LM_LUI__RENDERING__UI_TEXTURE_MANAGER_H


// Standard Library
#include <memory>
#include <unordered_map>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "UITexture.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class IUIAssetLoader;


		// Texture Manager
		class UITextureManager
		{
		public:
			UITextureManager(IUIAssetLoader &assetLoader);
			virtual ~UITextureManager();

		private:
			UITextureManager() = delete;

		public:
			std::shared_ptr<UICachedTexture> GetTexture(const UITextureNameType &textureName);
			void PreLoadTexture(const UITextureNameType &textureName);

			void RegisterExternalTexture(const UITextureNameType &textureName, std::shared_ptr<Graphics::GLTextureResource> texture, std::shared_ptr<Graphics::TextureAtlasTable> atlasTable = nullptr);
			std::shared_ptr<UICachedTexture> GetCachedTexture(const UITextureNameType &savedTextureName);

		public:
			void ClearCache();

		protected:
			std::shared_ptr<UICachedTexture> _LoadTexture(const UITextureNameType &textureName);

		protected:
			IUIAssetLoader &m_assetLoader;

			std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> > m_textureCache;
			Utility::Threading::SafePthreadRWLock m_cacheLock;
		};
	}
}

#endif
