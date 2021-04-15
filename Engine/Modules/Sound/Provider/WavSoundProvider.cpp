////////////////////////////////////////////////////////////////////////////////
// Provider/WavSoundProvider.cpp (Leggiero/Modules - Sound)
//
// WAV sound provider implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WavSoundProvider.h"

// Standard Library
#include <cstdint>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// Wav Format

		#ifndef WAVE_FORMAT_PCM
			#define WAVE_FORMAT_PCM ((uint16_t)0x0001)
		#endif

		namespace WaveFormat
		{
			const char _kWaveFileSig[] = "RIFF";
			const char _kWaveId[] = "WAVE";
			const char _kFmtChunkId[] = "fmt ";
			const char _kDataChunkId[] = "data";
			const char _kJunkChunkId[] = "JUNK";
			const char _kListChunkId[] = "LIST";

			#if (defined(_MSC_VER)) || (defined(__clang__))
				#pragma pack(push, 1)
			#elif defined(__GNUC__)
				#pragma pack(1)
			#endif

			struct WaveFileHeader
			{
			public:
				char		ckID[4];
				uint32_t	cksize;
				char		WAVEID[4];

			public:
				bool IsValidWaveFileHeader() const
				{
					for (int i = 0; i < sizeof(ckID); ++i)
					{
						if (ckID[i] != _kWaveFileSig[i])
							return false;
					}
					for (int i = 0; i < sizeof(WAVEID); ++i)
					{
						if (WAVEID[i] != _kWaveId[i])
							return false;
					}
					return true;
				}
			};

			struct WaveChunkHeader
			{
			public:
				char		ckID[4];
				uint32_t	cksize;

			public:
				bool IsValidFmtChunkHeader() const
				{
					return _IsChunkHeaderOf(_kFmtChunkId);
				}

				bool IsValidDataChunkHeader() const
				{
					return _IsChunkHeaderOf(_kDataChunkId);
				}

				bool IsSkippingChunkHeader() const
				{
					if (_IsChunkHeaderOf(_kJunkChunkId))
					{
						return true;
					}
					if (_IsChunkHeaderOf(_kListChunkId))
					{
						return true;
					}
					return false;
				}

			protected:
				bool _IsChunkHeaderOf(const char *checkId) const
				{
					for (int i = 0; i < sizeof(ckID); ++i)
					{
						if (ckID[i] != checkId[i])
							return false;
					}
					return true;
				}
			};

			struct WaveFileFmtChunk
			{
			public:
				WaveChunkHeader header;
				uint16_t		wFormatTag;
				uint16_t		nChannels;
				uint32_t		nSamplesPerSec;
				uint32_t		nAvgBytesPerSec;
				uint16_t		nBlockAlign;
				uint16_t		wBitsPerSample;
			};

			#if (defined(_MSC_VER)) || (defined(__clang__))
				#pragma pack(pop)
			#elif defined(__GNUC__)
				#pragma pack()
			#endif
		}


		//////////////////////////////////////////////////////////////////////////////// WavSoundProvider

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(FILE *source, bool isCloseFileOnDestroy)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false), m_dataStartOffset(0)
			, m_isOwnFileSource(isCloseFileOnDestroy), m_fileSource(source), m_sharedIStreamPtr(nullptr), m_iStreamPtr(nullptr), m_memoryData(nullptr), m_dataSourceType(DataSourceType::kINVALID)
		{
			_initializeProviderFromFile(source);
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(const char *filePath)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false), m_dataStartOffset(0)
			, m_isOwnFileSource(false), m_fileSource(nullptr), m_sharedIStreamPtr(nullptr), m_iStreamPtr(nullptr), m_memoryData(nullptr), m_dataSourceType(DataSourceType::kINVALID)
		{
			m_fileSource = fopen(filePath, "rb");
			if (m_fileSource == nullptr)
			{
				// Cannot open a file
				return;
			}
			m_isOwnFileSource = true;

			_initializeProviderFromFile(m_fileSource);
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(std::string filePath)
			: WavSoundProvider(filePath.c_str())
		{
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(std::istream *source)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false), m_dataStartOffset(0)
			, m_isOwnFileSource(false), m_fileSource(nullptr), m_sharedIStreamPtr(nullptr), m_iStreamPtr(nullptr), m_memoryData(nullptr), m_dataSourceType(DataSourceType::kINVALID)
		{
			WaveFormat::WaveFileHeader waveHeader;
			source->read((char *)(&waveHeader), (std::streamsize)(sizeof(WaveFormat::WaveFileHeader)));
			if (source->gcount() != sizeof(WaveFormat::WaveFileHeader))
			{
				return;
			}
			if (!waveHeader.IsValidWaveFileHeader())
			{
				return;
			}

			// Skip Until Fmt Chunk
			WaveFormat::WaveChunkHeader checkingChunkHeader;
			source->read((char *)(&checkingChunkHeader), (std::streamsize)(sizeof(WaveFormat::WaveChunkHeader)));
			if (source->gcount() != sizeof(WaveFormat::WaveChunkHeader))
			{
				return;
			}
			while (checkingChunkHeader.IsSkippingChunkHeader())
			{
				source->seekg((std::streamoff)checkingChunkHeader.cksize, std::ios_base::cur);
				if (source->fail() || source->bad() || source->eof())
				{
					return;
				}
				source->read((char *)(&checkingChunkHeader), (std::streamsize)(sizeof(WaveFormat::WaveChunkHeader)));
				if (source->gcount() != sizeof(WaveFormat::WaveChunkHeader))
				{
					return;
				}
			}

			if (!checkingChunkHeader.IsValidFmtChunkHeader())
			{
				return;
			}
			WaveFormat::WaveFileFmtChunk fmtChunk;
			fmtChunk.header = checkingChunkHeader;
			std::streamsize remainFmtChunkSize = (std::streamsize)(sizeof(WaveFormat::WaveFileFmtChunk) - ((unsigned char *)&fmtChunk.wFormatTag - (unsigned char *)&fmtChunk));
			source->read((char *)(&fmtChunk.wFormatTag), remainFmtChunkSize);
			if (source->gcount() != remainFmtChunkSize)
			{
				return;
			}
			if (fmtChunk.wFormatTag != WAVE_FORMAT_PCM)
			{
				// Only PCM format wave file is supported now
				return;
			}
			if (fmtChunk.header.cksize != sizeof(WaveFormat::WaveFileFmtChunk) - sizeof(WaveFormat::WaveChunkHeader))
			{
				// Unsupported format chunk
				return;
			}

			// Skip Until Data Chunk
			WaveFormat::WaveChunkHeader dataChunkHeader;
			source->read((char *)(&dataChunkHeader), (std::streamsize)(sizeof(WaveFormat::WaveChunkHeader)));
			if (source->gcount() != sizeof(WaveFormat::WaveChunkHeader))
			{
				return;
			}
			while (dataChunkHeader.IsSkippingChunkHeader())
			{
				source->seekg((std::streamoff)dataChunkHeader.cksize, std::ios_base::cur);
				if (source->fail() || source->bad() || source->eof())
				{
					return;
				}
				source->read((char *)(&dataChunkHeader), (std::streamsize)(sizeof(WaveFormat::WaveChunkHeader)));
				if (source->gcount() != sizeof(WaveFormat::WaveChunkHeader))
				{
					return;
				}
			}
			if (!dataChunkHeader.IsValidDataChunkHeader())
			{
				return;
			}

			std::streampos streamPosition = source->tellg();
			if (streamPosition == (std::streampos)(-1))
			{
				// Stream Error
				return;
			}

			// Set Data Source
			m_dataSourceType = DataSourceType::kStream;
			m_dataStartOffset = (size_t)streamPosition;
			m_iStreamPtr = source;

			_InitializeSoundData(&fmtChunk, (size_t)dataChunkHeader.cksize);
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(std::shared_ptr<std::istream> source)
			: WavSoundProvider(source.get())
		{
			// Change Source Type
			m_dataSourceType = DataSourceType::kStreamSharedPtr;
			m_sharedIStreamPtr = source;
			m_iStreamPtr = nullptr;
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::WavSoundProvider(const void *data, size_t size)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false), m_dataStartOffset(0)
			, m_isOwnFileSource(false), m_fileSource(nullptr), m_sharedIStreamPtr(nullptr), m_iStreamPtr(nullptr), m_memoryData(nullptr), m_dataSourceType(DataSourceType::kINVALID)
		{
			size_t currentReadOffset = 0;

			size_t minimumDataSize = sizeof(WaveFormat::WaveFileHeader) + sizeof(WaveFormat::WaveFileFmtChunk) + sizeof(WaveFormat::WaveChunkHeader);
			if (size < minimumDataSize)
			{
				// Invalid Wave Data
				return;
			}

			const WaveFormat::WaveFileHeader &waveHeader = *((const WaveFormat::WaveFileHeader *)data);
			currentReadOffset += sizeof(WaveFormat::WaveFileHeader);
			if (!waveHeader.IsValidWaveFileHeader())
			{
				return;
			}
			if (size < sizeof(WaveFormat::WaveFileHeader) + waveHeader.cksize - sizeof(waveHeader.WAVEID))
			{
				return;
			}

			// Skip Until Fmt Chunk
			const WaveFormat::WaveChunkHeader *checkingChunkHeader = (const WaveFormat::WaveChunkHeader *)((unsigned char *)data + currentReadOffset);
			currentReadOffset += sizeof(WaveFormat::WaveChunkHeader);
			while (checkingChunkHeader->IsSkippingChunkHeader())
			{
				minimumDataSize += sizeof(WaveFormat::WaveChunkHeader) + checkingChunkHeader->cksize;
				if (size < minimumDataSize)
				{
					return;
				}
				currentReadOffset += checkingChunkHeader->cksize;

				checkingChunkHeader = (const WaveFormat::WaveChunkHeader *)((unsigned char *)data + currentReadOffset);
				currentReadOffset += sizeof(WaveFormat::WaveChunkHeader);
			}

			if (!checkingChunkHeader->IsValidFmtChunkHeader())
			{
				return;
			}
			const WaveFormat::WaveFileFmtChunk &fmtChunk = *((const WaveFormat::WaveFileFmtChunk *)checkingChunkHeader);
			currentReadOffset += sizeof(WaveFormat::WaveFileFmtChunk) - ((unsigned char *)&fmtChunk.wFormatTag - (unsigned char *)&fmtChunk);
			if (fmtChunk.wFormatTag != WAVE_FORMAT_PCM)
			{
				// Only PCM format wave file is supported now
				return;
			}
			if (fmtChunk.header.cksize != sizeof(WaveFormat::WaveFileFmtChunk) - sizeof(WaveFormat::WaveChunkHeader))
			{
				// Unsupported format chunk
				return;
			}

			// Skip Until Data Chunk
			checkingChunkHeader = (const WaveFormat::WaveChunkHeader *)((unsigned char *)data + currentReadOffset);
			currentReadOffset += sizeof(WaveFormat::WaveChunkHeader);
			while (checkingChunkHeader->IsSkippingChunkHeader())
			{
				minimumDataSize += sizeof(WaveFormat::WaveChunkHeader) + checkingChunkHeader->cksize;
				if (size < minimumDataSize)
				{
					return;
				}
				currentReadOffset += checkingChunkHeader->cksize;

				checkingChunkHeader = (const WaveFormat::WaveChunkHeader *)((unsigned char *)data + currentReadOffset);
				currentReadOffset += sizeof(WaveFormat::WaveChunkHeader);
			}

			const WaveFormat::WaveChunkHeader &dataChunkHeader = *checkingChunkHeader;
			if (!dataChunkHeader.IsValidDataChunkHeader())
			{
				return;
			}
			if (size < currentReadOffset + dataChunkHeader.cksize)
			{
				return;
			}

			// Set Data Source
			m_dataSourceType = DataSourceType::kMemory;
			m_dataStartOffset = currentReadOffset;
			m_memoryData = data;

			_InitializeSoundData(&fmtChunk, (size_t)dataChunkHeader.cksize);
		}

		//------------------------------------------------------------------------------
		void WavSoundProvider::_initializeProviderFromFile(FILE *source)
		{
			WaveFormat::WaveFileHeader waveHeader;
			if (fread(&waveHeader, sizeof(WaveFormat::WaveFileHeader), 1, source) != 1)
			{
				return;
			}
			if (!waveHeader.IsValidWaveFileHeader())
			{
				return;
			}

			// Skip Until Fmt Chunk
			WaveFormat::WaveChunkHeader checkingChunkHeader;
			if (fread(&checkingChunkHeader, sizeof(WaveFormat::WaveChunkHeader), 1, source) != 1)
			{
				return;
			}
			while (checkingChunkHeader.IsSkippingChunkHeader())
			{
				if (fseek(source, checkingChunkHeader.cksize, SEEK_CUR) != 0)
				{
					return;
				}
				if (fread(&checkingChunkHeader, sizeof(WaveFormat::WaveChunkHeader), 1, source) != 1)
				{
					return;
				}
			}

			if (!checkingChunkHeader.IsValidFmtChunkHeader())
			{
				return;
			}
			WaveFormat::WaveFileFmtChunk fmtChunk;
			fmtChunk.header = checkingChunkHeader;
			if (fread(&fmtChunk.wFormatTag, sizeof(WaveFormat::WaveFileFmtChunk) - ((unsigned char *)&fmtChunk.wFormatTag - (unsigned char *)&fmtChunk), 1, source) != 1)
			{
				return;
			}
			if (fmtChunk.wFormatTag != WAVE_FORMAT_PCM)
			{
				// Only PCM format wave file is supported now
				return;
			}
			if (fmtChunk.header.cksize != sizeof(WaveFormat::WaveFileFmtChunk) - sizeof(WaveFormat::WaveChunkHeader))
			{
				// Unsupported format chunk
				return;
			}

			// Skip Until Data Chunk
			WaveFormat::WaveChunkHeader dataChunkHeader;
			if (fread(&dataChunkHeader, sizeof(WaveFormat::WaveChunkHeader), 1, source) != 1)
			{
				return;
			}
			while (dataChunkHeader.IsSkippingChunkHeader())
			{
				if (fseek(source, dataChunkHeader.cksize, SEEK_CUR) != 0)
				{
					return;
				}
				if (fread(&dataChunkHeader, sizeof(WaveFormat::WaveChunkHeader), 1, source) != 1)
				{
					return;
				}
			}
			if (!dataChunkHeader.IsValidDataChunkHeader())
			{
				return;
			}

			long int filePosition = ftell(source);
			if (filePosition == -1L)
			{
				// File Error
				return;
			}

			// Set Data Source
			m_dataSourceType = DataSourceType::kFile;
			m_dataStartOffset = (size_t)filePosition;

			_InitializeSoundData(&fmtChunk, (size_t)dataChunkHeader.cksize);
		}

		//------------------------------------------------------------------------------
		void WavSoundProvider::_InitializeSoundData(const WaveFormat::WaveFileFmtChunk *format, size_t dataChunkSize)
		{
			if (format->wBitsPerSample != 8 && format->wBitsPerSample != 16)
			{
				// Only 8-bit or 16-bit PCM samples are supported
				return;
			}
			if (format->nChannels < 1 || format->nChannels > 2)
			{
				// Channel Mixing is NOT supported
				return;
			}

			m_frequency = static_cast<SamplingFrequencyType>(format->nSamplesPerSec);
			m_channelNumber = (int)format->nChannels;

			if (format->wBitsPerSample == 8)
			{
				if (format->nChannels == 1)
				{
					m_format = SampleFormat::kMono_8;
				}
				else
				{
					m_format = SampleFormat::kStereo_8;
				}
			}
			else
			{
				if (format->nChannels == 1)
				{
					m_format = SampleFormat::kMono_16;
				}
				else
				{
					m_format = SampleFormat::kStereo_16;
				}
			}

			m_lengthInSample = dataChunkSize / GetSampleByteSize(m_format);
			m_lengthInSecond = (double)m_lengthInSample / (double)m_frequency;

			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		WavSoundProvider::~WavSoundProvider()
		{
			if (m_isInitialized)
			{
				m_isInitialized = false;
			}

			if (m_isOwnFileSource)
			{
				fclose(m_fileSource);
				m_isOwnFileSource = false;
			}
			m_fileSource = nullptr;
		}

		//------------------------------------------------------------------------------
		size_t WavSoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			if (!m_isInitialized)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			size_t sampleSize = GetSampleByteSize(GetFormat());
			SampleNumberType sampleCount = static_cast<SampleNumberType>(bufferSize / sampleSize);

			SampleNumberType endSample = startSample + sampleCount;
			size_t maxSampleCount = GetSampleLength();
			if (endSample > maxSampleCount)
			{
				endSample = maxSampleCount;
			}

			if (startSample >= endSample)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			if (m_dataSourceType == DataSourceType::kMemory)
			{
				return _FillSampleData_MemoryData(dataBuffer, bufferSize, sampleSize, startSample, endSample, outWrittenSamples);
			}
			else
			{
				int lockResult = pthread_mutex_lock(&m_sourceFileMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_sourceFileMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					switch (m_dataSourceType)
					{
						case DataSourceType::kStream:
							return _FillSampleData_Stream(dataBuffer, bufferSize, sampleSize, startSample, endSample, outWrittenSamples, m_iStreamPtr);

						case DataSourceType::kStreamSharedPtr:
							return _FillSampleData_Stream(dataBuffer, bufferSize, sampleSize, startSample, endSample, outWrittenSamples, m_sharedIStreamPtr.get());

						case DataSourceType::kFile:
							return _FillSampleData_File(dataBuffer, bufferSize, sampleSize, startSample, endSample, outWrittenSamples);
					}
				}
				else
				{
					// Lock Failed - Don't do anytning
				}
			}

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = 0;
			}
			return 0;
		}

		//------------------------------------------------------------------------------
		size_t WavSoundProvider::_FillSampleData_Stream(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples, std::istream *targetStream)
		{
			if (targetStream->eof())
			{
				targetStream->clear();
			}

			std::streamoff byteOffset = static_cast<std::streamoff>(m_dataStartOffset + startSample * sampleSize);
			targetStream->seekg(byteOffset, std::ios_base::beg);

			if (targetStream->fail() || targetStream->bad() || targetStream->eof())
			{
				// Stream Seek Failed
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			SampleNumberType writtenSampleCount = endSample - startSample;
			targetStream->read((char *)dataBuffer, static_cast<std::streamsize>(sampleSize * writtenSampleCount));
			size_t readBytes = static_cast<size_t>(targetStream->gcount());
			size_t overBytes = readBytes % sampleSize;
			if (overBytes != 0)
			{
				readBytes -= overBytes;
			}
			writtenSampleCount = readBytes / sampleSize;

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = writtenSampleCount;
			}
			return readBytes;
		}

		//------------------------------------------------------------------------------
		size_t WavSoundProvider::_FillSampleData_File(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples)
		{
			long byteOffset = static_cast<long>(m_dataStartOffset + startSample * sampleSize);
			if (fseek(m_fileSource, byteOffset, SEEK_SET) != 0)
			{
				// File Seek Failed
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			SampleNumberType writtenSampleCount = endSample - startSample;
			size_t readSamples = fread(dataBuffer, sampleSize, static_cast<size_t>(writtenSampleCount), m_fileSource);
			if (readSamples < writtenSampleCount)
			{
				writtenSampleCount = readSamples;
			}

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = writtenSampleCount;
			}
			return static_cast<size_t>(writtenSampleCount * sampleSize);
		}

		//------------------------------------------------------------------------------
		size_t WavSoundProvider::_FillSampleData_MemoryData(void *dataBuffer, size_t bufferSize, size_t sampleSize, SampleNumberType startSample, SampleNumberType endSample, SampleNumberType *outWrittenSamples)
		{
			SampleNumberType writtenSampleCount = endSample - startSample;
			memcpy(dataBuffer, (uint8_t *)m_memoryData + m_dataStartOffset + startSample * sampleSize, static_cast<size_t>(writtenSampleCount) * sampleSize);

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = writtenSampleCount;
			}
			return static_cast<size_t>(writtenSampleCount * sampleSize);
		}
	}
}
