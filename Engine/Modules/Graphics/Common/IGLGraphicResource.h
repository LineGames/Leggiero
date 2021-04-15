////////////////////////////////////////////////////////////////////////////////
// Common/IGLGraphicResource.h (Leggiero/Modules - Graphics)
//
// Common Base Interface for OpenGL ES Graphic Resource
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__COMMON__I_GL_GRAPHIC_RESOURCE_H
#define __LM_GRAPHICS__COMMON__I_GL_GRAPHIC_RESOURCE_H


namespace Leggiero
{
	namespace Graphics
	{
		// OpenGL ES Graphic Resource Interface
		class IGLGraphicResource
		{
		public:
			virtual bool IsValid() = 0;
			virtual bool Restore() = 0;
		};
	}
}

#endif
