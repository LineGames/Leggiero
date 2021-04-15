////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/TexturedColorSimpleShader_Shader.cpp (Leggiero/Modules - Graphics)
//
// Shader Source Code for Textured Color Simple Shader Program
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TexturedColorSimpleShader.h"


namespace Leggiero
{
	namespace Graphics
	{
		//------------------------------------------------------------------------------
		// Vertex Shader
		const char *TexturedColorSimpleShader::s_sourceVert =
			"uniform mat4 u_ModelMatrix;\n"
			"uniform mat4 u_ViewMatrix;\n"
			"uniform mat4 u_ProjMatrix;\n"
			"\n"
			"attribute vec4 a_Position;\n"
			"attribute vec4 a_SourceColor;\n"
			"attribute vec2 a_TextureUV0;\n"
			"\n"
			"varying vec4 DestinationColor;\n"
			"varying vec2 TextureUV0;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	vec4 worldPosition = u_ModelMatrix * a_Position;\n"
			"	gl_Position = u_ProjMatrix * u_ViewMatrix * worldPosition;\n"
			"\n"
			"	DestinationColor = a_SourceColor;\n"
			"	TextureUV0 = a_TextureUV0;\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Fragment Shader
		const char *TexturedColorSimpleShader::s_sourceFrag =
			"varying lowp vec4 DestinationColor;\n"
			"varying mediump vec2 TextureUV0;\n"
			"\n"
			"uniform sampler2D u_Texture0;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	mediump vec4 texture0Color = texture2D(u_Texture0, TextureUV0);\n"
			"	if (DestinationColor.a * texture0Color.a < 0.0001)\n"
			"	{\n"
			"		discard;\n"
			"	}\n"
			"	gl_FragColor = DestinationColor * texture0Color;\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Fragment Shader with Pre-multiplied Alpha
		const char *TexturedColorSimpleShader::s_sourceFrag_premulAlpha =
			"varying lowp vec4 DestinationColor;\n"
			"varying mediump vec2 TextureUV0;\n"
			"\n"
			"uniform sampler2D u_Texture0;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	mediump vec4 effectiveColor = DestinationColor * texture2D(u_Texture0, TextureUV0);\n"
			"	if (effectiveColor.a < 0.0001)\n"
			"	{\n"
			"		discard;\n"
			"	}\n"
			"	gl_FragColor = vec4(effectiveColor.r, effectiveColor.g, effectiveColor.b, 1.0) * effectiveColor.a;\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Tinting Fragment Shader
		const char *TexturedColorSimpleShader::s_sourceFrag_tint =
			"varying lowp vec4 DestinationColor;\n"
			"varying mediump vec2 TextureUV0;\n"
			"\n"
			"uniform sampler2D u_Texture0;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	mediump float texture0Alpha = texture2D(u_Texture0, TextureUV0).a;\n"
			"	mediump float effectiveAlpha = DestinationColor.a * texture0Alpha;\n"
			"	if (effectiveAlpha < 0.0001)\n"
			"	{\n"
			"		discard;\n"
			"	}\n"
			"	gl_FragColor = vec4(DestinationColor.r, DestinationColor.g, DestinationColor.b, effectiveAlpha);\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Tinting Fragment Shader with Pre-multiplied Alpha
		const char *TexturedColorSimpleShader::s_sourceFrag_tint_premulAlpha =
			"varying lowp vec4 DestinationColor;\n"
			"varying mediump vec2 TextureUV0;\n"
			"\n"
			"uniform sampler2D u_Texture0;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	mediump float effectiveAlpha = DestinationColor.a * texture2D(u_Texture0, TextureUV0).a;\n"
			"	if (effectiveAlpha < 0.0001)\n"
			"	{\n"
			"		discard;\n"
			"	}\n"
			"	gl_FragColor = vec4(DestinationColor.r, DestinationColor.g, DestinationColor.b, 1.0) * effectiveAlpha;\n"
			"}\n"
			;
	}
}
