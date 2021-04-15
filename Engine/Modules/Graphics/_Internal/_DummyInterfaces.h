////////////////////////////////////////////////////////////////////////////////
// _Internal/_DummyInterfaces.h (Leggiero/Modules - Graphics)
//   * DO NOT directly include this header file outside of Graphics project
//
// Dummy Interfaces of Graphics Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS___INTERNAL___DUMMY_INTERFACES_H
#define __LM_GRAPHICS___INTERNAL___DUMMY_INTERFACES_H


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		namespace Texture
		{
			struct GLESRawTextureInformation;
		}


		namespace _Internal
		{
			Texture::GLESRawTextureInformation _GLTextureResourceCreatorFunc_Invalid();
			Texture::GLESRawTextureInformation _GLTextureResourceCreatorFunc_Dummy();
		}
	}
}

#endif
