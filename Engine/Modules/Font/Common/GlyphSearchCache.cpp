////////////////////////////////////////////////////////////////////////////////
// Common/GlyphSearchCache.cpp (Leggiero/Modules - Font)
//
// Glyph Search Cache Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GlyphSearchCache.h"

// External Library
#include <errno.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Font
#include "../_Internal/_FontInternalUtility.h"
#include "CachedGlyph.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// GlyphSearchCache

		//------------------------------------------------------------------------------
		GlyphSearchCache::GlyphSearchCache()
		{
		}

		//------------------------------------------------------------------------------
		GlyphSearchCache::~GlyphSearchCache()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphSearchCache::ReadGlyph(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			_CacheKey key(atlasGroupId, fontId, glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);

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

				Cache_Level_4_Type::RealType::iterator level4_findIt = m_cache.element.find(key.keys[4]);
				if (level4_findIt == m_cache.element.end())
				{
					return nullptr;
				}

				Cache_Level_3_Type::RealType::iterator level3_findIt = level4_findIt->second.element.find(key.keys[3]);
				if (level3_findIt == level4_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_2_Type::RealType::iterator level2_findIt = level3_findIt->second.element.find(key.keys[2]);
				if (level2_findIt == level3_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_1_Type::RealType::iterator level1_findIt = level2_findIt->second.element.find(key.keys[1]);
				if (level1_findIt == level2_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_0_Type::RealType::iterator level0_findIt = level1_findIt->second.element.find(key.keys[0]);
				if (level0_findIt == level1_findIt->second.element.end())
				{
					return nullptr;
				}

				std::shared_ptr<CachedGlyph> currentCachedValue = level0_findIt->second.lock();
				if (currentCachedValue)
				{
					return currentCachedValue;
				}
			}

			// Collect Removed Value
			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				Cache_Level_4_Type::RealType::iterator level4_findIt = m_cache.element.find(key.keys[4]);
				if (level4_findIt == m_cache.element.end())
				{
					return nullptr;
				}

				Cache_Level_3_Type::RealType::iterator level3_findIt = level4_findIt->second.element.find(key.keys[3]);
				if (level3_findIt == level4_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_2_Type::RealType::iterator level2_findIt = level3_findIt->second.element.find(key.keys[2]);
				if (level2_findIt == level3_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_1_Type::RealType::iterator level1_findIt = level2_findIt->second.element.find(key.keys[1]);
				if (level1_findIt == level2_findIt->second.element.end())
				{
					return nullptr;
				}

				Cache_Level_0_Type::RealType::iterator level0_findIt = level1_findIt->second.element.find(key.keys[0]);
				if (level0_findIt == level1_findIt->second.element.end())
				{
					return nullptr;
				}

				std::shared_ptr<CachedGlyph> currentCachedValue = level0_findIt->second.lock();
				if (currentCachedValue)
				{
					return currentCachedValue;
				}

				level1_findIt->second.element.erase(level0_findIt);
				if (level1_findIt->second.element.empty())
				{
					level2_findIt->second.element.erase(level1_findIt);
					if (level2_findIt->second.element.empty())
					{
						level3_findIt->second.element.erase(level2_findIt);
						if (level3_findIt->second.element.empty())
						{
							level4_findIt->second.element.erase(level3_findIt);
							if (level4_findIt->second.element.empty())
							{
								m_cache.element.erase(level4_findIt);
							}
						}
					}
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		void GlyphSearchCache::WriteFindResult(std::shared_ptr<CachedGlyph> foundGlyph,
			Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			_CacheKey key(atlasGroupId, fontId, glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
			std::weak_ptr<CachedGlyph> foundGlyphReference(foundGlyph);

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				Cache_Level_4_Type::RealType::iterator level4_findIt = m_cache.element.find(key.keys[4]);
				if (level4_findIt == m_cache.element.end())
				{
					m_cache.element[key.keys[4]] = Cache_Level_3_Type({ { { key.keys[3], Cache_Level_2_Type({ { { key.keys[2], Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], foundGlyphReference } } }) } } }) } } }) } } });
				}
				else
				{
					Cache_Level_3_Type::RealType::iterator level3_findIt = level4_findIt->second.element.find(key.keys[3]);
					if (level3_findIt == level4_findIt->second.element.end())
					{
						level4_findIt->second.element[key.keys[3]] = Cache_Level_2_Type({ { { key.keys[2], Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], foundGlyphReference } } }) } } }) } } });
					}
					else
					{
						Cache_Level_2_Type::RealType::iterator level2_findIt = level3_findIt->second.element.find(key.keys[2]);
						if (level2_findIt == level3_findIt->second.element.end())
						{
							level3_findIt->second.element[key.keys[2]] = Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], foundGlyphReference } } }) } } });
						}
						else
						{
							Cache_Level_1_Type::RealType::iterator level1_findIt = level2_findIt->second.element.find(key.keys[1]);
							if (level1_findIt == level2_findIt->second.element.end())
							{
								level2_findIt->second.element[key.keys[1]] = Cache_Level_0_Type({ { { key.keys[0], foundGlyphReference } } });
							}
							else
							{
								level1_findIt->second.element[key.keys[0]] = foundGlyphReference;
							}
						}
					}
				}
			}
			else
			{
				// Do not register without lock
			}
		}

		//------------------------------------------------------------------------------
		GlyphSearchCache::_CacheKey::_CacheKey(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			keys[0] = ((uint64_t)((uint32_t)atlasGroupId) << 32) | (uint64_t)((uint32_t)fontId);
			keys[1] = ((uint64_t)((uint32_t)(strokeWidth / _Internal::kFontSizeDetail)) << 32) | (uint64_t)((uint32_t)strokeColor.ToColorCodeARGB());
			keys[2] = ((uint64_t)((uint32_t)color.ToColorCodeARGB()) << 32) | (uint64_t)((uint32_t)(horizontalRatio / _Internal::kFontSizeRatioDetail));
			keys[3] = ((uint64_t)((uint32_t)textureType) << 32) | (uint64_t)((uint32_t)style);
			keys[4] = ((uint64_t)((uint32_t)glyph) << 32) | (uint64_t)((uint32_t)(size / _Internal::kFontSizeDetail));
		}
	}
}
