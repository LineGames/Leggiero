////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/_Internal/_OpenALSourceReleasingContext.h (Leggiero/Modules - Sound)
//
// Source releasing context after delete
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND___INTERNAL___OPENAL_SOURCE_RELEASING_CONTEXT_H
#define __LM_SOUND__OPENAL_BACKEND___INTERNAL___OPENAL_SOURCE_RELEASING_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <vector>

// External Library
#include <OpenAL.h>


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			// Forward Declaration
			class OpenALSoundMixer;


			namespace _Internal
			{
				// Source Releasing Context
				class OpenALSourceReleasingContext
				{
					friend class OpenALSoundMixer;

				public:
					OpenALSourceReleasingContext(ALuint source);
					virtual ~OpenALSourceReleasingContext();

				public:
					void Update(std::vector<ALuint> &releasingBuffers);
					bool IsReleaseCompleted() const { return m_isReleaseCompleted; }

				public:
					ALuint	m_source;
					bool	m_isReleaseCompleted;
				};
			}
		}
	}
}

#endif
