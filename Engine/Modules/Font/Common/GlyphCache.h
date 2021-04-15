////////////////////////////////////////////////////////////////////////////////
// Common/GlyphCache.h (Leggiero/Modules - Font)
//
// Glyph Cache that stores loaded glyphs
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__GLYPH_CACHE_H
#define __LM_FONT__COMMON__GLYPH_CACHE_H


// Standard Library
#include <memory>
#include <unordered_map>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Texture/RuntimeTextureAtlasManager.h>

// Leggiero.Font
#include "FontTypes.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class RuntimeTextureAtlasEntry;
	}


	namespace Font
	{
		// Forward Declaration
		class IFontFace;
		class CachedGlyph;


		// Glyph Cache
		class GlyphCache
		{
		public:
			GlyphCache();
			virtual ~GlyphCache();

		public:
			std::shared_ptr<CachedGlyph> ReadGlyph(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor);

			std::shared_ptr<CachedGlyph> WriteLoadedGlyph(bool isDrawSuccess, GLint offsetX, GLint offsetY, int advanceX, int advanceY, std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image, GlyphCodeInFont glyphCode, bool isManualBold, bool isManualItalic,
				Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, std::shared_ptr<IFontFace> font, GlyphCodePointType glyph, GlyphSizeType size,
				float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor);

			void Clear();

		protected:
			// Cache Saving Key
			struct _CacheKey
			{
			public:
				using InternalKeyType = uint64_t;
				static constexpr int kKeyCount = 5;

			public:
				InternalKeyType keys[kKeyCount];

			public:
				_CacheKey(Graphics::RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroupId, FontIdType fontId, GlyphCodePointType glyph, GlyphSizeType size,
					float horizontalRatio, GlyphStyleType style, Graphics::GLByteARGB color, TextureType textureType, float strokeWidth, Graphics::GLByteARGB strokeColor);
			};

		protected:
			Utility::Threading::SafePthreadRWLock m_cacheLock;	//note: can be changed to fine-grained lock for performance

			// short-name struct defines to avoid C4503 warning
			struct Cache_Level_0_Type
			{
				std::unordered_map <_CacheKey::InternalKeyType, std::shared_ptr<CachedGlyph> > element;
				using RealType = decltype(element);
			};
			struct Cache_Level_1_Type
			{
				std::unordered_map <_CacheKey::InternalKeyType, Cache_Level_0_Type> element;
				using RealType = decltype(element);
			};
			struct Cache_Level_2_Type
			{
				std::unordered_map <_CacheKey::InternalKeyType, Cache_Level_1_Type> element;
				using RealType = decltype(element);
			};
			struct Cache_Level_3_Type
			{
				std::unordered_map <_CacheKey::InternalKeyType, Cache_Level_2_Type> element;
				using RealType = decltype(element);
			};
			struct Cache_Level_4_Type
			{
				std::unordered_map <_CacheKey::InternalKeyType, Cache_Level_3_Type> element;
				using RealType = decltype(element);
			};
			Cache_Level_4_Type m_cache;
		};
	}
}

#endif
