////////////////////////////////////////////////////////////////////////////////
// Texture/TextureHelper_JPEG.cpp (Leggiero/Modules - Graphics)
//
// Texture Helper functions for JPEG image format
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureHelper.h"

// Standard Library
#include <string>

// External Library
#include <turbojpeg.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace Graphics
	{
		namespace Texture
		{
			namespace Loader
			{
				namespace JPEG
				{
					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from in-memory JPEG Image Data
					GLESRawTextureInformation LoadJPEGTextureFromMemory(const void *data, size_t size, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						tjhandle tjInstance = tjInitDecompress();
						if (tjInstance == NULL)
						{
							return GLESRawTextureInformation::kINVALID;
						}

						auto tjCleanupFunc = [tjInstance]() mutable {
							if (tjInstance)
							{
								tjDestroy(tjInstance);
							}
						};
						FINALLY_OF_BLOCK(_CLEANUP_TJ_INSTANCE, tjCleanupFunc);

						int width, height;
						int inSubsamp, inColorspace;
						if (tjDecompressHeader3(tjInstance, (const unsigned char *)data, (unsigned long)size, &width, &height, &inSubsamp, &inColorspace) < 0)
						{
							// JPEG Header Parse Error
							return GLESRawTextureInformation::kINVALID;
						}

						unsigned char *imageData = (unsigned char *)malloc(width * height * 3);	// RGB Format
						if (imageData == NULL)
						{
							// Out of Memory
							return GLESRawTextureInformation::kINVALID;
						}

						// Buffer Auto-Free Block
						{
							auto bufferFreeFunc = [imageData]() mutable {
								free(imageData);
							};
							FINALLY_OF_BLOCK(_FREE_DATA_BUFFER, bufferFreeFunc);

							if (tjDecompress2(tjInstance, (const unsigned char *)data, (unsigned long)size, imageData, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT) < 0)
							{
								// JPEG Decompress Error
								return GLESRawTextureInformation::kINVALID;
							}

							return LoadTextureFromBitmapDataInMemory(imageData, width, height, GL_RGB, GL_UNSIGNED_BYTE, mipmapLevel, isUsePower2Dimension);
						}
					}

					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from JPEG Stream
					GLESRawTextureInformation LoadJPEGTextureFromStream(std::istream &sourceStream, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						std::string dataString(std::istreambuf_iterator<char>(sourceStream), {});
						return LoadJPEGTextureFromMemory(dataString.c_str(), dataString.length(), mipmapLevel, isUsePower2Dimension);
					}

					//------------------------------------------------------------------------------
					// Create OpenGL ES Texture from JPEG File
					GLESRawTextureInformation LoadJPEGTextureFromFile(const char *filePath, GLint mipmapLevel, bool isUsePower2Dimension)
					{
						FILE *jpegFile = fopen(filePath, "rb");
						if (jpegFile == nullptr)
						{
							// Cannot open a file
							return GLESRawTextureInformation::kINVALID;
						}

						// Close file block
						{
							auto fileCloseFunc = [jpegFile]() mutable {
								fclose(jpegFile);
							};
							FINALLY_OF_BLOCK(_FILE_CLOSE, fileCloseFunc);

							fseek(jpegFile, 0, SEEK_END);
							size_t fileSize = ftell(jpegFile);
							fseek(jpegFile, 0, SEEK_SET);

							void *fileBuffer = malloc(fileSize);
							if (fileBuffer == NULL)
							{
								// Out of Memory
								return GLESRawTextureInformation::kINVALID;
							}

							// Buffer Auto-Free Block
							{
								auto bufferFreeFunc = [fileBuffer]() mutable {
									free(fileBuffer);
								};
								FINALLY_OF_BLOCK(_FREE_DATA_BUFFER, bufferFreeFunc);

								fread(fileBuffer, 1, fileSize, jpegFile);

								return LoadJPEGTextureFromMemory(fileBuffer, fileSize, mipmapLevel, isUsePower2Dimension);
							}
						}
					}
				}
			}
		}
	}
}
