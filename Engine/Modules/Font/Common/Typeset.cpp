////////////////////////////////////////////////////////////////////////////////
// Common/Typeset.cpp (Leggiero/Modules - Font)
//
// Typeset Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Typeset.h"

// Standard Library
#include <cmath>
#include <tuple>
#include <vector>

// External Library
#include <utfcpp/utf8.h>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>

// Leggiero.Graphics
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Texture/RuntimeTextureAtlasEntry.h>

// Leggiero.Font
#include "../GlyphManagerComponent.h"
#include "IFontFace.h"
#include "CachedGlyph.h"
#include "MultiPageFontSetting.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// Utility Functions

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			void _SplitLinesForTypeSetting(std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> &singleLinedTextBlock, bool isIgnoreLineBreak, std::vector<bool> &isLineForcedBreaked, std::vector<std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > > &lineBuffers, bool &isLastLineForceBreaked, Leggiero::Graphics::BasicCoordType blockWidth);
			void _SplitLinesForTypeSettingWordAware(std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> &singleLinedTextBlock, bool isIgnoreLineBreak, std::vector<bool> &isLineForcedBreaked, std::vector<std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > > &lineBuffers, bool &isLastLineForceBreaked, Leggiero::Graphics::BasicCoordType blockWidth);

			//------------------------------------------------------------------------------
			std::shared_ptr<CachedGlyphTypesetting> TypesetHorizontalBlockFromSingleLineTypesetting(std::shared_ptr<CachedGlyphTypesetting> singleLinedTextBlock,
				GlyphSizeType size, Graphics::BasicCoordType blockWidth, TextAlignType textAlign, TextAlignType lastLineTextAlign, float lineSpacing,
				Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign, bool isFitGrid, 
				bool isWordAwareLineBreak, bool isIgnoreLineBreak)
			{
				// Split as lines
				bool isLastLineForceBreaked = false;
				std::vector<std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > > lineBuffers;
				lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());
				std::vector<bool> isLineForcedBreaked;
				if (isWordAwareLineBreak)
				{
					_SplitLinesForTypeSettingWordAware(singleLinedTextBlock, isIgnoreLineBreak, isLineForcedBreaked, lineBuffers, isLastLineForceBreaked, blockWidth);
				}
				else
				{
					_SplitLinesForTypeSetting(singleLinedTextBlock, isIgnoreLineBreak, isLineForcedBreaked, lineBuffers, isLastLineForceBreaked, blockWidth);
				}

				if (!lineBuffers.empty() && lineBuffers.back().empty() && !isLastLineForceBreaked)
				{
					isLineForcedBreaked.pop_back();
					lineBuffers.pop_back();
					if (!isLineForcedBreaked.empty())
					{
						isLineForcedBreaked.back() = true;
					}
				}
				if (lineBuffers.size() <= 1)
				{
					// Single Line
					return singleLinedTextBlock;
				}

				if (textAlign == TextAlignType::kDefault)
				{
					textAlign = TextAlignType::kLeft;
				}
				if (lastLineTextAlign == TextAlignType::kDefault)
				{
					lastLineTextAlign = textAlign;
				}

				// Typeset each lines
				std::shared_ptr<CachedGlyphTypesetting> resultTypesetting(std::make_shared<CachedGlyphTypesetting>());
				Graphics::BasicCoordType lineYSpace = static_cast<Graphics::BasicCoordType>(size * lineSpacing);
				for (int lineCount = 0; lineCount < lineBuffers.size(); ++lineCount)
				{
					Graphics::BasicCoordType lineYStart = static_cast<Graphics::BasicCoordType>(lineCount) * lineYSpace;
					if (isFitGrid)
					{
						lineYStart = static_cast<Graphics::BasicCoordType>(round(lineYStart));
					}

					std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > &currentLineBuffer = lineBuffers[lineCount];

					if (!isLineForcedBreaked[lineCount])
					{
						// Normal Line
						int lastNonWhitespaceIndex = -1;
						for (int i = (int)currentLineBuffer.size() - 1; i >= 0; --i)
						{
							if (currentLineBuffer[i]->glyphInformation->HasImage())
							{
								lastNonWhitespaceIndex = i;
								break;
							}
						}
						if (lastNonWhitespaceIndex == -1)
						{
							continue;
						}

						Graphics::BasicCoordType offsetX = static_cast<Graphics::BasicCoordType>(0.0f);
						Graphics::BasicCoordType offsetEachX = static_cast<Graphics::BasicCoordType>(0.0f);
						Graphics::BasicCoordType offsetSpaceX = static_cast<Graphics::BasicCoordType>(0.0f);
						switch (textAlign)
						{
							case TextAlignType::kDefault:
							case TextAlignType::kLeft:
								// Offset is default left
								break;

							case TextAlignType::kCenter:
								offsetX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) * 0.5f;
								break;

							case TextAlignType::kRight:
								offsetX = blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX());
								break;

							case TextAlignType::kJustify:
								if (lastNonWhitespaceIndex > 0)
								{
									offsetEachX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) / static_cast<Graphics::BasicCoordType>(lastNonWhitespaceIndex);
								}
								break;

							case TextAlignType::kJustifyWord:
								int whitespaceCount = 0;
								for (int i = 0; i < lastNonWhitespaceIndex; ++i)
								{
									if (!currentLineBuffer[i]->glyphInformation->HasImage())
									{
										++whitespaceCount;
									}
								}
								if (whitespaceCount > 0)
								{
									offsetSpaceX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) / static_cast<Graphics::BasicCoordType>(whitespaceCount);
								}
								break;
						}

						int currentPassedWhitespaceCount = 0;
						for (int i = 0; i <= lastNonWhitespaceIndex; ++i)
						{
							Graphics::BasicCoordType currentOffsetX = offsetX + offsetEachX * static_cast<Graphics::BasicCoordType>(i) + offsetSpaceX * static_cast<Graphics::BasicCoordType>(currentPassedWhitespaceCount);
							if (isFitGrid)
							{
								currentOffsetX = static_cast<Graphics::BasicCoordType>(round(currentOffsetX));
							}

							std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter = currentLineBuffer[i];
							currentCharacter->characterOrigin.y += lineYStart;
							currentCharacter->effectiveCharacterRect.top += lineYStart;
							currentCharacter->effectiveCharacterRect.bottom += lineYStart;

							currentCharacter->characterOrigin.x += currentOffsetX;
							currentCharacter->effectiveCharacterRect.left += currentOffsetX;
							currentCharacter->effectiveCharacterRect.right += currentOffsetX;

							resultTypesetting->characters.push_back(currentCharacter);

							if (!currentCharacter->glyphInformation->HasImage())
							{
								++currentPassedWhitespaceCount;
							}

							if (currentCharacter->effectiveCharacterRect.left < resultTypesetting->boundingBox.left)
							{
								resultTypesetting->boundingBox.left = currentCharacter->effectiveCharacterRect.left;
							}
							if (currentCharacter->effectiveCharacterRect.right > resultTypesetting->boundingBox.right)
							{
								resultTypesetting->boundingBox.right = currentCharacter->effectiveCharacterRect.right;
							}
						}
					}
					else
					{
						// Last or Force Breaked Line
						int lastNonWhitespaceIndex = -1;
						for (int i = (int)currentLineBuffer.size() - 1; i >= 0; --i)
						{
							if (currentLineBuffer[i]->glyphInformation->HasImage())
							{
								lastNonWhitespaceIndex = i;
								break;
							}
						}
						if (lastNonWhitespaceIndex == -1)
						{
							continue;
						}

						Graphics::BasicCoordType offsetX = static_cast<Graphics::BasicCoordType>(0.0f);
						Graphics::BasicCoordType offsetEachX = static_cast<Graphics::BasicCoordType>(0.0f);
						Graphics::BasicCoordType offsetSpaceX = static_cast<Graphics::BasicCoordType>(0.0f);
						switch (lastLineTextAlign)
						{
							case TextAlignType::kDefault:
							case TextAlignType::kLeft:
								// Offset is default left
								break;

							case TextAlignType::kCenter:
								offsetX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) * 0.5f;
								break;

							case TextAlignType::kRight:
								offsetX = blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX());
								break;

							case TextAlignType::kJustify:
								if (lastNonWhitespaceIndex > 0)
								{
									offsetEachX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) / static_cast<Graphics::BasicCoordType>(lastNonWhitespaceIndex);
								}
								break;

							case TextAlignType::kJustifyWord:
								int whitespaceCount = 0;
								for (int i = 0; i < lastNonWhitespaceIndex; ++i)
								{
									if (!currentLineBuffer[i]->glyphInformation->HasImage())
									{
										++whitespaceCount;
									}
								}
								if (whitespaceCount > 0)
								{
									offsetSpaceX = (blockWidth - (currentLineBuffer[lastNonWhitespaceIndex]->characterOrigin.x + currentLineBuffer[lastNonWhitespaceIndex]->glyphInformation->GetAdvanceX())) / static_cast<Graphics::BasicCoordType>(whitespaceCount);
								}
								break;
						}

						int currentPassedWhitespaceCount = 0;
						for (int i = 0; i <= lastNonWhitespaceIndex; ++i)
						{
							Graphics::BasicCoordType currentOffsetX = offsetX + offsetEachX * static_cast<Graphics::BasicCoordType>(i) + offsetSpaceX * static_cast<Graphics::BasicCoordType>(currentPassedWhitespaceCount);
							if (isFitGrid)
							{
								currentOffsetX = static_cast<Graphics::BasicCoordType>(round(currentOffsetX));
							}

							std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter = currentLineBuffer[i];
							currentCharacter->characterOrigin.y += lineYStart;
							currentCharacter->effectiveCharacterRect.top += lineYStart;
							currentCharacter->effectiveCharacterRect.bottom += lineYStart;

							currentCharacter->characterOrigin.x += currentOffsetX;
							currentCharacter->effectiveCharacterRect.left += currentOffsetX;
							currentCharacter->effectiveCharacterRect.right += currentOffsetX;

							resultTypesetting->characters.push_back(currentCharacter);

							if (!currentCharacter->glyphInformation->HasImage())
							{
								++currentPassedWhitespaceCount;
							}

							if (currentCharacter->effectiveCharacterRect.bottom > resultTypesetting->boundingBox.bottom)
							{
								resultTypesetting->boundingBox.bottom = currentCharacter->effectiveCharacterRect.bottom;
							}
							if (currentCharacter->effectiveCharacterRect.left < resultTypesetting->boundingBox.left)
							{
								resultTypesetting->boundingBox.left = currentCharacter->effectiveCharacterRect.left;
							}
							if (currentCharacter->effectiveCharacterRect.right > resultTypesetting->boundingBox.right)
							{
								resultTypesetting->boundingBox.right = currentCharacter->effectiveCharacterRect.right;
							}
						}
					}
				}

				// Align
				Graphics::BasicCoordType offsetX = resultTypesetting->boundingBox.left;
				Graphics::BasicCoordType offsetY = resultTypesetting->boundingBox.top;
				switch (horizontalAlign)
				{
					case Graphics::HorizontalAlignType::kNone:
					case Graphics::HorizontalAlignType::kLeft:
						// Offset is default left
						break;

					case Graphics::HorizontalAlignType::kCenter:
						offsetX += resultTypesetting->boundingBox.GetWidth() * 0.5f;
						break;

					case Graphics::HorizontalAlignType::kRight:
						offsetX += resultTypesetting->boundingBox.GetWidth();
						break;
				}
				switch (verticalAlign)
				{
					case Graphics::VerticalAlignType::kNone:
					case Graphics::VerticalAlignType::kTop:
						// Offset is default top
						break;

					case Graphics::VerticalAlignType::kCenter:
						offsetY += resultTypesetting->boundingBox.GetHeight() * 0.5f;
						break;

					case Graphics::VerticalAlignType::kBottom:
						offsetY += resultTypesetting->boundingBox.GetHeight();
						break;
				}
				if (isFitGrid)
				{
					offsetX = static_cast<Graphics::BasicCoordType>(round(offsetX));
					offsetY = static_cast<Graphics::BasicCoordType>(round(offsetY));
				}
				if (offsetX != static_cast<Graphics::BasicCoordType>(0.0f) || offsetY != static_cast<Graphics::BasicCoordType>(0.0f))
				{
					for (std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter : resultTypesetting->characters)
					{
						currentCharacter->characterOrigin.x -= offsetX;
						currentCharacter->characterOrigin.y -= offsetY;
						currentCharacter->effectiveCharacterRect.left -= offsetX;
						currentCharacter->effectiveCharacterRect.right -= offsetX;
						currentCharacter->effectiveCharacterRect.top -= offsetY;
						currentCharacter->effectiveCharacterRect.bottom -= offsetY;
					}
					resultTypesetting->boundingBox.left -= offsetX;
					resultTypesetting->boundingBox.right -= offsetX;
					resultTypesetting->boundingBox.top -= offsetY;
					resultTypesetting->boundingBox.bottom -= offsetY;
				}

				resultTypesetting->baselineOffset = singleLinedTextBlock->baselineOffset - offsetY;
				resultTypesetting->logicalHeight = static_cast<Graphics::BasicCoordType>(lineBuffers.size()) * lineYSpace;
				resultTypesetting->firstLineLogicalHeight = singleLinedTextBlock->logicalHeight;
				if (isFitGrid)
				{
					resultTypesetting->logicalHeight = static_cast<Graphics::BasicCoordType>(ceil(resultTypesetting->logicalHeight));
				}

				return resultTypesetting;
			}

			//------------------------------------------------------------------------------
			void _SplitLinesForTypeSetting(std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> &singleLinedTextBlock, bool isIgnoreLineBreak, std::vector<bool> &isLineForcedBreaked, std::vector<std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > > &lineBuffers, bool &isLastLineForceBreaked, Leggiero::Graphics::BasicCoordType blockWidth)
			{
				Graphics::BasicCoordType lineOffset = static_cast<Graphics::BasicCoordType>(0.0f);
				for (std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter : singleLinedTextBlock->characters)
				{
					if (!isIgnoreLineBreak && currentCharacter->isLineBreak)
					{
						isLineForcedBreaked.push_back(true);
						lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());
						isLastLineForceBreaked = true;
						continue;
					}

					std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > &currentLineBuffer = lineBuffers.back();
					if (currentLineBuffer.empty())
					{
						if (!currentCharacter->glyphInformation->HasImage() && (lineBuffers.size() > 1 && !isLastLineForceBreaked))
						{
							// Trim Heading Whitespaces except the first line
							continue;
						}
						isLastLineForceBreaked = false;
						lineOffset = currentCharacter->characterOrigin.x;
						currentCharacter->effectiveCharacterRect.left -= lineOffset;
						currentCharacter->effectiveCharacterRect.right -= lineOffset;
						currentCharacter->characterOrigin.x -= lineOffset;
						currentLineBuffer.push_back(currentCharacter);
						continue;
					}
					isLastLineForceBreaked = false;

					if (currentCharacter->characterOrigin.x + currentCharacter->glyphInformation->GetAdvanceX() - lineOffset > blockWidth)
					{
						isLineForcedBreaked.push_back(false);
						lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());
						if (currentCharacter->glyphInformation->HasImage())
						{
							lineOffset = currentCharacter->characterOrigin.x;
							currentCharacter->effectiveCharacterRect.left -= lineOffset;
							currentCharacter->effectiveCharacterRect.right -= lineOffset;
							currentCharacter->characterOrigin.x -= lineOffset;
							lineBuffers.back().push_back(currentCharacter);
						}
						continue;
					}

					currentCharacter->effectiveCharacterRect.left -= lineOffset;
					currentCharacter->effectiveCharacterRect.right -= lineOffset;
					currentCharacter->characterOrigin.x -= lineOffset;
					currentLineBuffer.push_back(currentCharacter);
				}
				isLineForcedBreaked.push_back(true);	// Consider last line as forced breaked line
			}

			//------------------------------------------------------------------------------
			void _SplitLinesForTypeSettingWordAware(std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> &singleLinedTextBlock, bool isIgnoreLineBreak, std::vector<bool> &isLineForcedBreaked, std::vector<std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > > &lineBuffers, bool &isLastLineForceBreaked, Leggiero::Graphics::BasicCoordType blockWidth)
			{
				Graphics::BasicCoordType lineOffset = static_cast<Graphics::BasicCoordType>(0.0f);
				int currentWordStart = -1;
				for (std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter : singleLinedTextBlock->characters)
				{
					if (!isIgnoreLineBreak && currentCharacter->isLineBreak)
					{
						isLineForcedBreaked.push_back(true);
						lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());
						currentWordStart = -1;
						isLastLineForceBreaked = true;
						continue;
					}

					std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > *pCurrentLineBuffer = &lineBuffers.back();
					if (pCurrentLineBuffer->empty())
					{
						if (!currentCharacter->glyphInformation->HasImage() && (lineBuffers.size() > 1 && !isLastLineForceBreaked))
						{
							// Trim Heading Whitespaces except the first line
							continue;
						}
						isLastLineForceBreaked = false;
						lineOffset = currentCharacter->characterOrigin.x;
						currentCharacter->effectiveCharacterRect.left -= lineOffset;
						currentCharacter->effectiveCharacterRect.right -= lineOffset;
						currentCharacter->characterOrigin.x -= lineOffset;
						pCurrentLineBuffer->push_back(currentCharacter);
						if (currentCharacter->glyphInformation->HasImage())
						{
							currentWordStart = 0;
						}
						continue;
					}
					isLastLineForceBreaked = false;

					if (currentCharacter->characterOrigin.x + currentCharacter->glyphInformation->GetAdvanceX() - lineOffset > blockWidth && currentWordStart > 0)
					{
						// Move word to next line
						Graphics::BasicCoordType inLineOffset = (*pCurrentLineBuffer)[currentWordStart]->characterOrigin.x;

						isLineForcedBreaked.push_back(false);
						lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());

						std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> > &movingLineBuffer = lineBuffers.back();
						pCurrentLineBuffer = &lineBuffers[lineBuffers.size() - 2];

						int movingLength = static_cast<int>(pCurrentLineBuffer->size()) - currentWordStart;
						for (int movingIndex = 0; movingIndex < movingLength; ++movingIndex)
						{
							movingLineBuffer.push_back((*pCurrentLineBuffer)[currentWordStart + movingIndex]);
							movingLineBuffer.back()->effectiveCharacterRect.left -= inLineOffset;
							movingLineBuffer.back()->effectiveCharacterRect.right -= inLineOffset;
							movingLineBuffer.back()->characterOrigin.x -= inLineOffset;
						}
						pCurrentLineBuffer->resize(currentWordStart);
						lineOffset += inLineOffset;
						currentWordStart = 0;
						pCurrentLineBuffer = &lineBuffers.back();
					}
					if (currentCharacter->characterOrigin.x + currentCharacter->glyphInformation->GetAdvanceX() - lineOffset > blockWidth)
					{
						isLineForcedBreaked.push_back(false);
						lineBuffers.push_back(std::vector<std::shared_ptr<CachedGlyphTypesetting::CharacterType> >());
						currentWordStart = -1;
						if (currentCharacter->glyphInformation->HasImage())
						{
							lineOffset = currentCharacter->characterOrigin.x;
							currentCharacter->effectiveCharacterRect.left -= lineOffset;
							currentCharacter->effectiveCharacterRect.right -= lineOffset;
							currentCharacter->characterOrigin.x -= lineOffset;
							lineBuffers.back().push_back(currentCharacter);
							currentWordStart = 0;
						}
						continue;
					}

					currentCharacter->effectiveCharacterRect.left -= lineOffset;
					currentCharacter->effectiveCharacterRect.right -= lineOffset;
					currentCharacter->characterOrigin.x -= lineOffset;
					pCurrentLineBuffer->push_back(currentCharacter);

					if (currentCharacter->glyphInformation->HasImage())
					{
						if (currentWordStart < 0)
						{
							currentWordStart = static_cast<int>(pCurrentLineBuffer->size()) - 1;
						}
					}
					else
					{
						// Whitespace Character
						currentWordStart = -1;
					}
				}
				isLineForcedBreaked.push_back(true);	// Consider last line as forced breaked line
			}

			//------------------------------------------------------------------------------
			void AlignHorizontalSingleLineTypesetting(std::shared_ptr<CachedGlyphTypesetting> toAlign, GlyphSizeType size, Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign, bool isFitGrid)
			{
				// Align
				Graphics::BasicCoordType offsetX = toAlign->boundingBox.left;
				Graphics::BasicCoordType offsetY = toAlign->boundingBox.top;
				switch (horizontalAlign)
				{
					case Graphics::HorizontalAlignType::kNone:
					case Graphics::HorizontalAlignType::kLeft:
						// Offset is default left
						break;

					case Graphics::HorizontalAlignType::kCenter:
						offsetX += toAlign->boundingBox.GetWidth() * 0.5f;
						break;

					case Graphics::HorizontalAlignType::kRight:
						offsetX += toAlign->boundingBox.GetWidth();
						break;
				}
				switch (verticalAlign)
				{
					case Graphics::VerticalAlignType::kNone:
					case Graphics::VerticalAlignType::kTop:
						// Offset is default top
						break;

					case Graphics::VerticalAlignType::kCenter:
						offsetY += toAlign->boundingBox.GetHeight() * 0.5f;
						break;

					case Graphics::VerticalAlignType::kBottom:
						offsetY += toAlign->boundingBox.GetHeight();
						break;
				}
				if (isFitGrid)
				{
					offsetX = static_cast<Graphics::BasicCoordType>(round(offsetX));
					offsetY = static_cast<Graphics::BasicCoordType>(round(offsetY));
				}
				if (offsetX != static_cast<Graphics::BasicCoordType>(0.0f) || offsetY != static_cast<Graphics::BasicCoordType>(0.0f))
				{
					for (std::shared_ptr<CachedGlyphTypesetting::CharacterType> &currentCharacter : toAlign->characters)
					{
						currentCharacter->characterOrigin.x -= offsetX;
						currentCharacter->characterOrigin.y -= offsetY;
						currentCharacter->effectiveCharacterRect.left -= offsetX;
						currentCharacter->effectiveCharacterRect.right -= offsetX;
						currentCharacter->effectiveCharacterRect.top -= offsetY;
						currentCharacter->effectiveCharacterRect.bottom -= offsetY;
					}
					toAlign->boundingBox.left -= offsetX;
					toAlign->boundingBox.right -= offsetX;
					toAlign->boundingBox.top -= offsetY;
					toAlign->boundingBox.bottom -= offsetY;
				}

				toAlign->baselineOffset = -offsetY;
				toAlign->logicalHeight = size;
				toAlign->firstLineLogicalHeight = size;
			}
		}

		//------------------------------------------------------------------------------
		TextAlignType ParseTextAlignType(const char *alignmentString)
		{
			if (alignmentString == nullptr)
			{
				return TextAlignType::kDefault;
			}

			if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "Justify")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "J"))
			{
				return TextAlignType::kJustify;
			}
			else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "L")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "Left"))
			{
				return TextAlignType::kLeft;
			}
			else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "C")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "Center"))
			{
				return TextAlignType::kCenter;
			}
			else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "R")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "Right"))
			{
				return TextAlignType::kRight;
			}
			else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "JustifyWord")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "JW")
				|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(alignmentString, "JustifyW"))
			{
				return TextAlignType::kJustifyWord;
			}

			return TextAlignType::kDefault;
		}


		//////////////////////////////////////////////////////////////////////////////// Cached / Horizontal Typeset Functions

		//------------------------------------------------------------------------------
		// Typeset one line horizontal text for given font set
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalSingleLine(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<IFontSet> fontSet, GlyphSizeType size, GlyphStyleType style,
			Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign,
			Graphics::GLByteARGB color, float strokeWidth, Graphics::GLByteARGB strokeColor,
			bool isFitGrid, TextureType textureType)
		{
			bool isStroked = false;
			if (strokeWidth > 0.0f && strokeColor.alpha > 0)
			{
				isStroked = true;
			}

			Graphics::GLByteARGB effectiveDrawColor = Graphics::GLByteARGB::kWhite;
			Graphics::GLByteARGB effectiveStrokeColor = strokeColor;
			Graphics::GLByteARGB multiplyColor = color;
			if (color.alpha == 255)
			{
				if (isStroked)
				{
					if (color == strokeColor)
					{
						effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
					}
					else if (strokeColor.red == 0 && strokeColor.green == 0 && strokeColor.blue == 0)
					{
						// Keep setting
					}
					else if (color.red == 0 && color.green == 0 && color.blue == 0)
					{
						multiplyColor = strokeColor;
						effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
						effectiveDrawColor = color;
					}
					else
					{
						effectiveDrawColor = color;
						effectiveStrokeColor = strokeColor;
						multiplyColor = Graphics::GLByteARGB::kWhite;
					}
				}
			}
			else
			{
				effectiveDrawColor = color;
				multiplyColor = Graphics::GLByteARGB::kWhite;
			}

			// Render Glyph
			std::vector<std::tuple<uint32_t, std::shared_ptr<CachedGlyph> > > glyphRenderResult;
			GameDataString::iterator characterIt = text.begin();
			while (characterIt != text.end())
			{
				uint32_t codepoint = 0;
				try
				{
					codepoint = utf8::next(characterIt, text.end());
				}
				catch (utf8::not_enough_room ex)
				{
					// String End
					break;
				}

				std::shared_ptr<CachedGlyph> renderedGlyph = glyphManager->LoadGlyph(fontSet, static_cast<GlyphCodePointType>(codepoint), size, 1.0f, style, effectiveDrawColor, textureType, strokeWidth, effectiveStrokeColor);
				if (renderedGlyph)
				{
					glyphRenderResult.push_back(std::make_tuple(codepoint, renderedGlyph));
				}
			}

			// Typeset
			std::shared_ptr<CachedGlyphTypesetting> resultTypesetting(std::make_shared<CachedGlyphTypesetting>());
			Graphics::CoordVector2D currentOrigin(0.0f, 0.0f);
			std::shared_ptr<CachedGlyphTypesetting::CharacterType> lastCharacter;
			for (std::tuple<uint32_t, std::shared_ptr<CachedGlyph> > &currentGlyphRendering : glyphRenderResult)
			{
				std::shared_ptr<CachedGlyphTypesetting::CharacterType> currentCharacter(std::make_shared<CachedGlyphTypesetting::CharacterType>());
				currentCharacter->codepoint = static_cast<GlyphCodePointType>(std::get<0>(currentGlyphRendering));

				currentCharacter->isLineBreak = (currentCharacter->codepoint == 10);	// process newline char

				// Kerning
				currentCharacter->glyphInformation = std::get<1>(currentGlyphRendering);
				if (lastCharacter)
				{
					if (currentCharacter->glyphInformation->GetEffectiveFontFace()->GetFontId() == lastCharacter->glyphInformation->GetEffectiveFontFace()->GetFontId())
					{
						Graphics::CoordVector2D kerningAmount = currentCharacter->glyphInformation->GetEffectiveFontFace()->GetKerningAmount(lastCharacter->glyphInformation->GetGlyphCode(), currentCharacter->glyphInformation->GetGlyphCode(),
							size, currentCharacter->glyphInformation->GetHorizontalRatio(), currentCharacter->glyphInformation->IsManualBoldGlyph(), currentCharacter->glyphInformation->IsManualItalicGlyph());
						currentOrigin.x += kerningAmount.x;
					}
				}

				currentCharacter->characterOrigin = currentOrigin;
				if (currentCharacter->glyphInformation->HasImage())
				{
					std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image = currentCharacter->glyphInformation->GetFontImage();
					const Graphics::TextureRectSection &imageSection = image->GetTextureSection();
					currentCharacter->effectiveCharacterRect = Graphics::CoordBasicRect::FromPositionSize(currentOrigin.x + currentCharacter->glyphInformation->GetOffsetX(), currentOrigin.y - currentCharacter->glyphInformation->GetOffsetY(),
						static_cast<Graphics::BasicCoordType>(imageSection.GetPixelWidth()), static_cast<Graphics::BasicCoordType>(imageSection.GetPixelHeight()));
					if (currentCharacter->effectiveCharacterRect.left < resultTypesetting->boundingBox.left)
					{
						resultTypesetting->boundingBox.left = currentCharacter->effectiveCharacterRect.left;
					}
					if (currentCharacter->effectiveCharacterRect.top < resultTypesetting->boundingBox.top)
					{
						resultTypesetting->boundingBox.top = currentCharacter->effectiveCharacterRect.top;
					}
					if (currentCharacter->effectiveCharacterRect.right > resultTypesetting->boundingBox.right)
					{
						resultTypesetting->boundingBox.right = currentCharacter->effectiveCharacterRect.right;
					}
					if (currentCharacter->effectiveCharacterRect.bottom > resultTypesetting->boundingBox.bottom)
					{
						resultTypesetting->boundingBox.bottom = currentCharacter->effectiveCharacterRect.bottom;
					}
				}
				else
				{
					currentCharacter->effectiveCharacterRect = Graphics::CoordBasicRect::FromPositionSize(currentOrigin.x, currentOrigin.y, 0.0f, 0.0f);
				}
				currentCharacter->multiplyColor = multiplyColor;
				resultTypesetting->characters.push_back(currentCharacter);

				currentOrigin.x += static_cast<Graphics::BasicCoordType>(currentCharacter->glyphInformation->GetAdvanceX());
				if (isFitGrid)
				{
					currentOrigin.x = static_cast<Graphics::BasicCoordType>(round(currentOrigin.x));
				}

				if (currentOrigin.x > resultTypesetting->boundingBox.right)
				{
					resultTypesetting->boundingBox.right = currentOrigin.x;
				}

				lastCharacter = currentCharacter;
			}

			// Align
			_Internal::AlignHorizontalSingleLineTypesetting(resultTypesetting, size, horizontalAlign, verticalAlign, isFitGrid);

			return resultTypesetting;
		}

		//------------------------------------------------------------------------------
		// Typeset one line horizontal text for given multi-page font setting
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalSingleLine(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<MultiPageFontSetting> fontSettting, GlyphSizeType size,
			Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign,
			Graphics::GLByteARGB color, float strokeWidth, Graphics::GLByteARGB strokeColor,
			bool isFitGrid, TextureType textureType)
		{
			bool isStroked = false;
			if (strokeWidth > 0.0f && strokeColor.alpha > 0)
			{
				isStroked = true;
			}

			Graphics::GLByteARGB effectiveDrawColor = Graphics::GLByteARGB::kWhite;
			Graphics::GLByteARGB effectiveStrokeColor = strokeColor;
			Graphics::GLByteARGB multiplyColor = color;
			if (color.alpha == 255)
			{
				if (isStroked)
				{
					if (color == strokeColor)
					{
						effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
					}
					else if (strokeColor.red == 0 && strokeColor.green == 0 && strokeColor.blue == 0)
					{
						// Keep setting
					}
					else if (color.red == 0 && color.green == 0 && color.blue == 0)
					{
						multiplyColor = strokeColor;
						effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
						effectiveDrawColor = color;
					}
					else
					{
						effectiveDrawColor = color;
						effectiveStrokeColor = strokeColor;
						multiplyColor = Graphics::GLByteARGB::kWhite;
					}
				}
			}
			else
			{
				effectiveDrawColor = color;
				multiplyColor = Graphics::GLByteARGB::kWhite;
			}

			// Render Glyph
			std::vector<std::tuple<uint32_t, std::shared_ptr<CachedGlyph>, float, bool, float> > glyphRenderResult; // Codepoint, Glyph, Baseline Offset Ratio, Is Force Monospace, Monospace Ratio
			GameDataString::iterator characterIt = text.begin();
			while (characterIt != text.end())
			{
				uint32_t codepoint = 0;
				try
				{
					codepoint = utf8::next(characterIt, text.end());
				}
				catch (utf8::not_enough_room ex)
				{
					// String End
					break;
				}

				MultiPageFontSetting::FontSetting applyingSetting = fontSettting->GetSettingForGlyph(static_cast<GlyphCodePointType>(codepoint));
				if (!applyingSetting.IsValid())
				{
					applyingSetting = fontSettting->GetTofuSetting();
					if (!applyingSetting.IsValid())
					{
						continue;
					}
				}

				GlyphSizeType effectiveSize = static_cast<GlyphSizeType>(size * applyingSetting.scaleMultiply);
				GlyphStyleType effectiveStyle = GlyphStyleType::kNone;
				if (applyingSetting.isForceBold)
				{
					effectiveStyle |= GlyphStyleType::kBold;
				}
				if (applyingSetting.isForceItalic)
				{
					effectiveStyle |= GlyphStyleType::kItalic;
				}

				std::shared_ptr<CachedGlyph> renderedGlyph = glyphManager->LoadGlyph(applyingSetting.face, static_cast<GlyphCodePointType>(codepoint), effectiveSize, applyingSetting.horizontalRatioMultiply, effectiveStyle, effectiveDrawColor, textureType, strokeWidth, effectiveStrokeColor);
				if (renderedGlyph)
				{
					glyphRenderResult.push_back(std::make_tuple(codepoint, renderedGlyph, applyingSetting.baselineOffsetRatio, applyingSetting.isForceMonospacing, applyingSetting.monospacingWidthRatio));
				}
			}

			// Typeset
			std::shared_ptr<CachedGlyphTypesetting> resultTypesetting(std::make_shared<CachedGlyphTypesetting>());
			Graphics::CoordVector2D currentOrigin(0.0f, 0.0f);
			std::shared_ptr<CachedGlyphTypesetting::CharacterType> lastCharacter;
			for (std::tuple<uint32_t, std::shared_ptr<CachedGlyph>, float, bool, float> &currentGlyphRendering : glyphRenderResult)
			{
				std::shared_ptr<CachedGlyphTypesetting::CharacterType> currentCharacter(std::make_shared<CachedGlyphTypesetting::CharacterType>());
				currentCharacter->codepoint = static_cast<GlyphCodePointType>(std::get<0>(currentGlyphRendering));
				currentCharacter->glyphInformation = std::get<1>(currentGlyphRendering);

				currentCharacter->isLineBreak = (currentCharacter->codepoint == 10);	// process newline char

				bool isMonospacing = std::get<3>(currentGlyphRendering);
				Graphics::BasicCoordType advanceOffset = static_cast<Graphics::BasicCoordType>(0.0f);

				if (isMonospacing)
				{
					advanceOffset = static_cast<Graphics::BasicCoordType>(size * std::get<4>(currentGlyphRendering));
					currentCharacter->characterOrigin = currentOrigin;
					currentCharacter->characterOrigin.x += static_cast<Graphics::BasicCoordType>((advanceOffset - currentCharacter->glyphInformation->GetAdvanceX()) * 0.5f);
					if (isFitGrid)
					{
						currentCharacter->characterOrigin.x = static_cast<Graphics::BasicCoordType>(round(currentCharacter->characterOrigin.x));
					}
				}
				else
				{
					// Kerning
					if (lastCharacter)
					{
						if (currentCharacter->glyphInformation->GetEffectiveFontFace()->GetFontId() == lastCharacter->glyphInformation->GetEffectiveFontFace()->GetFontId())
						{
							Graphics::CoordVector2D kerningAmount = currentCharacter->glyphInformation->GetEffectiveFontFace()->GetKerningAmount(lastCharacter->glyphInformation->GetGlyphCode(), currentCharacter->glyphInformation->GetGlyphCode(),
								size, currentCharacter->glyphInformation->GetHorizontalRatio(), currentCharacter->glyphInformation->IsManualBoldGlyph(), currentCharacter->glyphInformation->IsManualItalicGlyph());
							currentOrigin.x += kerningAmount.x;
						}
					}

					advanceOffset = static_cast<Graphics::BasicCoordType>(currentCharacter->glyphInformation->GetAdvanceX());
					currentCharacter->characterOrigin = currentOrigin;
				}

				// Baseline Offset
				Graphics::BasicCoordType baselineOffset = static_cast<Graphics::BasicCoordType>(size * std::get<2>(currentGlyphRendering));
				currentCharacter->characterOrigin.y += baselineOffset;

				if (currentCharacter->glyphInformation->HasImage())
				{
					std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image = currentCharacter->glyphInformation->GetFontImage();
					const Graphics::TextureRectSection &imageSection = image->GetTextureSection();
					currentCharacter->effectiveCharacterRect = Graphics::CoordBasicRect::FromPositionSize(currentCharacter->characterOrigin.x + currentCharacter->glyphInformation->GetOffsetX(), currentCharacter->characterOrigin.y - currentCharacter->glyphInformation->GetOffsetY(),
						static_cast<Graphics::BasicCoordType>(imageSection.GetPixelWidth()), static_cast<Graphics::BasicCoordType>(imageSection.GetPixelHeight()));
					if (currentCharacter->effectiveCharacterRect.left < resultTypesetting->boundingBox.left)
					{
						resultTypesetting->boundingBox.left = currentCharacter->effectiveCharacterRect.left;
					}
					if (currentCharacter->effectiveCharacterRect.top < resultTypesetting->boundingBox.top)
					{
						resultTypesetting->boundingBox.top = currentCharacter->effectiveCharacterRect.top;
					}
					if (currentCharacter->effectiveCharacterRect.right > resultTypesetting->boundingBox.right)
					{
						resultTypesetting->boundingBox.right = currentCharacter->effectiveCharacterRect.right;
					}
					if (currentCharacter->effectiveCharacterRect.bottom > resultTypesetting->boundingBox.bottom)
					{
						resultTypesetting->boundingBox.bottom = currentCharacter->effectiveCharacterRect.bottom;
					}
				}
				else
				{
					currentCharacter->effectiveCharacterRect = Graphics::CoordBasicRect::FromPositionSize(currentCharacter->characterOrigin.x, currentCharacter->characterOrigin.y, 0.0f, 0.0f);
				}
				currentCharacter->multiplyColor = multiplyColor;
				resultTypesetting->characters.push_back(currentCharacter);

				currentOrigin.x += advanceOffset;

				if (isFitGrid)
				{
					currentOrigin.x = static_cast<Graphics::BasicCoordType>(round(currentOrigin.x));
				}

				if (currentOrigin.x > resultTypesetting->boundingBox.right)
				{
					resultTypesetting->boundingBox.right = currentOrigin.x;
				}

				lastCharacter = currentCharacter;
			}

			// Align
			_Internal::AlignHorizontalSingleLineTypesetting(resultTypesetting, size, horizontalAlign, verticalAlign, isFitGrid);

			return resultTypesetting;
		}

		//------------------------------------------------------------------------------
		// Typeset given width text block for given font set
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalBlock(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<IFontSet> fontSet, GlyphSizeType size, Graphics::BasicCoordType blockWidth, GlyphStyleType style,
			TextAlignType textAlign, TextAlignType lastLineTextAlign, float lineSpacing,
			Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign,
			Graphics::GLByteARGB color, float strokeWidth, Graphics::GLByteARGB strokeColor,
			bool isFitGrid, TextureType textureType, 
			bool isWordAwareLineBreak, bool isIgnoreLineBreak)
		{
			std::shared_ptr<CachedGlyphTypesetting> singleLinedTextBlock = TypesetCachedGlyphesHorizontalSingleLine(glyphManager, text, fontSet, size, style, Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop, color, strokeWidth, strokeColor, isFitGrid, textureType);
			if (!singleLinedTextBlock)
			{
				return singleLinedTextBlock;
			}

			return _Internal::TypesetHorizontalBlockFromSingleLineTypesetting(singleLinedTextBlock, size, blockWidth, textAlign, lastLineTextAlign, lineSpacing, horizontalAlign, verticalAlign, isFitGrid, isWordAwareLineBreak, isIgnoreLineBreak);
		}

		//------------------------------------------------------------------------------
		// Typeset given width text block for given multi-page font setting
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalBlock(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<MultiPageFontSetting> fontSettting, GlyphSizeType size, Graphics::BasicCoordType blockWidth,
			TextAlignType textAlign, TextAlignType lastLineTextAlign, float lineSpacing,
			Graphics::HorizontalAlignType horizontalAlign, Graphics::VerticalAlignType verticalAlign,
			Graphics::GLByteARGB color, float strokeWidth, Graphics::GLByteARGB strokeColor,
			bool isFitGrid, TextureType textureType, 
			bool isWordAwareLineBreak, bool isIgnoreLineBreak)
		{
			std::shared_ptr<CachedGlyphTypesetting> singleLinedTextBlock = TypesetCachedGlyphesHorizontalSingleLine(glyphManager, text, fontSettting, size, Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop, color, strokeWidth, strokeColor, isFitGrid, textureType);
			if (!singleLinedTextBlock)
			{
				return singleLinedTextBlock;
			}

			return _Internal::TypesetHorizontalBlockFromSingleLineTypesetting(singleLinedTextBlock, size, blockWidth, textAlign, lastLineTextAlign, lineSpacing, horizontalAlign, verticalAlign, isFitGrid, isWordAwareLineBreak, isIgnoreLineBreak);
		}
	}
}
