////////////////////////////////////////////////////////////////////////////////
// Common/GlyphCache.cpp (Leggiero/Modules - Font)
//
// Glyph Cache Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GlyphCache.h"

// External Library
#include <errno.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Font
#include "../_Internal/_FontInternalUtility.h"
#include "IFontFace.h"
#include "CachedGlyph.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// GlyphCache

		//------------------------------------------------------------------------------
		GlyphCache::GlyphCache()
		{
		}

		//------------------------------------------------------------------------------
		GlyphCache::~GlyphCache()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphCache::ReadGlyph(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
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

				return level0_findIt->second;
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CachedGlyph> GlyphCache::WriteLoadedGlyph(bool isDrawSuccess, GLint offsetX, GLint offsetY, int advanceX, int advanceY, std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image, GlyphCodeInFont glyphCode, bool isManualBold, bool isManualItalic,
			Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			_CacheKey key(atlasGroupId, font->GetFontId(), glyph, size, horizontalRatio, style, color, textureType, strokeWidth, strokeColor);
			std::shared_ptr<CachedGlyph> createdEntry(new CachedGlyph(isDrawSuccess, offsetX, offsetY, advanceX, advanceY, image, font, glyphCode, horizontalRatio, isManualBold, isManualItalic));

			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				Cache_Level_4_Type::RealType::iterator level4_findIt = m_cache.element.find(key.keys[4]);
				if (level4_findIt == m_cache.element.end())
				{
					m_cache.element[key.keys[4]] = Cache_Level_3_Type({ { { key.keys[3], Cache_Level_2_Type({ { { key.keys[2], Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], createdEntry } } }) } } }) } } }) } } });
				}
				else
				{
					Cache_Level_3_Type::RealType::iterator level3_findIt = level4_findIt->second.element.find(key.keys[3]);
					if (level3_findIt == level4_findIt->second.element.end())
					{
						level4_findIt->second.element[key.keys[3]] = Cache_Level_2_Type({ { { key.keys[2], Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], createdEntry } } }) } } }) } } });
					}
					else
					{
						Cache_Level_2_Type::RealType::iterator level2_findIt = level3_findIt->second.element.find(key.keys[2]);
						if (level2_findIt == level3_findIt->second.element.end())
						{
							level3_findIt->second.element[key.keys[2]] = Cache_Level_1_Type({ { { key.keys[1], Cache_Level_0_Type({ { { key.keys[0], createdEntry } } }) } } });
						}
						else
						{
							Cache_Level_1_Type::RealType::iterator level1_findIt = level2_findIt->second.element.find(key.keys[1]);
							if (level1_findIt == level2_findIt->second.element.end())
							{
								level2_findIt->second.element[key.keys[1]] = Cache_Level_0_Type({ { { key.keys[0], createdEntry } } });
							}
							else
							{
								level1_findIt->second.element[key.keys[0]] = createdEntry;
							}
						}
					}
				}
			}
			else
			{
				// Do not register without lock
			}

			return createdEntry;
		}

		//------------------------------------------------------------------------------
		GlyphCache::_CacheKey::_CacheKey(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
			float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor)
		{
			keys[0] = ((uint64_t)((uint32_t)atlasGroupId) << 32) | (uint64_t)((uint32_t)fontId);
			keys[1] = ((uint64_t)((uint32_t)(strokeWidth / _Internal::kFontSizeDetail)) << 32) | (uint64_t)((uint32_t)strokeColor.ToColorCodeARGB());
			keys[2] = ((uint64_t)((uint32_t)color.ToColorCodeARGB()) << 32) | (uint64_t)((uint32_t)(horizontalRatio / _Internal::kFontSizeRatioDetail));
			keys[3] = ((uint64_t)((uint32_t)textureType) << 32) | (uint64_t)((uint32_t)style);
			keys[4] = ((uint64_t)((uint32_t)glyph) << 32) | (uint64_t)((uint32_t)(size / _Internal::kFontSizeDetail));
		}

		//------------------------------------------------------------------------------
		void GlyphCache::Clear()
		{
			int lockResult = pthread_rwlock_wrlock(&m_cacheLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_cacheLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				for (Cache_Level_4_Type::RealType::iterator level4_It = m_cache.element.begin(); level4_It != m_cache.element.end(); ++level4_It)
				{
					for (Cache_Level_3_Type::RealType::iterator level3_It = level4_It->second.element.begin(); level3_It != level4_It->second.element.end(); ++level3_It)
					{
						for (Cache_Level_2_Type::RealType::iterator level2_It = level3_It->second.element.begin(); level2_It != level3_It->second.element.end(); ++level2_It)
						{
							for (Cache_Level_1_Type::RealType::iterator level1_It = level2_It->second.element.begin(); level1_It != level2_It->second.element.end(); ++level1_It)
							{
								level1_It->second.element.clear();
							}
							level2_It->second.element.clear();
						}
						level3_It->second.element.clear();
					}
					level4_It->second.element.clear();
				}
				m_cache.element.clear();
			}
			else
			{
				// Anyway, GO
				for (Cache_Level_4_Type::RealType::iterator level4_It = m_cache.element.begin(); level4_It != m_cache.element.end(); ++level4_It)
				{
					for (Cache_Level_3_Type::RealType::iterator level3_It = level4_It->second.element.begin(); level3_It != level4_It->second.element.end(); ++level3_It)
					{
						for (Cache_Level_2_Type::RealType::iterator level2_It = level3_It->second.element.begin(); level2_It != level3_It->second.element.end(); ++level2_It)
						{
							for (Cache_Level_1_Type::RealType::iterator level1_It = level2_It->second.element.begin(); level1_It != level2_It->second.element.end(); ++level1_It)
							{
								level1_It->second.element.clear();
							}
							level2_It->second.element.clear();
						}
						level3_It->second.element.clear();
					}
					level4_It->second.element.clear();
				}
				m_cache.element.clear();
			}
		}
	}
}
