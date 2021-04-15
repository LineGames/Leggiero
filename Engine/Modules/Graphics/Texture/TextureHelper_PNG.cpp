////////////////////////////////////////////////////////////////////////////////
// Texture/TextureHelper_PNG.cpp (Leggiero/Modules - Graphics)
//
// Texture Helper functions for PNG image format
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureHelper.h"

// Standard Library
#include <cctype>
#include <cstdio>
#include <climits>
#include <cstring>
#include <functional>

// External Library
#include <libpng/png.h>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace Graphics
	{
		namespace Texture
		{
			namespace Loader
			{
				namespace PNG
				{
					//////////////////////////////////////////////////////////////////////////////// PNG Internal Utility

					namespace _Internal
					{
						const size_t kPNGSignatureSize = 8;

						// Structure to store stateful memory buffer
						struct _PNGMemoryBufferState
						{
							png_const_bytep	buffer;
							size_t			offset;
							size_t			size;
						};


						//------------------------------------------------------------------------------
						// Read PNG data from Memory Buffer Stream
						void PNG_ReadFunction_MemoryBuffer(png_structp pngPtr, png_bytep buffer, png_size_t count)
						{
							png_voidp ioPtr = png_get_io_ptr(pngPtr);
							if (ioPtr == NULL)
							{
								// NULL IO Pointer
								return;
							}

							_PNGMemoryBufferState &bufferState = *((_PNGMemoryBufferState *)ioPtr);

							png_size_t sizeToRead = count;
							if (bufferState.offset + sizeToRead > bufferState.size)
							{
								sizeToRead = bufferState.size - bufferState.offset;
							}
							if (sizeToRead <= 0)
							{
								// Buffer Overflow
								return;
							}

							memcpy(buffer, bufferState.buffer + bufferState.offset, sizeToRead);
							bufferState.offset += sizeToRead;
						}

						//------------------------------------------------------------------------------
						// Read PNG data from std::istream
						void PNG_ReadFunction_IStream(png_structp pngPtr, png_bytep buffer, png_size_t count)
						{
							png_voidp ioPtr = png_get_io_ptr(pngPtr);
							if (ioPtr == NULL)
							{
								// NULL IO Pointer
								return;
							}

							std::istream &sourceStream = *((std::istream *)ioPtr);
							sourceStream.read((char *)buffer, (std::streamsize)count);
						}

						//------------------------------------------------------------------------------
						// Create PNG Texture from an IO source
						GLESRawTextureInformation _LoadPNGTexture(std::function<void(png_structp)> funcIOSetter, GLint mipmapLevel, bool isUsePower2Dimension)
						{
							png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
							if (!pngPtr)
							{
								// png_create_read_struct Failed
								return GLESRawTextureInformation::kINVALID;
							}

							png_infop infoPtr = png_create_info_struct(pngPtr);
							if (!infoPtr)
							{
								// png_create_info_struct Failed
								png_destroy_read_struct(&pngPtr, (png_infopp)NULL, (png_infopp)NULL);
								return GLESRawTextureInformation::kINVALID;
							}
							png_infop endInfoPtr = png_create_info_struct(pngPtr);
							if (!endInfoPtr)
							{
								png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)NULL);
								return GLESRawTextureInformation::kINVALID;
							}

							if (setjmp(png_jmpbuf(pngPtr))) {
								// Jmp Buffer Failed
								png_destroy_read_struct(&pngPtr, &infoPtr, &endInfoPtr);
								return GLESRawTextureInformation::kINVALID;
							}

							funcIOSetter(pngPtr);

							png_set_sig_bytes(pngPtr, kPNGSignatureSize);

							png_read_info(pngPtr, infoPtr);

							int bitDepth;
							int colorType;
							png_uint_32 imageWidth;
							png_uint_32 imageHeight;
							if (png_get_IHDR(pngPtr, infoPtr, &imageWidth, &imageHeight, &bitDepth, &colorType, NULL, NULL, NULL) == 0)
							{
								// PNG Error
								png_destroy_read_struct(&pngPtr, &infoPtr, &endInfoPtr);
								return GLESRawTextureInformation::kINVALID;
							}

							if (bitDepth < 8)
							{
								bitDepth = 8;
								png_set_packing(pngPtr);
								png_read_update_info(pngPtr, infoPtr);
							}
							int byteDepth = bitDepth / 8;

							png_size_t rowbytes = png_get_rowbytes(pngPtr, infoPtr);

							// PNG Resource Auto-Cleanup Block
							{
								auto pngCleanupFunc = [pngPtr, infoPtr, endInfoPtr]() mutable {
									png_destroy_read_struct(&pngPtr, &infoPtr, &endInfoPtr);
								};
								FINALLY_OF_BLOCK(_CLEANUP_PNG_STRUCTURES, pngCleanupFunc);

								if (bitDepth == 8 &&
									(colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_RGB_ALPHA))
								{
									// PNG Data can be used as glTexImage2D Image Data
									png_bytep imageData = (png_byte *)malloc(rowbytes * imageHeight);
									if (imageData == NULL)
									{
										// Out of Memory
										return GLESRawTextureInformation::kINVALID;
									}

									png_bytepp rawPointers = (png_bytep *)malloc(imageHeight * sizeof(png_bytep));
									if (rawPointers == NULL)
									{
										// Out of Memory
										free(imageData);
										return GLESRawTextureInformation::kINVALID;
									}

									for (unsigned int i = 0; i < imageHeight; ++i)
									{
										rawPointers[i] = imageData + i * rowbytes;
									}

									// Buffer Auto-Free Block
									{
										auto bufferFreeFunc = [imageData, rawPointers]() mutable {
											free(rawPointers);
											free(imageData);
										};
										FINALLY_OF_BLOCK(_FREE_PNG_DATA_BUFFERS, bufferFreeFunc);

										png_read_image(pngPtr, rawPointers);

										GLenum format = (colorType == PNG_COLOR_TYPE_RGB) ? GL_RGB : GL_RGBA;

										return LoadTextureFromBitmapDataInMemory(imageData, imageWidth, imageHeight, format, GL_UNSIGNED_BYTE, mipmapLevel, isUsePower2Dimension);
									}
								}
								else
								{
									png_colorp palette;
									int paletteNumber;
									png_bytep transparentPalette;
									int transparentPaletteNumber;

									bool isPalette = ((colorType | PNG_COLOR_MASK_PALETTE) != 0);
									bool isTransparentPalette = false;
									if (isPalette)
									{
										if (png_get_PLTE(pngPtr, infoPtr, &palette, &paletteNumber) == 0)
										{
											// Invalid Palette
											return GLESRawTextureInformation::kINVALID;
										}

										isTransparentPalette = (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS) == PNG_INFO_tRNS);
										if (isTransparentPalette)
										{
											if (png_get_tRNS(pngPtr, infoPtr, &transparentPalette, &transparentPaletteNumber, NULL) == 0)
											{
												// Invalid Palette
												return GLESRawTextureInformation::kINVALID;
											}
										}
									}

									png_byte channelNumber = png_get_channels(pngPtr, infoPtr);
									bool hasAlpha = (((colorType | PNG_COLOR_MASK_ALPHA) != 0) || isTransparentPalette);
									int resultChannelNumber = hasAlpha ? 4 : 3;

									GLsizei effectiveWidth = imageWidth;
									GLsizei effectiveHeight = imageHeight;
									if (isUsePower2Dimension)
									{
										effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(effectiveWidth);
										effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(effectiveHeight);
									}
									GLsizei remainRows = effectiveHeight - imageHeight;
									GLsizei remainPixelsOfRows = effectiveWidth - imageWidth;

									unsigned char *resultDataBuffer = (unsigned char *)malloc(effectiveWidth * effectiveHeight * resultChannelNumber);
									if (resultDataBuffer == NULL)
									{
										// Out of Memory
										return GLESRawTextureInformation::kINVALID;
									}

									png_bytep rowBuffer = (png_byte *)malloc(rowbytes);
									if (rowBuffer == NULL)
									{
										// Out of Memory
										free(resultDataBuffer);
										return GLESRawTextureInformation::kINVALID;
									}

									// Buffer Auto-Free Block
									{
										auto bufferFreeFunc = [resultDataBuffer, rowBuffer]() mutable {
											free(resultDataBuffer);
											free(rowBuffer);
										};
										FINALLY_OF_BLOCK(_FREE_PNG_DATA_BUFFERS, bufferFreeFunc);

										// For non-branching optimazation
										switch (colorType)
										{
											case PNG_COLOR_TYPE_GRAY:
												{
													for (unsigned int i = 0; i < imageHeight; ++i)
													{
														png_read_row(pngPtr, rowBuffer, NULL);
														GLsizei rowOffset = i * effectiveWidth * resultChannelNumber;
														for (unsigned int j = 0; j < imageWidth; ++j)
														{
															GLsizei pixelOffset = rowOffset + j * resultChannelNumber;
															GLsizei sourcePixelOffset = rowOffset + j * channelNumber * byteDepth;

															unsigned char brightness =
																(bitDepth == 16)
																? (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset)) >> 8)
																: *(((unsigned char *)rowBuffer) + sourcePixelOffset);
															*(((unsigned char *)resultDataBuffer) + pixelOffset) = brightness;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 1) = brightness;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 2) = brightness;
														}
														if (remainPixelsOfRows > 0)
														{
															memset(((char *)resultDataBuffer) + rowOffset + (imageWidth * resultChannelNumber), 0, remainPixelsOfRows * resultChannelNumber);
														}
													}
												}
												break;

											case PNG_COLOR_TYPE_GRAY_ALPHA:
												{
													for (unsigned int i = 0; i < imageHeight; ++i)
													{
														png_read_row(pngPtr, rowBuffer, NULL);
														GLsizei rowOffset = i * effectiveWidth * resultChannelNumber;
														for (unsigned int j = 0; j < imageWidth; ++j)
														{
															GLsizei pixelOffset = rowOffset + j * resultChannelNumber;
															GLsizei sourcePixelOffset = rowOffset + j * channelNumber * byteDepth;

															unsigned char brightness =
																(bitDepth == 16)
																? (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset)) >> 8)
																: *(((unsigned char *)rowBuffer) + sourcePixelOffset);
															unsigned char alpha =
																(bitDepth == 16)
																? (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 1) >> 8)
																: *(((unsigned char *)rowBuffer) + sourcePixelOffset + 1);
															*(((unsigned char *)resultDataBuffer) + pixelOffset) = brightness;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 1) = brightness;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 2) = brightness;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 3) = alpha;
														}
														if (remainPixelsOfRows > 0)
														{
															memset(((char *)resultDataBuffer) + rowOffset + (imageWidth * resultChannelNumber), 0, remainPixelsOfRows * resultChannelNumber);
														}
													}
												}
												break;

											case PNG_COLOR_TYPE_PALETTE:
												{
													for (unsigned int i = 0; i < imageHeight; ++i)
													{
														png_read_row(pngPtr, rowBuffer, NULL);
														GLsizei rowOffset = i * effectiveWidth * resultChannelNumber;
														for (unsigned int j = 0; j < imageWidth; ++j)
														{
															GLsizei pixelOffset = rowOffset + j * resultChannelNumber;
															GLsizei sourcePixelOffset = rowOffset + j * channelNumber * byteDepth;

															unsigned char paletteIndex = *(((unsigned char *)rowBuffer) + sourcePixelOffset);
															png_color &currentPalette = palette[paletteIndex];
															*(((unsigned char *)resultDataBuffer) + pixelOffset) = currentPalette.red;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 1) = currentPalette.green;
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 2) = currentPalette.blue;
															if (hasAlpha)
															{
																if (paletteIndex < transparentPaletteNumber)
																{
																	*(((unsigned char *)resultDataBuffer) + pixelOffset + 3) = (unsigned char)transparentPalette[paletteIndex];
																}
																else
																{
																	*(((unsigned char *)resultDataBuffer) + pixelOffset + 3) = UCHAR_MAX;
																}
															}
														}
														if (remainPixelsOfRows > 0)
														{
															memset(((char *)resultDataBuffer) + rowOffset + (imageWidth * resultChannelNumber), 0, remainPixelsOfRows * resultChannelNumber);
														}
													}
												}
												break;

											case PNG_COLOR_TYPE_RGB:
												{
													// bitDepth should be 16 at this point
													if (bitDepth != 16)
													{
														return GLESRawTextureInformation::kINVALID;
													}
													for (unsigned int i = 0; i < imageHeight; ++i)
													{
														png_read_row(pngPtr, rowBuffer, NULL);
														GLsizei rowOffset = i * effectiveWidth * resultChannelNumber;
														for (unsigned int j = 0; j < imageWidth; ++j)
														{
															GLsizei pixelOffset = rowOffset + j * resultChannelNumber;
															GLsizei sourcePixelOffset = rowOffset + j * channelNumber * byteDepth;

															*(((unsigned char *)resultDataBuffer) + pixelOffset) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset)) >> 8);
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 1) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 1) >> 8);
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 2) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 2) >> 8);
														}
														if (remainPixelsOfRows > 0)
														{
															memset(((char *)resultDataBuffer) + rowOffset + (imageWidth * resultChannelNumber), 0, remainPixelsOfRows * resultChannelNumber);
														}
													}
												}
												break;

											case PNG_COLOR_TYPE_RGB_ALPHA:
												{
													// bitDepth should be 16 at this point
													if (bitDepth != 16)
													{
														return GLESRawTextureInformation::kINVALID;
													}
													for (unsigned int i = 0; i < imageHeight; ++i)
													{
														png_read_row(pngPtr, rowBuffer, NULL);
														GLsizei rowOffset = i * effectiveWidth * resultChannelNumber;
														for (unsigned int j = 0; j < imageWidth; ++j)
														{
															GLsizei pixelOffset = rowOffset + j * resultChannelNumber;
															GLsizei sourcePixelOffset = rowOffset + j * channelNumber * byteDepth;

															*(((unsigned char *)resultDataBuffer) + pixelOffset) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset)) >> 8);
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 1) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 1) >> 8);
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 2) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 2) >> 8);
															*(((unsigned char *)resultDataBuffer) + pixelOffset + 3) = (unsigned char)(*((unsigned short *)(((unsigned char *)rowBuffer) + sourcePixelOffset) + 3) >> 8);
														}
														if (remainPixelsOfRows > 0)
														{
															memset(((char *)resultDataBuffer) + rowOffset + (imageWidth * resultChannelNumber), 0, remainPixelsOfRows * resultChannelNumber);
														}
													}
												}
												break;

											default:
												{
													// Un-supported Format
													return GLESRawTextureInformation::kINVALID;
												}
										}

										if (remainRows > 0)
										{
											memset(((char *)resultDataBuffer) + (effectiveWidth * imageHeight * resultChannelNumber), 0, (effectiveHeight - imageHeight) * effectiveWidth * resultChannelNumber);
										}

										GLenum format = ((colorType | PNG_COLOR_MASK_ALPHA) == 0) ? GL_RGB : GL_RGBA;
										GLESRawTextureInformation resultInformation = LoadTextureFromBitmapDataInMemory(resultDataBuffer, effectiveWidth, effectiveHeight, format, GL_UNSIGNED_BYTE, mipmapLevel, false);
										if (isUsePower2Dimension && (remainRows > 0 || remainPixelsOfRows > 0))
										{
											resultInformation.contentWidth = imageWidth;
											resultInformation.contentHeight = imageHeight;
										}
										return resultInformation;
									}
								}
							}
						}
					}


					//////////////////////////////////////////////////////////////////////////////// PNG Loaders

					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from in-memory PNG Image Data
					GLESRawTextureInformation LoadPNGTextureFromMemory(const void *data, size_t size, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						if (size < _Internal::kPNGSignatureSize)
						{
							// Not a PNG Image
							return GLESRawTextureInformation::kINVALID;
						}

						png_const_bytep dataReadPointer = (png_const_bytep)data;

						if (png_sig_cmp(dataReadPointer, 0, _Internal::kPNGSignatureSize) != 0)
						{
							// Not a PNG Image
							return GLESRawTextureInformation::kINVALID;
						}

						_Internal::_PNGMemoryBufferState dataBuffer;
						dataBuffer.buffer = dataReadPointer;
						dataBuffer.offset = _Internal::kPNGSignatureSize;
						dataBuffer.size = size;

						return _Internal::_LoadPNGTexture([&dataBuffer](png_structp pngPtr) mutable {
							png_set_read_fn(pngPtr, &dataBuffer, _Internal::PNG_ReadFunction_MemoryBuffer);
							}, mipmapLevel, isUsePower2Dimension);
					}

					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from PNG Stream
					GLESRawTextureInformation LoadPNGTextureFromStream(std::istream &sourceStream, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						// Check Signature
						char checkSignature[8];
						sourceStream.read(checkSignature, _Internal::kPNGSignatureSize);
						if (png_sig_cmp((png_bytep)checkSignature, 0, _Internal::kPNGSignatureSize) != 0)
						{
							// Not a PNG Image
							return GLESRawTextureInformation::kINVALID;
						}

						return _Internal::_LoadPNGTexture([&sourceStream](png_structp pngPtr) mutable {
							png_set_read_fn(pngPtr, &sourceStream, _Internal::PNG_ReadFunction_IStream);
							}, mipmapLevel, isUsePower2Dimension);
					}

					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from PNG File
					GLESRawTextureInformation LoadPNGTextureFromFile(const char *filePath, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						FILE *pngFile = fopen(filePath, "rb");
						if (pngFile == nullptr)
						{
							// Cannot open a file
							return GLESRawTextureInformation::kINVALID;
						}

						// Close file block
						{
							auto fileCloseFunc = [pngFile]() mutable {
								fclose(pngFile);
							};
							FINALLY_OF_BLOCK(_FILE_CLOSE, fileCloseFunc);

							unsigned char checkSignature[8];
							fread(checkSignature, 1, _Internal::kPNGSignatureSize, pngFile);
							if (png_sig_cmp(checkSignature, 0, _Internal::kPNGSignatureSize) != 0)
							{
								// Not a PNG Image
								return GLESRawTextureInformation::kINVALID;
							}

							return _Internal::_LoadPNGTexture([pngFile](png_structp pngPtr) mutable {
								png_init_io(pngPtr, pngFile);
								}, mipmapLevel, isUsePower2Dimension);
						}
					}
				}
			}
		}
	}
}
