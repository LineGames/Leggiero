////////////////////////////////////////////////////////////////////////////////
// Shader/CommonGLVertexType.h (Leggiero/Modules - Graphics)
//
// Common basic vertex data structure for OpenGL
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__COMMON_GL_VERTEX_TYPE_H
#define __LM_GRAPHICS__SHADER__COMMON_GL_VERTEX_TYPE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>


namespace Leggiero
{
	namespace Graphics
	{

// Make vertices have fitting size
#pragma pack(push, 1)

		// Vertex that only have position value
		struct PositionVertex
		{
		public:
			float	position[4];

		public:
			PositionVertex LerpWith(const PositionVertex &withVertex, float param) const
			{
				return Lerp(*this, withVertex, param);
			}

		public:
			static void SetGLVertexAttribPointer(GLuint positionSlot, const PositionVertex *pArrayStart)
			{
				glVertexAttribPointer(positionSlot, 4, GL_FLOAT, GL_FALSE, sizeof(PositionVertex), &pArrayStart->position);
			}

			static PositionVertex Lerp(const PositionVertex &v1, const PositionVertex &v2, float param)
			{
				PositionVertex resultVertex;
				resultVertex.position[0] = Utility::Math::LerpValue(v1.position[0], v2.position[0], param);
				resultVertex.position[1] = Utility::Math::LerpValue(v1.position[1], v2.position[1], param);
				resultVertex.position[2] = Utility::Math::LerpValue(v1.position[2], v2.position[2], param);
				resultVertex.position[3] = Utility::Math::LerpValue(v1.position[3], v2.position[3], param);

				return resultVertex;
			}
		};


		// Basic vertex with position and color of each point
		struct ColoredVertex
		{
		public:
			float	position[4];
			GLubyte color[4];

		public:
			ColoredVertex LerpWith(const ColoredVertex &withVertex, float param) const
			{
				return Lerp(*this, withVertex, param);
			}

		public:
			static void SetGLVertexAttribPointer(GLuint positionSlot, GLuint colorSlot, const ColoredVertex *pArrayStart)
			{
				glVertexAttribPointer(positionSlot, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), &pArrayStart->position);
				glVertexAttribPointer(colorSlot, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColoredVertex), &pArrayStart->color);
			}

			static ColoredVertex Lerp(const ColoredVertex &v1, const ColoredVertex &v2, float param)
			{
				ColoredVertex resultVertex;
				resultVertex.position[0] = Utility::Math::LerpValue(v1.position[0], v2.position[0], param);
				resultVertex.position[1] = Utility::Math::LerpValue(v1.position[1], v2.position[1], param);
				resultVertex.position[2] = Utility::Math::LerpValue(v1.position[2], v2.position[2], param);
				resultVertex.position[3] = Utility::Math::LerpValue(v1.position[3], v2.position[3], param);

				resultVertex.color[0] = Utility::Math::LerpValue(v1.color[0], v2.color[0], param);
				resultVertex.color[1] = Utility::Math::LerpValue(v1.color[1], v2.color[1], param);
				resultVertex.color[2] = Utility::Math::LerpValue(v1.color[2], v2.color[2], param);
				resultVertex.color[3] = Utility::Math::LerpValue(v1.color[3], v2.color[3], param);

				return resultVertex;
			}
		};


		// Vertex with position, texel uv coordinates, and blending color
		struct TextureColoredVertex
		{
		public:
			float	position[4];
			GLubyte color[4];
			float	textureUV[2];

		public:
			TextureColoredVertex LerpWith(const TextureColoredVertex &withVertex, float param) const
			{
				return Lerp(*this, withVertex, param);
			}

		public:
			static void SetGLVertexAttribPointer(GLuint positionSlot, GLuint colorSlot, GLuint textureUVSlot, const TextureColoredVertex *pArrayStart)
			{
				glVertexAttribPointer(positionSlot, 4, GL_FLOAT, GL_FALSE, sizeof(TextureColoredVertex), &pArrayStart->position);
				glVertexAttribPointer(colorSlot, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TextureColoredVertex), &pArrayStart->color);
				glVertexAttribPointer(textureUVSlot, 2, GL_FLOAT, GL_FALSE, sizeof(TextureColoredVertex), &pArrayStart->textureUV);
			}

			static TextureColoredVertex Lerp(const TextureColoredVertex &v1, const TextureColoredVertex &v2, float param)
			{
				TextureColoredVertex resultVertex;
				resultVertex.position[0] = Utility::Math::LerpValue(v1.position[0], v2.position[0], param);
				resultVertex.position[1] = Utility::Math::LerpValue(v1.position[1], v2.position[1], param);
				resultVertex.position[2] = Utility::Math::LerpValue(v1.position[2], v2.position[2], param);
				resultVertex.position[3] = Utility::Math::LerpValue(v1.position[3], v2.position[3], param);

				resultVertex.color[0] = Utility::Math::LerpValue(v1.color[0], v2.color[0], param);
				resultVertex.color[1] = Utility::Math::LerpValue(v1.color[1], v2.color[1], param);
				resultVertex.color[2] = Utility::Math::LerpValue(v1.color[2], v2.color[2], param);
				resultVertex.color[3] = Utility::Math::LerpValue(v1.color[3], v2.color[3], param);

				resultVertex.textureUV[0] = Utility::Math::LerpValue(v1.textureUV[0], v2.textureUV[0], param);
				resultVertex.textureUV[1] = Utility::Math::LerpValue(v1.textureUV[1], v2.textureUV[1], param);

				return resultVertex;
			}
		};

#pragma pack(pop)

	}
}

#endif
