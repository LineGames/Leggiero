////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/_Internal/_OpenALSourceReleasingContext.cpp (Leggiero/Modules - Sound)
//
// OpenAL Source Releasing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_OpenALSourceReleasingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			namespace _Internal
			{
				//------------------------------------------------------------------------------
				OpenALSourceReleasingContext::OpenALSourceReleasingContext(ALuint source)
					: m_source(source)
					, m_isReleaseCompleted(false)
				{
					alSourceStop(m_source);
				}

				//------------------------------------------------------------------------------
				OpenALSourceReleasingContext::~OpenALSourceReleasingContext()
				{
				}

				//------------------------------------------------------------------------------
				void OpenALSourceReleasingContext::Update(std::vector<ALuint> &releasingBuffers)
				{
					// Wait to be stopped
					ALint sourceState;
					alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
					if (sourceState != AL_STOPPED)
					{
						return;
					}

					// Release Buffers
					ALint processedBuffers;
					alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
					if (processedBuffers > 0)
					{
						alGetError();
						for (int i = 0; i < processedBuffers; ++i)
						{
							ALuint dequeuedBuffer;
							alSourceUnqueueBuffers(m_source, 1, &dequeuedBuffer);
							if (alGetError() == AL_NO_ERROR)
							{
								releasingBuffers.push_back(dequeuedBuffer);
							}
						}
					}

					// Mark Compelete
					m_isReleaseCompleted = true;
				}
			}
		}
	}
}
