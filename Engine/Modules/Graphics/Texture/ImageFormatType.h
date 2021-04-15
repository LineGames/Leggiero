////////////////////////////////////////////////////////////////////////////////
// Texture/ImageFormatType.h (Leggiero/Modules - Graphics)
//
// Type to represent format of an Image
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__IMAGE_FORMAT_TYPE_H
#define __LM_GRAPHICS__TEXTURE__IMAGE_FORMAT_TYPE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Image File Format Type
		enum class ImageFormatType
		{
			kINVALID,

			kPNG,
			kJPEG, 
		};


		// Parse file name and detect image format form extension
		ImageFormatType GetImageFormatFromFilePathExtension(const char *filePath);
	}
}

#endif
