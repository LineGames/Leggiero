////////////////////////////////////////////////////////////////////////////////
// Rendering/UITextureManager.cpp (Leggiero/Modules - LegacyUI)
//
// Texture Manager Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITextureManager.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.LegacyUI
#include "../Loader/IUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UITextureManager

		//------------------------------------------------------------------------------
		UITextureManager::UITextureManager(IUIAssetLoader &assetLoader)
			: m_assetLoader(assetLoader)
		{
		}

		//------------------------------------------------------------------------------
		UITextureManager::~UITextureManager()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UICachedTexture> UITextureManager::GetTexture(const UITextureNameType &textureName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_cacheLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_cacheLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					return findIt->second;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					return findIt->second;
				}
			}

			std::shared_ptr<UICachedTexture> loadedTexture(_LoadTexture(textureName));
			if (!loadedTexture)
			{
				return nullptr;
			}

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					// Race Condition
					return findIt->second;
				}
				m_textureCache.insert(std::make_pair(textureName, loadedTexture));
			}

			return loadedTexture;
		}

		//------------------------------------------------------------------------------
		void UITextureManager::PreLoadTexture(const UITextureNameType &textureName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_cacheLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_cacheLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					return;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					return;
				}
			}

			std::shared_ptr<UICachedTexture> loadedTexture(_LoadTexture(textureName));
			if (!loadedTexture)
			{
				return;
			}

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(textureName);
				if (findIt != m_textureCache.end())
				{
					return;
				}
				m_textureCache.insert(std::make_pair(textureName, loadedTexture));
			}
		}

		//------------------------------------------------------------------------------
		void UITextureManager::RegisterExternalTexture(const UITextureNameType &textureName, std::shared_ptr<Graphics::GLTextureResource> texture, std::shared_ptr<Graphics::TextureAtlasTable> atlasTable)
		{
			std::shared_ptr<UICachedTexture> textureEntry(std::make_shared<UICachedTexture>(texture, atlasTable));

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureCache[textureName] = textureEntry;
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UICachedTexture> UITextureManager::GetCachedTexture(const UITextureNameType &savedTextureName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_cacheLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_cacheLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(savedTextureName);
				if (findIt != m_textureCache.end())
				{
					return findIt->second;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UITextureNameType, std::shared_ptr<UICachedTexture> >::iterator findIt = m_textureCache.find(savedTextureName);
				if (findIt != m_textureCache.end())
				{
					return findIt->second;
				}
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
		void UITextureManager::ClearCache()
		{
			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureCache.clear();
			}
			else
			{
				// Anyway, we should go...
				m_textureCache.clear();
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UICachedTexture> UITextureManager::_LoadTexture(const UITextureNameType &textureName)
		{
			std::shared_ptr<Graphics::GLTextureResource> texture(m_assetLoader.LoadTexture(textureName));
			if (!texture)
			{
				return nullptr;
			}

			return std::make_shared<UICachedTexture>(texture, m_assetLoader.LoadTextureAtlasTable(textureName, texture));
		}
	}
}
