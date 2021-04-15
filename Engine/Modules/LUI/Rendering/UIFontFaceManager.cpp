////////////////////////////////////////////////////////////////////////////////
// Rendering/UIFontFaceManager.cpp (Leggiero/Modules - LegacyUI)
//
// Font Face Manager Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIFontFaceManager.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.LegacyUI
#include "../Loader/IUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIFontFaceManager

		//------------------------------------------------------------------------------
		UIFontFaceManager::UIFontFaceManager(IUIAssetLoader &assetLoader)
			: m_assetLoader(assetLoader)
		{
		}

		//------------------------------------------------------------------------------
		UIFontFaceManager::~UIFontFaceManager()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIFontFaceManager::GetFont(const UIFontNameType &fontName)
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

				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return findIt->second;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return findIt->second;
				}
			}

			std::shared_ptr<Font::IFontFace> loadedFont(_LoadFont(fontName));
			if (!loadedFont)
			{
				return nullptr;
			}

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					// Race Condition
					return findIt->second;
				}
				m_fontCache.insert(std::make_pair(fontName, loadedFont));
			}

			return loadedFont;
		}

		//------------------------------------------------------------------------------
		void UIFontFaceManager::PreLoadFont(const UIFontNameType &fontName)
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

				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return;
				}
			}

			std::shared_ptr<Font::IFontFace> loadedFont(_LoadFont(fontName));
			if (!loadedFont)
			{
				return;
			}

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return;
				}
				m_fontCache.insert(std::make_pair(fontName, loadedFont));
			}
		}

		//------------------------------------------------------------------------------
		void UIFontFaceManager::RegisterExternalFont(const UIFontNameType &fontName, std::shared_ptr<Font::IFontFace> font)
		{
			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				if (!font)
				{
					m_fontCache.erase(fontName);
				}
				else
				{
					m_fontCache[fontName] = font;
				}
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIFontFaceManager::GetCachedFont(const UIFontNameType &fontName)
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

				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return findIt->second;
				}
			}
			else
			{
				// Anyway, Go
				std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontCache.find(fontName);
				if (findIt != m_fontCache.end())
				{
					return findIt->second;
				}
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
		void UIFontFaceManager::ClearCache()
		{
			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_fontCache.clear();
			}
			else
			{
				// Anyway, we should go...
				m_fontCache.clear();
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIFontFaceManager::_LoadFont(const UIFontNameType &fontName)
		{
			return m_assetLoader.LoadFontFace(fontName);
		}
	}
}
