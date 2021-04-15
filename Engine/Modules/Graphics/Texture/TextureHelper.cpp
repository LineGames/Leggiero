////////////////////////////////////////////////////////////////////////////////
// Texture/TextureHelper.cpp (Leggiero/Modules - Graphics)
//
// Texture Helper Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureHelper.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace Texture
		{
			//////////////////////////////////////////////////////////////////////////////// GLESRawTextureInformation
			
			//------------------------------------------------------------------------------
			const GLESRawTextureInformation GLESRawTextureInformation::kINVALID = {
				kINVALID_TEXTURE_NAME, // name
				GL_TEXTURE_2D, // target
				0, 0, // width, height
				0, 0, // contentWidth, contentHeight
				false, // hasAlpha
				false // hasMimmap
			};

			//------------------------------------------------------------------------------
			// Check give texture information points same OpenGL ES Texture
			bool GLESRawTextureInformation::operator==(const GLESRawTextureInformation &compare) const
			{
				if (name == compare.name && target == compare.target)
				{
					return true;
				}
				return false;
			}


			//////////////////////////////////////////////////////////////////////////////// Utility

			//------------------------------------------------------------------------------
			GLsizei GetTexelByteSize(GLenum format, GLenum dataType)
			{
				// For OpenGL ES 3.0 Spec.

				GLsizei componentSize = 0;
				switch (dataType)
				{
					case GL_UNSIGNED_SHORT_5_6_5:
					case GL_UNSIGNED_SHORT_4_4_4_4:
					case GL_UNSIGNED_SHORT_5_5_5_1:
						return 2;

					case GL_UNSIGNED_INT_2_10_10_10_REV:
					case GL_UNSIGNED_INT_10F_11F_11F_REV:
					case GL_UNSIGNED_INT_5_9_9_9_REV:
					case GL_UNSIGNED_INT_24_8:
					case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
						return 4;

					case GL_UNSIGNED_BYTE:
					case GL_BYTE:
						componentSize = 1; break;

					case GL_UNSIGNED_SHORT:
					case GL_SHORT:
					case GL_HALF_FLOAT:
						componentSize = 2; break;

					case GL_UNSIGNED_INT:
					case GL_INT:
					case GL_FLOAT:
						componentSize = 4; break;
				}

				GLsizei componentNumber = 0;
				switch (format)
				{
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_DEPTH_COMPONENT:
					case GL_DEPTH_STENCIL:
					case GL_ALPHA:
					case GL_LUMINANCE:
						componentNumber = 1; break;

					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
						componentNumber = 2; break;

					case GL_RGB:
					case GL_RGB_INTEGER:
						componentNumber = 3; break;

					case GL_RGBA:
					case GL_RGBA_INTEGER:
						componentNumber = 4; break;
				}

				return (componentSize * componentNumber);
			}

			//------------------------------------------------------------------------------
			bool HasFormatAlpha(GLenum format)
			{
				// For OpenGL ES 3.0 Spec.

				switch (format)
				{
					case GL_ALPHA:
					case GL_LUMINANCE_ALPHA:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
						return true;
				}

				return false;
			}
		}
	}
}
