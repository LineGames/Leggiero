////////////////////////////////////////////////////////////////////////////////
// Texture/ImageFormatType.cpp (Leggiero/Modules - Graphics)
//
// Implementations related to image format type
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ImageFormatType.h"


namespace Leggiero
{
	namespace Graphics
	{
		//------------------------------------------------------------------------------
		ImageFormatType GetImageFormatFromFilePathExtension(const char *filePath)
		{
			if (filePath == nullptr)
			{
				return ImageFormatType::kINVALID;
			}

			const char *lastDot = strrchr(filePath, '.');
			if (lastDot != nullptr)
			{
				const int kMaxExtensionSize = 16;

				char extensionBuffer[kMaxExtensionSize + 1];
				strncpy(extensionBuffer, lastDot + 1, kMaxExtensionSize);
				extensionBuffer[kMaxExtensionSize] = '\0';

				int extensionLength = (int)strlen(extensionBuffer);
				for (int i = 0; i < extensionLength; ++i)
				{
					extensionBuffer[i] = (char)(toupper(extensionBuffer[i]));
				}

				if (strcmp(extensionBuffer, "PNG") == 0)
				{
					return ImageFormatType::kPNG;
				}
				else if (strcmp(extensionBuffer, "JPG") == 0 || strcmp(extensionBuffer, "JPEG") == 0)
				{
					return ImageFormatType::kJPEG;
				}
			}

			return ImageFormatType::kINVALID;
		}
	}
}
