////////////////////////////////////////////////////////////////////////////////
// Loader/UIChainAssetLoader.cpp (Leggiero/Modules - LegacyUI)
//
// Chain loader implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIChainAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIChainAssetLoader

		//------------------------------------------------------------------------------
		UIChainAssetLoader::UIChainAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		UIChainAssetLoader::UIChainAssetLoader(std::initializer_list<std::shared_ptr<IUIAssetLoader> > loaders)
			: m_loaderChain(loaders)
		{
			_TrimLoaderChain();
		}

		//------------------------------------------------------------------------------
		UIChainAssetLoader::~UIChainAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		size_t UIChainAssetLoader::ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (std::shared_ptr<IUIAssetLoader> &currentLoader : m_loaderChain)
				{
					size_t readBytes = currentLoader->ReadDescriptionUnitData(unitName, buffer);
					if (readBytes > 0)
					{
						return readBytes;
					}
				}
			}

			return 0;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::GLTextureResource> UIChainAssetLoader::LoadTexture(const std::string &textureName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (std::shared_ptr<IUIAssetLoader> &currentLoader : m_loaderChain)
				{
					std::shared_ptr<Graphics::GLTextureResource> texture(currentLoader->LoadTexture(textureName));
					if (texture)
					{
						return texture;
					}
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::TextureAtlasTable> UIChainAssetLoader::LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (std::shared_ptr<IUIAssetLoader> &currentLoader : m_loaderChain)
				{
					std::shared_ptr<Graphics::TextureAtlasTable> atlasTable(currentLoader->LoadTextureAtlasTable(textureName, texture));
					if (atlasTable)
					{
						return atlasTable;
					}
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIChainAssetLoader::LoadFontFace(const std::string &fontName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (std::shared_ptr<IUIAssetLoader> &currentLoader : m_loaderChain)
				{
					std::shared_ptr<Font::IFontFace> fontFace(currentLoader->LoadFontFace(fontName));
					if (fontFace)
					{
						return fontFace;
					}
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		size_t UIChainAssetLoader::GetLoaderChainLength()
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				return m_loaderChain.size();
			}
			else
			{
				// Lock failed, but go
				return m_loaderChain.size();
			}
		}

		//------------------------------------------------------------------------------
		std::list<std::shared_ptr<IUIAssetLoader> > UIChainAssetLoader::GetLoaderChain()
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_loaderChainLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				return m_loaderChain;
			}
			else
			{
				// Lock failed, but go
				return m_loaderChain;
			}
		}

		//------------------------------------------------------------------------------
		bool UIChainAssetLoader::AttachLoaderFront(std::shared_ptr<IUIAssetLoader> loader)
		{
			if (!loader)
			{
				return false;
			}
			if (loader.get() == this)
			{
				// Can make infinite recursive call
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_loaderChainLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_loaderChain.push_front(loader);
				return true;
			}
			else
			{
				// Lock Failed
			}
			return false;
		}

		//------------------------------------------------------------------------------
		bool UIChainAssetLoader::AttachLoaderBack(std::shared_ptr<IUIAssetLoader> loader)
		{
			if (!loader)
			{
				return false;
			}
			if (loader.get() == this)
			{
				// Can make infinite recursive call
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_loaderChainLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_loaderChain.push_back(loader);
				return true;
			}
			else
			{
				// Lock Failed
			}
			return false;
		}

		//------------------------------------------------------------------------------
		void UIChainAssetLoader::EraseLoader(std::shared_ptr<IUIAssetLoader> loader)
		{
			int lockResult = pthread_rwlock_wrlock(&m_loaderChainLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_loaderChain.remove(loader);
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		void UIChainAssetLoader::ClearLoader()
		{
			int lockResult = pthread_rwlock_wrlock(&m_loaderChainLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_loaderChainLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_loaderChain.clear();
			}
			else
			{
				// Lock Failed - anyway, go
				m_loaderChain.clear();
			}
		}

		//------------------------------------------------------------------------------
		// Should called in write lock or non-race condition
		void UIChainAssetLoader::_TrimLoaderChain()
		{
			for (std::list<std::shared_ptr<IUIAssetLoader> >::iterator it = m_loaderChain.begin(); it != m_loaderChain.end(); )
			{
				if (!(*it))
				{
					it = m_loaderChain.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
}
