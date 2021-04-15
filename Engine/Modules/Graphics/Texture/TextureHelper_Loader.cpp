////////////////////////////////////////////////////////////////////////////////
// Texture/TextureHelper_Loader.cpp (Leggiero/Modules - Graphics)
//
// Loader part of Texture Helper Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureHelper.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Graphics
#include "../_Internal/_GraphicsModuleInternalState.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace Texture
		{
			//////////////////////////////////////////////////////////////////////////////// Loader Utility

			namespace Loader
			{
				//------------------------------------------------------------------------------
				// Create OpenGL ES Texture from image data in memory
				GLESRawTextureInformation LoadTextureFromImageInMemory(const void *data, size_t size, ImageFormatType imageType, GLint mipmapLevel, bool isUsePower2Dimension)
				{
					switch (imageType)
					{
						case ImageFormatType::kPNG:
							{
								return PNG::LoadPNGTextureFromMemory(data, size, mipmapLevel, isUsePower2Dimension);
							}
							break;

						case ImageFormatType::kJPEG:
							{
								return JPEG::LoadJPEGTextureFromMemory(data, size, mipmapLevel, isUsePower2Dimension);
							}
							break;

						default: break; // Do Nothing
					}

					// Un-supported Image Format
					return GLESRawTextureInformation::kINVALID;
				}

				//------------------------------------------------------------------------------
				// Create OpenGL ES Texture from image data as stream
				GLESRawTextureInformation LoadTextureFromImageInStream(std::istream &sourceStream, ImageFormatType imageType, GLint mipmapLevel, bool isUsePower2Dimension)
				{
					switch (imageType)
					{
						case ImageFormatType::kPNG:
							{
								return PNG::LoadPNGTextureFromStream(sourceStream, mipmapLevel, isUsePower2Dimension);
							}
							break;

						case ImageFormatType::kJPEG:
							{
								return JPEG::LoadJPEGTextureFromStream(sourceStream, mipmapLevel, isUsePower2Dimension);
							}
							break;

						default: break; // Do Nothing
					}

					// Un-supported Image Format
					return GLESRawTextureInformation::kINVALID;
				}

				//------------------------------------------------------------------------------
				// Create OpenGL ES Texture from image data in file
				GLESRawTextureInformation LoadTextureFromImageInFile(const char *filePath, ImageFormatType imageType, GLint mipmapLevel, bool isUsePower2Dimension)
				{
					ImageFormatType loadingImageType = imageType;
					if (loadingImageType == ImageFormatType::kINVALID)
					{
						// Detect Image Type from File Extension
						loadingImageType = GetImageFormatFromFilePathExtension(filePath);
					}

					switch (loadingImageType)
					{
						case ImageFormatType::kPNG:
							{
								return PNG::LoadPNGTextureFromFile(filePath, mipmapLevel, isUsePower2Dimension);
							}
							break;

						case ImageFormatType::kJPEG:
							{
								return JPEG::LoadJPEGTextureFromFile(filePath, mipmapLevel, isUsePower2Dimension);
							}
							break;

						default: break; // Do Nothing
					}

					// Un-supported Image Format
					return GLESRawTextureInformation::kINVALID;
				}


				//////////////////////////////////////////////////////////////////////////////// Raw Texture Data

				//------------------------------------------------------------------------------
				// Create an OpenGL ES Texture from given bitmap data in memory
				GLESRawTextureInformation LoadTextureFromBitmapDataInMemory(const void *data,
					GLsizei width, GLsizei height,
					GLenum format, GLenum dataType,
					GLint mipmapLevel,
					bool isUsePower2Dimension)
				{
					GLuint textureName;

					// Clear Error Flags
					glGetError();

					Utility::Threading::SafePthreadLock &nameCreateLock = _Internal::Synchronization::GetTextureNameCreationLock();
					{
						// Generate Texture Name within the Lock
						std::unique_ptr<Utility::Threading::SafePthreadLock::LockContext> lockContext = nameCreateLock.Lock();
						if (lockContext)
						{
							glGenTextures(1, &textureName);
							if (glGetError() != GL_NO_ERROR)
							{
								return GLESRawTextureInformation::kINVALID;
							}
							glFinish();
						}
						else
						{
							// Lock Failed
							return GLESRawTextureInformation::kINVALID;
						}
					}

					glBindTexture(GL_TEXTURE_2D, textureName);
					if (glGetError() != GL_NO_ERROR)
					{
						glDeleteTextures(1, &textureName);
						return GLESRawTextureInformation::kINVALID;
					}

					// Calculate Effective Size (power of 2)
					GLsizei effectiveWidth = width;
					GLsizei effectiveHeight = height;
					const void *creatingData = data;
					if (isUsePower2Dimension)
					{
						effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(effectiveWidth);
						effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(effectiveHeight);
						if (creatingData != nullptr && (width != effectiveWidth || height != effectiveHeight))
						{
							creatingData = nullptr;
						}
					}

					glTexImage2D(GL_TEXTURE_2D, mipmapLevel, format, effectiveWidth, effectiveHeight, 0, format, dataType, creatingData);
					if (creatingData == nullptr && data != nullptr)
					{
						glTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, 0, 0, width, height, format, dataType, data);
					}
					if (glGetError() != GL_NO_ERROR)
					{
						glDeleteTextures(1, &textureName);
						return GLESRawTextureInformation::kINVALID;
					}

					glFinish();

					GLESRawTextureInformation resultTextureInfo;
					resultTextureInfo.name = textureName;
					resultTextureInfo.target = GL_TEXTURE_2D;
					resultTextureInfo.width = effectiveWidth;
					resultTextureInfo.height = effectiveHeight;
					resultTextureInfo.contentWidth = width;
					resultTextureInfo.contentHeight = height;
					resultTextureInfo.hasAlpha = HasFormatAlpha(format);
					resultTextureInfo.hasMimmap = (mipmapLevel > 0);

					return resultTextureInfo;
				}
			}
		}
	}
}
