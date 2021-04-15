////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeNormalGlyphDrawer.cpp (Leggiero/Modules - Font)
//
// FreeType based Normal Glyph Drawer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FreeTypeNormalGlyphDrawer.h"

// Standard Library
#include <cmath>
#include <memory>

// External Library
#include FT_GLYPH_H
#include FT_IMAGE_H
#include FT_STROKER_H

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsReferenceState.h>

// Leggiero.Font
#include "FreeTypeFontCommon.h"
#include "FreeTypeFontFace.h"


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			//////////////////////////////////////////////////////////////////////////////// Utility Function

			namespace _Internal
			{
				bool FillBitmapFontImage(FT_Bitmap &sourceBitmap, Graphics::GLByteARGB::ColorCodeType color, void *imageBuffer, GLenum format, GLenum dataType, bool isPremultipliedAlpha)
				{
					if (format != GL_RGBA || dataType != GL_UNSIGNED_BYTE)
					{
						//TODO: Will be developed.... Not supporting now
						return false;
					}

					Graphics::GLByteARGB drawColor = Graphics::GLByteARGB::FromColorCodeARGB(color);
					float floatAlphaColor = (float)drawColor.alpha / 255.0f;

					unsigned char *imageByteBuffer = (unsigned char *)imageBuffer;
					int pixelDataSize = 4; // For now, GL_RGBA is only used

					switch (sourceBitmap.pixel_mode)
					{
						case FT_PIXEL_MODE_NONE:
						case FT_PIXEL_MODE_GRAY:
							{
								float grayDivisor = 1.0f;
								if (sourceBitmap.num_grays > 1)
								{
									grayDivisor = 1.0f / (float)(sourceBitmap.num_grays - 1);
								}

								for (unsigned int y = 0; y < sourceBitmap.rows; ++y)
								{
									unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * y;
									for (unsigned int x = 0; x < sourceBitmap.width; ++x)
									{
										unsigned char rawPixelValue = *(pCurrentRowBuffer + x);
										unsigned char *pPixel = imageByteBuffer + (x + sourceBitmap.width * y) * pixelDataSize;
										if (rawPixelValue == 0)
										{
											pPixel[0] = 0;
											pPixel[1] = 0;
											pPixel[2] = 0;
											pPixel[3] = 0;
										}
										else
										{
											pPixel[3] = (unsigned char)fmin(255.0f * floatAlphaColor * (float)rawPixelValue * grayDivisor, 255.0f);
											if (isPremultipliedAlpha)
											{
												pPixel[0] = (GLubyte)((int)drawColor.red * (int)pPixel[3] / 255);
												pPixel[1] = (GLubyte)((int)drawColor.green * (int)pPixel[3] / 255);
												pPixel[2] = (GLubyte)((int)drawColor.blue * (int)pPixel[3] / 255);
											}
											else
											{
												pPixel[0] = drawColor.red;
												pPixel[1] = drawColor.green;
												pPixel[2] = drawColor.blue;
											}
										}
									}
								}
							}
							break;

						case FT_PIXEL_MODE_MONO:
							{
								if (isPremultipliedAlpha)
								{
									drawColor.red = (GLubyte)((int)drawColor.red * (int)drawColor.alpha / 255);
									drawColor.green = (GLubyte)((int)drawColor.green * (int)drawColor.alpha / 255);
									drawColor.blue = (GLubyte)((int)drawColor.blue * (int)drawColor.alpha / 255);
								}

								for (unsigned int y = 0; y < sourceBitmap.rows; ++y)
								{
									unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * y;
									for (unsigned int x = 0; x < sourceBitmap.width; ++x)
									{
										bool isFilledBit = (((*(pCurrentRowBuffer + x / 8)) & (0x80 >> (x % 8))) != 0);
										unsigned char *pPixel = imageByteBuffer + (x + sourceBitmap.width * y) * pixelDataSize;
										if (isFilledBit)
										{
											pPixel[0] = drawColor.red;
											pPixel[1] = drawColor.green;
											pPixel[2] = drawColor.blue;
											pPixel[3] = drawColor.alpha;
										}
										else
										{
											pPixel[0] = 0;
											pPixel[1] = 0;
											pPixel[2] = 0;
											pPixel[3] = 0;
										}
									}
								}
							}
							break;

						default:
							// Not-Supporting Modes
							return false;
					}

					return true;
				}

				bool FillOffsetedBitmapFontImage(FT_Bitmap &sourceBitmap, Graphics::GLByteARGB::ColorCodeType color, void *imageBuffer,
					GLint backgroundWidth, GLint backgroundHeight, GLint xOffset, GLint yOffset,
					GLenum format, GLenum dataType, bool isPremultipliedAlpha)
				{
					if (format != GL_RGBA || dataType != GL_UNSIGNED_BYTE)
					{
						//TODO: Will be developed.... Not supporting now
						return false;
					}

					Graphics::GLByteARGB drawColor = Graphics::GLByteARGB::FromColorCodeARGB(color);
					float floatAlphaColor = (float)drawColor.alpha / 255.0f;

					unsigned char *imageByteBuffer = (unsigned char *)imageBuffer;
					int pixelDataSize = 4; // For now, GL_RGBA is only used

					switch (sourceBitmap.pixel_mode)
					{
						case FT_PIXEL_MODE_NONE:
						case FT_PIXEL_MODE_GRAY:
							{
								float grayDivisor = 1.0f;
								if (sourceBitmap.num_grays > 1)
								{
									grayDivisor = 1.0f / (float)(sourceBitmap.num_grays - 1);
								}

								bool isXImageCulled = false;
								unsigned int xStartIndex = 0;
								if (xOffset > 0)
								{
									xStartIndex = xOffset;
								}
								unsigned int xFinishIndex = backgroundWidth;
								if ((xOffset + (int)sourceBitmap.width) < backgroundWidth)
								{
									int signedFinishIndex = (xOffset + (int)sourceBitmap.width);
									if (signedFinishIndex > 0)
									{
										xFinishIndex = (unsigned int)signedFinishIndex;
									}
									else
									{
										// No Image
										isXImageCulled = true;
										for (unsigned int y = 0; y < (unsigned int)backgroundHeight; ++y)
										{
											memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, backgroundWidth * pixelDataSize);
										}
									}
								}

								if (!isXImageCulled)
								{
									for (unsigned int y = 0; y < (unsigned int)backgroundHeight; ++y)
									{
										int sourceY = y - yOffset;
										if (sourceY < 0 || sourceY >= (int)sourceBitmap.rows)
										{
											memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, backgroundWidth * pixelDataSize);
										}
										else
										{
											unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * sourceY;
											if (xStartIndex > 0)
											{
												memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, xStartIndex * pixelDataSize);
											}
											for (unsigned int x = xStartIndex; x < xFinishIndex; ++x)
											{
												int sourceX = x - xOffset;
												if (sourceX < 0)
												{
													unsigned char *pPixel = imageByteBuffer + (x + backgroundWidth * y) * pixelDataSize;
													pPixel[0] = 0;
													pPixel[1] = 0;
													pPixel[2] = 0;
													pPixel[3] = 0;
												}
												else
												{
													unsigned char rawPixelValue = *(pCurrentRowBuffer + sourceX);
													unsigned char *pPixel = imageByteBuffer + (x + backgroundWidth * y) * pixelDataSize;
													if (rawPixelValue == 0)
													{
														pPixel[0] = 0;
														pPixel[1] = 0;
														pPixel[2] = 0;
														pPixel[3] = 0;
													}
													else
													{
														pPixel[3] = (unsigned char)fmin(255.0f * floatAlphaColor * (float)rawPixelValue * grayDivisor, 255.0f);
														if (isPremultipliedAlpha)
														{
															pPixel[0] = (GLubyte)((int)drawColor.red * (int)pPixel[3] / 255);
															pPixel[1] = (GLubyte)((int)drawColor.green * (int)pPixel[3] / 255);
															pPixel[2] = (GLubyte)((int)drawColor.blue * (int)pPixel[3] / 255);
														}
														else
														{
															pPixel[0] = drawColor.red;
															pPixel[1] = drawColor.green;
															pPixel[2] = drawColor.blue;
														}
													}
												}
											}
											if (xFinishIndex < (unsigned int)backgroundWidth)
											{
												memset((unsigned char *)(imageByteBuffer + (xFinishIndex + backgroundWidth * y) * pixelDataSize), 0, (backgroundWidth - xFinishIndex) * pixelDataSize);
											}
										}
									}
								}
							}
							break;

						case FT_PIXEL_MODE_MONO:
							{
								if (isPremultipliedAlpha)
								{
									drawColor.red = (GLubyte)((int)drawColor.red * (int)drawColor.alpha / 255);
									drawColor.green = (GLubyte)((int)drawColor.green * (int)drawColor.alpha / 255);
									drawColor.blue = (GLubyte)((int)drawColor.blue * (int)drawColor.alpha / 255);
								}

								bool isXImageCulled = false;
								unsigned int xStartIndex = 0;
								if (xOffset > 0)
								{
									xStartIndex = xOffset;
								}
								unsigned int xFinishIndex = backgroundWidth;
								if ((xOffset + (int)sourceBitmap.width) < backgroundWidth)
								{
									int signedFinishIndex = (xOffset + (int)sourceBitmap.width);
									if (signedFinishIndex > 0)
									{
										xFinishIndex = (unsigned int)signedFinishIndex;
									}
									else
									{
										// No Image
										isXImageCulled = true;
										for (unsigned int y = 0; y < (unsigned int)backgroundHeight; ++y)
										{
											memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, backgroundWidth * pixelDataSize);
										}
									}
								}

								if (!isXImageCulled)
								{
									for (unsigned int y = 0; y < (unsigned int)backgroundHeight; ++y)
									{
										int sourceY = y - yOffset;
										if (sourceY < 0 || sourceY >= (int)sourceBitmap.rows)
										{
											memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, backgroundWidth * pixelDataSize);
										}
										else
										{
											unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * sourceY;
											if (xStartIndex > 0)
											{
												memset((unsigned char *)(imageByteBuffer + backgroundWidth * y * pixelDataSize), 0, xStartIndex * pixelDataSize);
											}
											for (unsigned int x = xStartIndex; x < xFinishIndex; ++x)
											{
												int sourceX = x - xOffset;
												if (sourceX < 0)
												{
													unsigned char *pPixel = imageByteBuffer + (x + backgroundWidth * y) * pixelDataSize;
													pPixel[0] = 0;
													pPixel[1] = 0;
													pPixel[2] = 0;
													pPixel[3] = 0;
												}
												else
												{
													bool isFilledBit = (((*(pCurrentRowBuffer + sourceX / 8)) & (0x80 >> (sourceX % 8))) != 0);
													unsigned char *pPixel = imageByteBuffer + (x + backgroundWidth * y) * pixelDataSize;
													if (isFilledBit)
													{
														pPixel[0] = drawColor.red;
														pPixel[1] = drawColor.green;
														pPixel[2] = drawColor.blue;
														pPixel[3] = drawColor.alpha;
													}
													else
													{
														pPixel[0] = 0;
														pPixel[1] = 0;
														pPixel[2] = 0;
														pPixel[3] = 0;
													}
												}
											}
											if (xFinishIndex < (unsigned int)backgroundWidth)
											{
												memset((unsigned char *)(imageByteBuffer + (xFinishIndex + backgroundWidth * y) * pixelDataSize), 0, (backgroundWidth - xFinishIndex) * pixelDataSize);
											}
										}
									}
								}
							}
							break;

						default:
							// Not-Supporting Modes
							return false;
					}

					return true;
				}

				bool BlendBitmapFontImage(FT_Bitmap &sourceBitmap, Graphics::GLByteARGB::ColorCodeType color, void *imageBuffer,
					GLint backgroundWidth, GLint backgroundHeight, GLint xOffset, GLint yOffset, 
					GLenum format, GLenum dataType, bool isPremultipliedAlpha)
				{
					if (format != GL_RGBA || dataType != GL_UNSIGNED_BYTE)
					{
						// Will be develop.... Not supporting now
						return false;
					}

					Graphics::GLByteARGB drawColor = Graphics::GLByteARGB::FromColorCodeARGB(color);
					float floatAlphaColor = (float)drawColor.alpha / 255.0f;

					Graphics::GLByteARGB premultipliedAlphaColor = drawColor;
					if (isPremultipliedAlpha)
					{
						premultipliedAlphaColor.red = (GLubyte)((int)premultipliedAlphaColor.red * (int)premultipliedAlphaColor.alpha / 255);
						premultipliedAlphaColor.green = (GLubyte)((int)premultipliedAlphaColor.green * (int)premultipliedAlphaColor.alpha / 255);
						premultipliedAlphaColor.blue = (GLubyte)((int)premultipliedAlphaColor.blue * (int)premultipliedAlphaColor.alpha / 255);
					}
					bool isNoTransparency = (premultipliedAlphaColor.alpha == (GLubyte)255);

					unsigned char *imageByteBuffer = (unsigned char *)imageBuffer;
					int pixelDataSize = 4; // For now, GL_RGBA is only used

					switch (sourceBitmap.pixel_mode)
					{
						case FT_PIXEL_MODE_NONE:
						case FT_PIXEL_MODE_GRAY:
							{
								float grayDivisor = 1.0f;
								if (sourceBitmap.num_grays > 1)
								{
									grayDivisor = 1.0f / (float)(sourceBitmap.num_grays - 1);
								}

								for (unsigned int y = 0; y < sourceBitmap.rows; ++y)
								{
									GLint pixelY = (GLint)y + yOffset;
									if (pixelY < 0)
									{
										continue;
									}
									if (pixelY >= backgroundHeight)
									{
										break;
									}

									unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * y;
									for (unsigned int x = 0; x < sourceBitmap.width; ++x)
									{
										unsigned char rawPixelValue = *(pCurrentRowBuffer + x);
										if (rawPixelValue > 0)
										{
											GLint pixelX = (GLint)x + xOffset;
											if (pixelX < 0)
											{
												continue;
											}
											if (pixelX >= backgroundWidth)
											{
												break;
											}

											unsigned char *pPixel = imageByteBuffer + (pixelX + backgroundWidth * pixelY) * pixelDataSize;

											if (rawPixelValue == sourceBitmap.num_grays && isNoTransparency)
											{
												pPixel[0] = premultipliedAlphaColor.red;
												pPixel[1] = premultipliedAlphaColor.green;
												pPixel[2] = premultipliedAlphaColor.blue;
												pPixel[3] = premultipliedAlphaColor.alpha;
											}
											else
											{
												if (pPixel[3] == 0)
												{
													pPixel[3] = (unsigned char)fmin(255.0f * floatAlphaColor * (float)rawPixelValue * grayDivisor, 255.0f);
													if (isPremultipliedAlpha)
													{
														pPixel[0] = (GLubyte)((int)drawColor.red * (int)pPixel[3] / 255);
														pPixel[1] = (GLubyte)((int)drawColor.green * (int)pPixel[3] / 255);
														pPixel[2] = (GLubyte)((int)drawColor.blue * (int)pPixel[3] / 255);
													}
													else
													{
														pPixel[0] = drawColor.red;
														pPixel[1] = drawColor.green;
														pPixel[2] = drawColor.blue;
													}
												}
												else
												{
													float blendParameter = (float)rawPixelValue * grayDivisor;
													if (blendParameter > 1.0f)
													{
														blendParameter = 1.0f;
													}
													else if (blendParameter < 0.0f)
													{
														blendParameter = 0.0f;
													}
													float inverseBlendParameter = (1.0f - blendParameter);

													float existingBlendParameter = (float)pPixel[3] / 255.0f;
													float inverseExistingBlendParameter = (1.0f - existingBlendParameter);

													float pixelAlpha = 1.0f - inverseExistingBlendParameter * inverseBlendParameter;
													pPixel[3] = (GLubyte)fmin(255.0f * pixelAlpha, 255.0f);

													if (isPremultipliedAlpha)
													{
														float oneOverExistingBlendParameter = 1.0f / existingBlendParameter;
														pPixel[0] = (GLubyte)((drawColor.red * blendParameter + (float)fmin(pPixel[0] * oneOverExistingBlendParameter, 255.0f) * inverseBlendParameter) * pixelAlpha);
														pPixel[1] = (GLubyte)((drawColor.green * blendParameter + (float)fmin(pPixel[1] * oneOverExistingBlendParameter, 255.0f) * inverseBlendParameter) * pixelAlpha);
														pPixel[2] = (GLubyte)((drawColor.blue * blendParameter + (float)fmin(pPixel[2] * oneOverExistingBlendParameter, 255.0f) * inverseBlendParameter) * pixelAlpha);
													}
													else
													{
														float baseParam = inverseBlendParameter * existingBlendParameter;
														float oneOverPixelAlpha = 1.0f / pixelAlpha;
														pPixel[0] = (GLubyte)((drawColor.red * blendParameter + pPixel[0] * baseParam) * oneOverPixelAlpha);
														pPixel[1] = (GLubyte)((drawColor.green * blendParameter + pPixel[1] * baseParam) * oneOverPixelAlpha);
														pPixel[2] = (GLubyte)((drawColor.blue * blendParameter + pPixel[2] * baseParam) * oneOverPixelAlpha);
													}
												}
											}
										}
									}
								}
							}
							break;

						case FT_PIXEL_MODE_MONO:
							{
								for (unsigned int y = 0; y < sourceBitmap.rows; ++y)
								{
									GLint pixelY = (GLint)y + yOffset;
									if (pixelY < 0)
									{
										continue;
									}
									if (pixelY >= backgroundHeight)
									{
										break;
									}

									unsigned char *pCurrentRowBuffer = sourceBitmap.buffer + sourceBitmap.pitch * y;
									for (unsigned int x = 0; x < sourceBitmap.width; ++x)
									{
										bool isFilledBit = (((*(pCurrentRowBuffer + x / 8)) & (0x80 >> (x % 8))) != 0);
										if (isFilledBit)
										{
											GLint pixelX = (GLint)x + xOffset;
											if (pixelX < 0)
											{
												continue;
											}
											if (pixelX >= backgroundWidth)
											{
												break;
											}

											unsigned char *pPixel = imageByteBuffer + (pixelX + backgroundWidth * pixelY) * pixelDataSize;

											pPixel[0] = premultipliedAlphaColor.red;
											pPixel[1] = premultipliedAlphaColor.green;
											pPixel[2] = premultipliedAlphaColor.blue;
											pPixel[3] = premultipliedAlphaColor.alpha;
										}
									}
								}
							}
							break;

						default:
							// Not-Supporting Modes
							return false;
					}

					return true;
				}
			}


			//////////////////////////////////////////////////////////////////////////////// NormalGlyphDrawer

			//------------------------------------------------------------------------------
			NormalGlyphDrawer::NormalGlyphDrawer()
			{
			}

			//------------------------------------------------------------------------------
			NormalGlyphDrawer::~NormalGlyphDrawer()
			{
			}

			//------------------------------------------------------------------------------
			// Draw bitmap image of a glyph on a given image buffer
			IGlyphDrawer::GlyphDrawResult NormalGlyphDrawer::DrawGlyph(void *imageBuffer, std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio,
				Graphics::GLByteARGB::ColorCodeType color, bool isManualBold, bool isManualItalic,
				GLenum format, GLenum dataType, bool isPremultipliedAlpha,
				float strokeWidth, Graphics::GLByteARGB::ColorCodeType strokeColor)
			{
				// Parameter Check
				std::shared_ptr<FontFace> freeTypeFont = std::dynamic_pointer_cast<FontFace>(font);
				if (!freeTypeFont || size < _Internal::kFloatEpsilon || (horizontalRatio < _Internal::kFloatEpsilon && horizontalRatio > -_Internal::kFloatEpsilon))
				{
					return IGlyphDrawer::GlyphDrawResult(false);
				}

				bool isStroked = true;
				if (strokeWidth < _Internal::kFloatEpsilon || ((strokeColor & 0xff000000) == 0))
				{
					isStroked = false;
				}

				bool isHollow = false;
				if ((color & 0xff000000) == 0)
				{
					if (!isStroked)
					{
						// What do you want to render???
						return IGlyphDrawer::GlyphDrawResult(false);
					}
					isHollow = true;
				}

				std::shared_ptr<FontFace::FaceInstance> face = freeTypeFont->UseFace();
				if (!face)
				{
					// Unavailable Font
					return IGlyphDrawer::GlyphDrawResult(false);
				}

				// Set Size
				FT_UInt horizontalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenXPPI());
				FT_UInt verticalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenYPPI());
				GlyphSizeType apiSize = 64.0f * (size * 72.0f / Graphics::ReferenceState::GetApproxScreenYPPI());
				FT_F26Dot6 apiFontHeight = (FT_F26Dot6)round(apiSize);
				FT_F26Dot6 apiFontWidth = 0;
				if (horizontalRatio > 1.0f + _Internal::kFloatEpsilon || horizontalRatio < 1.0f - _Internal::kFloatEpsilon)
				{
					apiFontWidth = (FT_F26Dot6)round(horizontalRatio * apiSize);
				}
				if (FT_Set_Char_Size(face->GetFaceHandle(), apiFontWidth, apiFontHeight, horizontalDPI, verticalDPI) != FT_Err_Ok)
				{
					// Failed to set font size
					return IGlyphDrawer::GlyphDrawResult(false);
				}

				GlyphSizeType strokeRadiusAPISize = static_cast<GlyphSizeType>(0.0);
				FT_Fixed strokeRadiusAPISizeFixed = 0;
				if (isStroked)
				{
					strokeRadiusAPISize = 64.0f * (strokeWidth * 72.0f / Graphics::ReferenceState::GetApproxScreenYPPI());
					strokeRadiusAPISizeFixed = (FT_Fixed)round(strokeRadiusAPISize);
				}

				// Load Glype
				FT_Int32 loadFlags = FT_LOAD_DEFAULT;
				if (isStroked || isManualBold || isManualItalic)
				{
					loadFlags |= FT_LOAD_NO_BITMAP;
				}
				if (FT_Load_Glyph(face->GetFaceHandle(), glyphCode, loadFlags) != FT_Err_Ok)
				{
					// Failed to load Glype
					return IGlyphDrawer::GlyphDrawResult(false);
				}

				// Apply Transform
				if (isManualBold)
				{
					FT_Outline_Embolden(&face->GetFaceHandle()->glyph->outline, (FT_Pos)((face->GetFaceHandle()->size->metrics.x_ppem * _Internal::kBoldAmount) * 64.0f));
				}
				if (isManualItalic)
				{
					FT_Matrix transform = { 0x10000, (FT_Fixed)(65536.0f * _Internal::kItalicAmount), 0x00000, 0x10000 };
					FT_Outline_Transform(&face->GetFaceHandle()->glyph->outline, &transform);
				}

				// Process Glyph
				if (isStroked)
				{
					FT_Glyph glyph = nullptr;
					if (FT_Get_Glyph(face->GetFaceHandle()->glyph, &glyph) != FT_Err_Ok)
					{
						// Cannot get glype
						return IGlyphDrawer::GlyphDrawResult(false);
					}
					{
						auto releaseGlyphFunc = [&glyph]() mutable { FT_Done_Glyph(glyph); };
						FINALLY_OF_BLOCK(_RELEASE_GLYPH, releaseGlyphFunc);

						FT_Glyph strokeGlyph = nullptr;
						if (FT_Glyph_Copy(glyph, &strokeGlyph) != FT_Err_Ok)
						{
							// Cannot get glype
							return IGlyphDrawer::GlyphDrawResult(false);
						}
						{
							auto releaseStrokeGlyphFunc = [&strokeGlyph]() mutable { FT_Done_Glyph(strokeGlyph); };
							FINALLY_OF_BLOCK(_RELEASE_STROKE_GLYPH, releaseStrokeGlyphFunc);

							FT_Stroker stroker = nullptr;
							if (FT_Stroker_New(_Internal::g_libraryInstance, &stroker) != FT_Err_Ok)
							{
								// Cannot Allocate Stroker
								return IGlyphDrawer::GlyphDrawResult(false);
							}
							{
								auto releaseStrokerFunc = [&stroker]() mutable { FT_Stroker_Done(stroker); };
								FINALLY_OF_BLOCK(_RELEASE_STROKER, releaseStrokerFunc);

								// Set Stroke
								FT_Stroker_Set(stroker, strokeRadiusAPISizeFixed, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
								FT_Glyph_StrokeBorder(&strokeGlyph, stroker, 0, 1);

								// Render Stroked Glyph First as Background
								if (strokeGlyph->format != FT_GLYPH_FORMAT_BITMAP)
								{
									if (FT_Glyph_To_Bitmap(&strokeGlyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != FT_Err_Ok)
									{
										// Cannot Convert Glyph to Bitmap
										return IGlyphDrawer::GlyphDrawResult(false);
									}
								}
								FT_BitmapGlyph bitmapStrokedGlype = (FT_BitmapGlyph)strokeGlyph;
								FT_Bitmap bitmapStroked = bitmapStrokedGlype->bitmap;
								if (bitmapStroked.buffer != nullptr)
								{
									if (!_Internal::FillBitmapFontImage(bitmapStroked, strokeColor, imageBuffer, format, dataType, isPremultipliedAlpha))
									{
										// Cannot Fill Bitmap Image
										return IGlyphDrawer::GlyphDrawResult(false);
									}
								}

								// Render Plain Glyph Over Stroked
								if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
								{
									if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != FT_Err_Ok)
									{
										// Cannot Convert Glyph to Bitmap
										return IGlyphDrawer::GlyphDrawResult(false);
									}
								}
								FT_BitmapGlyph bitmapGlype = (FT_BitmapGlyph)glyph;
								FT_Bitmap bitmap = bitmapGlype->bitmap;
								if (bitmap.buffer != nullptr)
								{
									GLint xOffset = (GLint)bitmapGlype->left - (GLint)bitmapStrokedGlype->left;
									GLint yOffset = (GLint)bitmapStrokedGlype->top - (GLint)bitmapGlype->top;

									if (!_Internal::BlendBitmapFontImage(bitmap, color, imageBuffer, (GLint)bitmapStroked.width, (GLint)bitmapStroked.rows, xOffset, yOffset, format, dataType, isPremultipliedAlpha))
									{
										// Cannot Fill Bitmap Image
										return IGlyphDrawer::GlyphDrawResult(false);
									}
								}

								return IGlyphDrawer::GlyphDrawResult(true, (GLsizei)bitmapStroked.width, (GLsizei)bitmapStroked.rows, (GLint)bitmapStrokedGlype->left, (GLint)bitmapStrokedGlype->top, (int)(strokeGlyph->advance.x >> 16), (int)(strokeGlyph->advance.x >> 16));
							}
						}
					}
				}
				else
				{
					FT_Glyph glyph = nullptr;
					if (FT_Get_Glyph(face->GetFaceHandle()->glyph, &glyph) != FT_Err_Ok)
					{
						// Cannot get glype image
						return IGlyphDrawer::GlyphDrawResult(false);
					}

					{
						auto releaseGlyphFunc = [&glyph]() mutable { FT_Done_Glyph(glyph); };
						FINALLY_OF_BLOCK(_RELEASE_GLYPH, releaseGlyphFunc);

						// Render
						if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
						{
							if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != FT_Err_Ok)
							{
								// Cannot Convert Glyph to Bitmap
								return IGlyphDrawer::GlyphDrawResult(false);
							}
						}

						// Copy Bitmap
						FT_BitmapGlyph bitmapGlype = (FT_BitmapGlyph)glyph;
						FT_Bitmap bitmap = bitmapGlype->bitmap;
						if (bitmap.buffer != nullptr)
						{
							if (!_Internal::FillBitmapFontImage(bitmap, color, imageBuffer, format, dataType, isPremultipliedAlpha))
							{
								// Cannot Fill Bitmap Image
								return IGlyphDrawer::GlyphDrawResult(false);
							}
						}

						return IGlyphDrawer::GlyphDrawResult(true, (GLsizei)bitmap.width, (GLsizei)bitmap.rows, (GLint)bitmapGlype->left, (GLint)bitmapGlype->top, (int)(glyph->advance.x >> 16), (int)(glyph->advance.x >> 16));
					}
				}

				return IGlyphDrawer::GlyphDrawResult(false);
			}

			//------------------------------------------------------------------------------
			// Get size information to prepare glyph drawing
			IGlyphDrawer::GlyphSizeInfo NormalGlyphDrawer::GetGlyphImageSIze(std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio,
				bool isManualBold, bool isManualItalic,
				float strokeWidth)
			{
				// Parameter Check
				std::shared_ptr<FontFace> freeTypeFont = std::dynamic_pointer_cast<FontFace>(font);
				if (!freeTypeFont || size < _Internal::kFloatEpsilon || (horizontalRatio < _Internal::kFloatEpsilon && horizontalRatio > -_Internal::kFloatEpsilon))
				{
					return IGlyphDrawer::GlyphSizeInfo(false);
				}

				bool isStroked = true;
				if (strokeWidth < _Internal::kFloatEpsilon)
				{
					isStroked = false;
				}

				std::shared_ptr<FontFace::FaceInstance> face = freeTypeFont->UseFace();
				if (!face)
				{
					// Unavailable Font
					return IGlyphDrawer::GlyphSizeInfo(false);
				}

				// Set Size
				FT_UInt horizontalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenXPPI());
				FT_UInt verticalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenYPPI());
				GlyphSizeType apiSize = 64.0f * (size * 72.0f / Graphics::ReferenceState::GetApproxScreenYPPI());
				FT_F26Dot6 apiFontHeight = (FT_F26Dot6)round(apiSize);
				FT_F26Dot6 apiFontWidth = 0;
				if (horizontalRatio > 1.0f + _Internal::kFloatEpsilon || horizontalRatio < 1.0f - _Internal::kFloatEpsilon)
				{
					apiFontWidth = (FT_F26Dot6)round(horizontalRatio * apiSize);
				}
				if (FT_Set_Char_Size(face->GetFaceHandle(), apiFontWidth, apiFontHeight, horizontalDPI, verticalDPI) != FT_Err_Ok)
				{
					// Failed to set font size
					return IGlyphDrawer::GlyphSizeInfo(false);
				}

				GlyphSizeType strokeRadiusAPISize = static_cast<GlyphSizeType>(0.0);
				FT_Fixed strokeRadiusAPISizeFixed = 0;
				if (isStroked)
				{
					strokeRadiusAPISize = 64.0f * (strokeWidth * 72.0f / Graphics::ReferenceState::GetApproxScreenYPPI());
					strokeRadiusAPISizeFixed = (FT_Fixed)round(strokeRadiusAPISize);
				}

				// Load Glype
				FT_Int32 loadFlags = FT_LOAD_DEFAULT;
				if (isStroked || isManualBold || isManualItalic)
				{
					loadFlags |= FT_LOAD_NO_BITMAP;
				}
				if (FT_Load_Glyph(face->GetFaceHandle(), glyphCode, loadFlags) != FT_Err_Ok)
				{
					// Failed to load Glype
					return IGlyphDrawer::GlyphSizeInfo(false);
				}

				// Apply Transform
				if (isManualBold)
				{
					FT_Outline_Embolden(&face->GetFaceHandle()->glyph->outline, (FT_Pos)((face->GetFaceHandle()->size->metrics.x_ppem * _Internal::kBoldAmount) * 64.0f));
				}
				if (isManualItalic)
				{
					FT_Matrix transform = { 0x10000, (FT_Fixed)(65536.0f * _Internal::kItalicAmount), 0x00000, 0x10000 };
					FT_Outline_Transform(&face->GetFaceHandle()->glyph->outline, &transform);
				}

				// Process Glyph
				FT_Glyph glyph;
				if (FT_Get_Glyph(face->GetFaceHandle()->glyph, &glyph) != FT_Err_Ok)
				{
					// Cannot get glype image
					return IGlyphDrawer::GlyphSizeInfo(false);
				}
				{
					auto releaseGlyphFunc = [&glyph]() mutable { FT_Done_Glyph(glyph); };
					FINALLY_OF_BLOCK(_RELEASE_GLYPH, releaseGlyphFunc);

					// Apply Stroke
					FT_Stroker stroker = nullptr;
					if (isStroked)
					{
						if (FT_Stroker_New(_Internal::g_libraryInstance, &stroker) != FT_Err_Ok)
						{
							// Cannot Allocate Stroker
							return IGlyphDrawer::GlyphSizeInfo(false);
						}
						FT_Stroker_Set(stroker, strokeRadiusAPISizeFixed, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
						FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
					}

					// Get Size
					FT_BBox boundingBox;
					FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &boundingBox);

					GLsizei width = (GLsizei)(boundingBox.xMax - boundingBox.xMin);
					GLsizei height = (GLsizei)(boundingBox.yMax - boundingBox.yMin);

					if (isStroked)
					{
						FT_Stroker_Done(stroker);
					}

					return IGlyphDrawer::GlyphSizeInfo(true, width, height, (int)(glyph->advance.x >> 16), (int)(glyph->advance.x >> 16));
				}

				return IGlyphDrawer::GlyphSizeInfo(false);
			}
		}
	}
}
