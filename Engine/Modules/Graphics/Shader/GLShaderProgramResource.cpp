////////////////////////////////////////////////////////////////////////////////
// Shader/GLShaderProgramResource.cpp (Leggiero/Modules - Graphics)
//
// Implementation of OpenGL ES Shader Program Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLShaderProgramResource.h"

// Standard Library
#include <cstring>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>

// Leggiero.Graphics
#include "../GraphicResourceManagerComponent.h"
#include "IGLProgramStateEventObserver.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// GLShaderProgramResource

		//------------------------------------------------------------------------------
		GLShaderProgramResource::GLShaderProgramResource(
			GraphicResourceManagerComponent *ownerManager,
			const std::string &vertexShaderSourceString, const char *vertexShaderSourceCString,
			const std::string &fragmentShaderSourceString, const char *fragmentShaderSourceCString)
			: m_isInvalidated(false), m_isInitialized(false), m_managerHolder(ownerManager->GetPointerHolder())
			, m_vertexShaderInfoLog(nullptr), m_fragmentShaderInfoLog(nullptr), m_programInfoLog(nullptr)
			, m_glProgram(0)
			, m_vertexShaderSourceString(vertexShaderSourceString), m_vertexShaderSourceCString(vertexShaderSourceCString)
			, m_fragmentShaderSourceString(fragmentShaderSourceString), m_fragmentShaderSourceCString(fragmentShaderSourceCString)
		{
			m_isUsingVertexShader = !m_vertexShaderSourceString.empty() || m_vertexShaderSourceCString != nullptr;
			m_isUsingFragmentShader = !m_fragmentShaderSourceString.empty() || m_fragmentShaderSourceCString != nullptr;

			_Compile();
		}

		//------------------------------------------------------------------------------
		GLShaderProgramResource::~GLShaderProgramResource()
		{
			if (!m_isInvalidated)
			{
				std::shared_ptr<Utility::Object::PointerHolder> ownerManagerHolder(m_managerHolder.lock());
				GraphicResourceManagerComponent *ownerManager = nullptr;
				std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
				if (ownerManagerHolder)
				{
					lockContext = ownerManagerHolder->LockTarget();
					if (lockContext)
					{
						ownerManager = lockContext->GetTypedObject<GraphicResourceManagerComponent>();
					}
				}
				if (ownerManager != nullptr)
				{
					ownerManager->TrashGLGarbageResources(_GenerateResourceTrashFunc());
				}
			}
			_ClearCompileStatus();
		}

		//------------------------------------------------------------------------------
		// Use the program in OpenGL ES context
		bool GLShaderProgramResource::Use()
		{
			if (m_isInvalidated || !m_isInitialized)
			{
				return false;
			}

			glGetError();

			glUseProgram(m_glProgram);

			if (glGetError() != GL_NO_ERROR)
			{
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		// Get the location of Attribute with given name
		GLint GLShaderProgramResource::GetAttribLocation(const GLchar *name)
		{
			if (m_isInvalidated || !m_isInitialized)
			{
				return kLocation_INVALID;
			}

			glGetError();

			GLint locationValue = glGetAttribLocation(m_glProgram, name);

			if (glGetError() != GL_NO_ERROR)
			{
				return kLocation_INVALID;
			}

			return locationValue;
		}

		//------------------------------------------------------------------------------
		// Get the location of Uniform with given name
		GLint GLShaderProgramResource::GetUniformLocation(const GLchar *name)
		{
			if (m_isInvalidated || !m_isInitialized)
			{
				return kLocation_INVALID;
			}

			glGetError();

			GLint locationValue = glGetUniformLocation(m_glProgram, name);

			if (glGetError() != GL_NO_ERROR)
			{
				return kLocation_INVALID;
			}

			return locationValue;
		}

		//------------------------------------------------------------------------------
		void GLShaderProgramResource::RegisterStateEventObserver(IGLProgramStateEventObserver *observer)
		{
			m_stateEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void GLShaderProgramResource::UnRegisterStateEventObserver(IGLProgramStateEventObserver *observer)
		{
			m_stateEventNotifier.UnRegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		// Check validity of the program resource.
		// Should be called in graphic thread with OpenGL context.
		bool GLShaderProgramResource::IsValid()
		{
			if (m_isInvalidated)
			{
				return false;
			}

			if (glIsProgram(m_glProgram) == GL_TRUE
				&& (!m_isUsingVertexShader || glIsShader(m_glVertexShader) == GL_TRUE)
				&& (!m_isUsingFragmentShader || glIsShader(m_glFragmentShader) == GL_TRUE))
			{
				return true;
			}

			m_isInvalidated = true;

			// Remove Remain Resources
			if (glIsProgram(m_glProgram) == GL_TRUE)
			{
				glDeleteProgram(m_glProgram);
				m_glProgram = 0;
			}
			if (m_isUsingVertexShader && glIsShader(m_glVertexShader) == GL_TRUE)
			{
				glDeleteShader(m_glVertexShader);
			}
			if (m_isUsingFragmentShader && glIsShader(m_glFragmentShader) == GL_TRUE)
			{
				glDeleteShader(m_glFragmentShader);
			}

			_ClearCompileStatus();

			std::shared_ptr<GLShaderProgramResource> sharedThis = shared_from_this();
			m_stateEventNotifier.NotifyEvent(
				[sharedThis](IGLProgramStateEventObserver *observer) mutable
				{
					observer->OnInvalidated(sharedThis);
				});

			return false;
		}

		//------------------------------------------------------------------------------
		// Restore invalidated program resource.
		// Should be called in graphic thread with OpenGL context.
		bool GLShaderProgramResource::Restore()
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			if (!_Compile())
			{
				return false;
			}

			m_isInvalidated = false;

			std::shared_ptr<GLShaderProgramResource> sharedThis = shared_from_this();
			m_stateEventNotifier.NotifyEvent(
				[sharedThis](IGLProgramStateEventObserver *observer) mutable
				{
					observer->OnRecompiled(sharedThis);
				});

			return true;
		}

		//------------------------------------------------------------------------------
		// Create function object to finalize the resource
		std::function<void()> GLShaderProgramResource::_GenerateResourceTrashFunc()
		{
			bool isNeedToDeleteProgram = m_isProgramLinkSuccess;
			bool isNeedToDeleteVertexShader = m_isUsingVertexShader && m_isVertexShaderCompileSuccess;
			bool isNeedToDeleteFragmentShader = m_isUsingFragmentShader && m_isFragmentShaderCompileSuccess;

			GLuint glProgram = m_glProgram;
			GLuint glVertexShader = m_glVertexShader;
			GLuint glFragmentShader = m_glFragmentShader;

			return [isNeedToDeleteProgram, isNeedToDeleteVertexShader, isNeedToDeleteFragmentShader, glProgram, glVertexShader, glFragmentShader]() mutable {
				if (isNeedToDeleteProgram && (glIsProgram(glProgram) == GL_TRUE))
				{
					glDeleteProgram(glProgram);
				}
				if (isNeedToDeleteVertexShader && (glIsShader(glVertexShader) == GL_TRUE))
				{
					glDeleteShader(glVertexShader);
				}
				if (isNeedToDeleteFragmentShader && (glIsShader(glFragmentShader) == GL_TRUE))
				{
					glDeleteShader(glFragmentShader);
				}
			};
		}

		//------------------------------------------------------------------------------
		// Clear compile state informations
		void GLShaderProgramResource::_ClearCompileStatus()
		{
			m_isVertexShaderCompileSuccess = false;
			if (m_vertexShaderInfoLog != nullptr)
			{
				free(m_vertexShaderInfoLog);
				m_vertexShaderInfoLog = nullptr;
			}

			m_isFragmentShaderCompileSuccess = false;
			if (m_fragmentShaderInfoLog != nullptr)
			{
				free(m_fragmentShaderInfoLog);
				m_fragmentShaderInfoLog = nullptr;
			}

			m_isProgramLinkSuccess = false;
			if (m_programInfoLog != nullptr)
			{
				free(m_programInfoLog);
				m_programInfoLog = nullptr;
			}

			m_isInitialized = false;
		}

		//------------------------------------------------------------------------------
		// Compile shader sources and create OpenGL Shader Program
		bool GLShaderProgramResource::_Compile()
		{
			bool isErrorOccured = false;

			// Vertex Shader Compile
			m_isVertexShaderCompileSuccess = false;
			if (m_isUsingVertexShader)
			{
				do
				{
					m_glVertexShader = glCreateShader(GL_VERTEX_SHADER);
					if (m_glVertexShader == 0)
					{
						// Shader Creation Error
						isErrorOccured = true;
						break;
					}

					glGetError();

					if (m_vertexShaderSourceCString == nullptr)
					{
						const char *stringCString = m_vertexShaderSourceString.c_str();
						glShaderSource(m_glVertexShader, 1, &stringCString, NULL);
					}
					else
					{
						glShaderSource(m_glVertexShader, 1, &m_vertexShaderSourceCString, NULL);
					}
					if (glGetError() != GL_NO_ERROR)
					{
						// Shader Source Error
						glDeleteShader(m_glVertexShader);
						isErrorOccured = true;
						break;
					}

					glCompileShader(m_glVertexShader);

					GLint compileStatus;
					glGetShaderiv(m_glVertexShader, GL_COMPILE_STATUS, &compileStatus);
					if (compileStatus == GL_FALSE)
					{
						GLint shaderInfoLogLength;
						glGetShaderiv(m_glVertexShader, GL_INFO_LOG_LENGTH, &shaderInfoLogLength);
						if (shaderInfoLogLength > 0)
						{
							m_vertexShaderInfoLog = (char *)malloc(shaderInfoLogLength);
							glGetShaderInfoLog(m_glVertexShader, shaderInfoLogLength, NULL, m_vertexShaderInfoLog);
						}

						glDeleteShader(m_glVertexShader);
						isErrorOccured = true;
						break;
					}

					m_isVertexShaderCompileSuccess = true;
				} while (false);
			}

			// Fragment Shader Compile
			m_isFragmentShaderCompileSuccess = false;
			if (m_isUsingFragmentShader)
			{
				do
				{
					m_glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
					if (m_glFragmentShader == 0)
					{
						// Shader Creation Error
						isErrorOccured = true;
						break;
					}

					glGetError();

					if (m_fragmentShaderSourceCString == nullptr)
					{
						const char *stringCString = m_fragmentShaderSourceString.c_str();
						glShaderSource(m_glFragmentShader, 1, &stringCString, NULL);
					}
					else
					{
						glShaderSource(m_glFragmentShader, 1, &m_fragmentShaderSourceCString, NULL);
					}
					if (glGetError() != GL_NO_ERROR)
					{
						// Shader Source Error
						glDeleteShader(m_glFragmentShader);
						isErrorOccured = true;
						break;
					}

					glCompileShader(m_glFragmentShader);

					GLint compileStatus;
					glGetShaderiv(m_glFragmentShader, GL_COMPILE_STATUS, &compileStatus);
					if (compileStatus == GL_FALSE)
					{
						GLint shaderInfoLogLength;
						glGetShaderiv(m_glFragmentShader, GL_INFO_LOG_LENGTH, &shaderInfoLogLength);
						if (shaderInfoLogLength > 0)
						{
							m_fragmentShaderInfoLog = (char *)malloc(shaderInfoLogLength);
							glGetShaderInfoLog(m_glFragmentShader, shaderInfoLogLength, NULL, m_fragmentShaderInfoLog);
						}

						glDeleteShader(m_glFragmentShader);
						isErrorOccured = true;
						break;
					}

					m_isFragmentShaderCompileSuccess = true;
				} while (false);
			}

			// Link Program
			m_isProgramLinkSuccess = false;
			if ((!m_isUsingVertexShader || m_isVertexShaderCompileSuccess) && (!m_isUsingFragmentShader || m_isFragmentShaderCompileSuccess))
			{
				do
				{
					m_glProgram = glCreateProgram();
					if (m_glProgram == 0)
					{
						// Shader Creation Error
						break;
					}

					glGetError();

					if (m_isUsingVertexShader)
					{
						glAttachShader(m_glProgram, m_glVertexShader);
						if (glGetError() != GL_NO_ERROR)
						{
							// Shader Attach Error
							glDeleteProgram(m_glProgram);
							isErrorOccured = true;
							break;
						}
					}

					if (m_isUsingFragmentShader)
					{
						glAttachShader(m_glProgram, m_glFragmentShader);
						if (glGetError() != GL_NO_ERROR)
						{
							// Shader Attach Error
							glDeleteProgram(m_glProgram);
							isErrorOccured = true;
							break;
						}
					}

					glLinkProgram(m_glProgram);

					GLint linkStatus;
					glGetProgramiv(m_glProgram, GL_LINK_STATUS, &linkStatus);
					if (linkStatus == GL_FALSE)
					{
						GLint programInfoLogLength;
						glGetProgramiv(m_glProgram, GL_INFO_LOG_LENGTH, &programInfoLogLength);
						if (programInfoLogLength > 0)
						{
							m_programInfoLog = (char *)malloc(programInfoLogLength);
							glGetProgramInfoLog(m_glProgram, programInfoLogLength, NULL, m_programInfoLog);
						}

						glDeleteProgram(m_glProgram);
						isErrorOccured = true;
						break;
					}

					m_isProgramLinkSuccess = true;
				} while (false);
			}

			if (!isErrorOccured)
			{
				m_isInitialized = true;
			}

			return m_isInitialized;
		}
	}
}
