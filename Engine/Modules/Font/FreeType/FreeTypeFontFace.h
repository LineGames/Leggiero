////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeFontFace.h (Leggiero/Modules - Font)
//
// Font Face of FreeType
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__FREE_TYPE__FREE_TYPE_FONT_FACE_H
#define __LM_FONT__FREE_TYPE__FREE_TYPE_FONT_FACE_H


// Standard Library
#include <functional>
#include <iostream>
#include <memory>
#include <string>

// External Library
#include <ft2build.h>
#include FT_FREETYPE_H
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "../Common/FontTypes.h"
#include "../Common/IFontFace.h"


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			// Foward Declaration for Creation Functions
			class FontFace;


			// Creation Functions
			std::shared_ptr<FontFace> CreateFontFaceFromFile(std::string filePath, FT_Long faceIndex = 0);
			std::shared_ptr<FontFace> CreateFontFaceFromMemory(const char *buffer, size_t size, FT_Long faceIndex = 0);
			std::shared_ptr<FontFace> CreateFontFaceFromMemory(std::istream &dataStream, std::streamsize size = (std::streamsize)0, FT_Long faceIndex = 0);


			// A Font Face
			class FontFace
				: public IFontFace
				, public std::enable_shared_from_this<FontFace>
			{
				friend std::shared_ptr<FontFace> CreateFontFaceFromFile(std::string, FT_Long);
				friend std::shared_ptr<FontFace> CreateFontFaceFromMemory(const char *, size_t, FT_Long);
				friend std::shared_ptr<FontFace> CreateFontFaceFromMemory(std::istream &, std::streamsize, FT_Long);

			public:
				virtual ~FontFace();

			protected:
				FontFace(FT_Face faceHandle, std::function<void()> resourceCollector, std::function<FT_Error(FT_Face &)> faceCreator);

			private:
				FontFace() = delete;
				FontFace(const FontFace &rhs) = delete;
				FontFace &operator=(const FontFace& rhs) = delete;

			public:
				// Instance for real usable face object
				class FaceInstance
				{
					friend FontFace;

				public:
					virtual ~FaceInstance();

				protected:
					FaceInstance(FT_Face faceHandle, std::shared_ptr<FontFace> ownerFace);

				private:
					FaceInstance() = delete;
					FaceInstance(const FaceInstance &rhs) = delete;
					FaceInstance &operator=(const FaceInstance& rhs) = delete;

				public:
					FT_Face		GetFaceHandle() const { return m_faceHandle; }
					FontIdType	GetFontIdOf() const { return m_ownerFace->GetFontId(); }

				protected:
					FT_Face m_faceHandle;
					std::shared_ptr<FontFace> m_ownerFace;
				};
				friend class FaceInstance;

			public:	// IFontFace
				// Find Glyph Code in the Font Face
				virtual GlyphCodeInFont FindGlyphCodeInFont(GlyphCodePointType glyph) override;

				// Calculate Kerning between given Two Characters
				virtual Graphics::CoordVector2D GetKerningAmount(GlyphCodeInFont lastGlyphIndex, GlyphCodeInFont currentGlyphIndex, GlyphSizeType size, float horizontalRatio, bool isManualBold, bool isManualItalic) override;

				// Get the Font Face's Backend Engine
				virtual FontBackendType GetFontBackendType() const override { return FontBackendType::kFreeType; }

				// Get Test Drawer for the Font
				virtual IGlyphDrawer *GetBasicTestDrawer() override;

			public:
				// Get Face Instance to use font face thread-safely
				std::shared_ptr<FaceInstance>	UseFace();

			protected:
				void _ReturnFaceResource(FT_Face faceHandle);
				
			protected:
				moodycamel::ConcurrentQueue<FT_Face>	m_faceHandles;

				std::function<void()>					m_resourceCollector;
				std::function<FT_Error(FT_Face &)>		m_faceCreator;
			};
		}
	}
}

#endif
