////////////////////////////////////////////////////////////////////////////////
// Common/IFontFace.h (Leggiero/Modules - Font)
//
// Definitions of Abstract Font Face Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__I_FONT_FACE_H
#define __LM_FONT__COMMON__I_FONT_FACE_H


// Standard Library
#include <memory>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsTypes.h>

// Leggiero.Font
#include "FontTypes.h"
#include "FontBackend.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declaration
		class IGlyphDrawer;


		// Abstract Font
		class IFontFace
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			IFontFace();
			virtual ~IFontFace() { }

		public:
			// Get Unique Font Id
			FontIdType GetFontId() const { return m_fontId; }

			// Find Glyph Code in the Font Face
			virtual GlyphCodeInFont FindGlyphCodeInFont(GlyphCodePointType glyph) { return kGlyphCode_INVALID; }

			// Calculate Kerning between given Two Characters
			virtual Graphics::CoordVector2D GetKerningAmount(GlyphCodeInFont lastGlyphIndex, GlyphCodeInFont currentGlyphIndex, GlyphSizeType size, float horizontalRatio, bool isManualBold, bool isManualItalic) { return Graphics::CoordVector2D::kZero; }

			// Get the Font Face's Backend Engine
			virtual FontBackendType GetFontBackendType() const { return FontBackendType::kINVALID; }

			// Get Test Drawer for the Font
			virtual IGlyphDrawer *GetBasicTestDrawer() { return nullptr; }

		protected:
			FontIdType m_fontId;
		};
	}
}

#endif
