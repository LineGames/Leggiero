////////////////////////////////////////////////////////////////////////////////
// Provider/MP3SoundProvider.h (Leggiero/Modules - Sound)
//
// Sound provider for MP3 format data
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__MP3_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__MP3_SOUND_PROVIDER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdio>
#include <istream>
#include <memory>
#include <string>

// External Library
#include <minimp3/minimp3_ex.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		// MP3 Vorbis Sound Data Provider
		class MP3SoundProvider : public ISoundProvider
		{
		public:
			MP3SoundProvider(FILE *source, bool needToClose = false);
			MP3SoundProvider(const char *filePath);
			MP3SoundProvider(const std::string &filePath);
			MP3SoundProvider(std::istream *source);
			MP3SoundProvider(std::shared_ptr<std::istream> source);
			MP3SoundProvider(const void *memoryBuffer, size_t bufferSize);
			virtual ~MP3SoundProvider();

		protected:
			void _InitializeSoundData();

		private:
			// Prevent Creating of NULL Sound Provider
			MP3SoundProvider() { }

		public:	// ISoundProvider
			virtual SampleFormat			GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType	GetFrequency() const override { return m_frequency; }

			virtual size_t	GetSampleLength() const override { return static_cast<size_t>(m_lengthInSample); }
			virtual float	GetLengthInSecond() const override { return m_lengthInSecond; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		public:
			bool IsValid() const { return m_isInitialized; }

		protected:
			bool	m_isInitialized;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;
			int						m_channelNumber;

			size_t					m_sampleByteSize;

			float				m_lengthInSecond;
			SampleNumberType	m_lengthInSample;

			FILE *m_ownFileSource;

			std::shared_ptr<std::istream> m_sharedIStreamPtr;

			mp3dec_io_t m_mp3IO;

			mp3dec_ex_t *m_mp3Object;
			Utility::Threading::SafePthreadLock m_sourceFileMutex;

			SampleNumberType m_lastReadSamplePosition;
		};
	}
}

#endif
