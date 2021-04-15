////////////////////////////////////////////////////////////////////////////////
// Common/Typesetting.h (Leggiero/Modules - Font)
//
// Typesetted text
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__TYPESETTING_H
#define __LM_FONT__COMMON__TYPESETTING_H


// Standard Library
#include <memory>
#include <vector>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsTypes.h>
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include "FontTypes.h"
#include "CachedGlyph.h"


namespace Leggiero
{
	namespace Font
	{
		// A character in a typesetted text
		template <typename GlyphInformationT>
		class TypesettingCharacter
		{
		public:
			using GlyphInformationType = GlyphInformationT;

		public:
			GlyphCodePointType codepoint;

			std::shared_ptr<GlyphInformationT> glyphInformation;

			Graphics::CoordVector2D		characterOrigin;
			Graphics::CoordBasicRect	effectiveCharacterRect;

			Graphics::GLByteARGB		multiplyColor;

			bool						isLineBreak;

		public:
			TypesettingCharacter()
				: characterOrigin(static_cast<Graphics::BasicCoordType>(0.0), static_cast<Graphics::BasicCoordType>(0.0))
				, isLineBreak(false)
			{ }
		};


		// A dependent substring (like ruby character, ..) in a typesetted text
		template <typename GlyphInformationType>
		class TypesettingDependentSubstring
		{
		public:
			// Start index of the target of the dependent string (inclusive)
			int originalStringBeginIndex;

			// Finish index of the target of the dependent string (exclusive)
			int originalStringEndIndex;

			std::vector<std::shared_ptr<TypesettingCharacter<GlyphInformationType> > > substringCharacters;
			std::vector<Graphics::CoordBasicRect>	boundingBoxes;
		};


		// A typesetted text
		template <typename GlyphInformationT>
		class Typesetting
		{
		public:
			using GlyphInformationType = GlyphInformationT;
			using CharacterType = TypesettingCharacter<GlyphInformationT>;
			using DependentSubstringType = TypesettingDependentSubstring<GlyphInformationT>;

		public:
			std::vector<std::shared_ptr<CharacterType> >			characters;
			std::vector<std::shared_ptr<DependentSubstringType> >	dependentStrings;

			Graphics::CoordBasicRect	boundingBox;
			Graphics::BasicCoordType	baselineOffset;
			Graphics::BasicCoordType	logicalHeight;
			Graphics::BasicCoordType	firstLineLogicalHeight;

		public:
			size_t GetLength() const { return characters.size(); }
		};


		// Typesetted text based on cached glyph images on texture atlas
		using CachedGlyphTypesetting = Typesetting<CachedGlyph>;
	}
}

#endif
