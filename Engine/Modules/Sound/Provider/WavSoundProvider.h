////////////////////////////////////////////////////////////////////////////////
// Provider/WavSoundProvider.h (Leggiero/Modules - Sound)
//
// Sound provider for WAV format data
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__WAV_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__WAV_SOUND_PROVIDER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdio>
#include <istream>
#include <memory>
#include <string>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		// Forward Declaration
		namespace WaveFormat
		{
			struct WaveFileFmtChunk;
		}


		// WAV sound data provider
		class WavSoundProvider
			: public ISoundProvider
		{
		public:
			WavSoundProvider(FILE *source, bool isCloseFileOnDestroy = false);
			WavSoundProvider(const char *filePath);
			WavSoundProvider(std::string filePath);
			WavSoundProvider(std::istream *source);
			WavSoundProvider(std::shared_ptr<std::istream> source);
			WavSoundProvider(const void *data, size_t size);
			virtual ~WavSoundProvider();

		protected:
			void _initializeProviderFromFile(FILE *source);
			void _InitializeSoundData(const WaveFormat::WaveFileFmtChunk *format, size_t dataChunkSize);

		protected:
			// Prevent Creating of NULL Sound Provider
			WavSoundProvider() { }

		public:	// ISoundProvider
			virtual SampleFormat	GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType GetFrequency() const override { return m_frequency; }

			virtual size_t	GetSampleLength() const override { return (size_t)m_lengthInSample; }
			virtual float	GetLengthInSecond() const override { return (float)m_lengthInSecond; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		public:
			bool IsValid() const { return m_isInitialized; }

		protected:
			size_t	_FillSampleData_Stream(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples, std::istream *targetStream);
			size_t	_FillSampleData_File(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples);
			size_t	_FillSampleData_MemoryData(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples);

		protected:
			enum class DataSourceType
			{
				kINVALID,

				kStream,
				kStreamSharedPtr,
				kFile,
				kMemory,
			};

		protected:
			bool	m_isInitialized;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;
			int						m_channelNumber;

			double	m_lengthInSecond;
			size_t	m_lengthInSample;

			DataSourceType	m_dataSourceType;

			std::shared_ptr<std::istream>	m_sharedIStreamPtr;
			std::istream					*m_iStreamPtr;
			const void						*m_memoryData;

			bool	m_isOwnFileSource;
			FILE	*m_fileSource;

			size_t	m_dataStartOffset;

			Utility::Threading::SafePthreadLock	m_sourceFileMutex;
		};
	}
}

#endif
