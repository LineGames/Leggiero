////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/ColorSimpleShader_Shader.cpp (Leggiero/Modules - Graphics)
//
// Shader Source Code for Color Simple Shader Program
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ColorSimpleShader.h"


namespace Leggiero
{
	namespace Graphics
	{
		//------------------------------------------------------------------------------
		// Vertex Shader
		const char *ColorSimpleShader::s_sourceVert = 
			"uniform mat4 u_ModelMatrix;\n"
			"uniform mat4 u_ViewMatrix;\n"
			"uniform mat4 u_ProjMatrix;\n"
			"\n"
			"attribute vec4 a_Position;\n"
			"attribute vec4 a_SourceColor;\n"
			"\n"
			"varying vec4 DestinationColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	vec4 worldPosition = u_ModelMatrix * a_Position;\n"
			"	gl_Position = u_ProjMatrix * u_ViewMatrix * worldPosition;\n"
			"\n"
			"	DestinationColor = a_SourceColor;\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Fragment Shader
		const char *ColorSimpleShader::s_sourceFrag =
			"varying lowp vec4 DestinationColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_FragColor = DestinationColor;\n"
			"}\n"
			;

		//------------------------------------------------------------------------------
		// Fragment Shader with Pre-multiplied Alpha
		const char *ColorSimpleShader::s_sourceFrag_premulAlpha =
			"varying lowp vec4 DestinationColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_FragColor = vec4(DestinationColor.r, DestinationColor.g, DestinationColor.b, 1.0) * DestinationColor.a;\n"
			"}\n"
			;
	}
}
