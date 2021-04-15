////////////////////////////////////////////////////////////////////////////////
// Texture/TextureSection.cpp (Leggiero/Modules - Graphics)
//
// Implementations related to texture sections
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureSection.h"

// Leggiero.Graphics
#include "GLTextureResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// TextureRectSection

		//------------------------------------------------------------------------------
		TextureRectSection::TextureRectSection(int pixelLeft, int pixelTop, int pixelWidth, int pixelHeight, std::shared_ptr<GLTextureResource> texture, float scaleFactor)
			: TextureRectSection(pixelLeft, pixelTop, pixelWidth, pixelHeight, ((texture) ? (int)texture->GetTextureInfo().width : 0), ((texture) ? (int)texture->GetTextureInfo().height : 0), scaleFactor)
		{
		}
	}
}
