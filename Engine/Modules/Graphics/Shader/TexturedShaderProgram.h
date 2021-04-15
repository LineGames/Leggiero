////////////////////////////////////////////////////////////////////////////////
// Shader/TexturedShaderProgram.h (Leggiero/Modules - Graphics)
//
// Base class for shader program with texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__TEXTURED_SHADER_PROGRAM_H
#define __LM_GRAPHICS__SHADER__TEXTURED_SHADER_PROGRAM_H


// Leggiero.Graphics
#include "CommonBaseShaderProgram.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Textur Shader Program Base Class
		class TexturedShaderProgram
			: public CommonBaseShaderProgram
		{
		public:
			TexturedShaderProgram(std::shared_ptr<GLShaderProgramResource> programResource)
				: CommonBaseShaderProgram(programResource)
			{ }

			virtual ~TexturedShaderProgram() { }

		public:
			virtual GLint GetTextureUVSlot() const = 0;
			virtual GLint GetTextureLocation() const = 0;
		};
	}
}

#endif
