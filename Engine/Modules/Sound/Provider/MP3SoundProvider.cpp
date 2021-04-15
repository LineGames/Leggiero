////////////////////////////////////////////////////////////////////////////////
// Provider/MP3SoundProvider.cpp (Leggiero/Modules - Sound)
//
// MP3 Vorbis sound provider implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "MP3SoundProvider.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// IO Functions

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			size_t mp3d_callback_file_fread(void *buf, size_t size, void *user_data)
			{
				FILE *sourceFile = (FILE *)user_data;
				return fread(buf, 1, size, sourceFile);
			}

			//------------------------------------------------------------------------------
			int mp3d_callback_file_fseek(uint64_t position, void *user_data)
			{
				FILE *sourceFile = (FILE *)user_data;
				return fseek(sourceFile, static_cast<long>(position), SEEK_SET);
			}

			//------------------------------------------------------------------------------
			size_t mp3d_callback_istream_fread(void *buf, size_t size, void *user_data)
			{
				std::istream *sourceStream = (std::istream *)user_data;
				if (sourceStream->eof())
				{
					return 0;
				}

				sourceStream->read((char *)buf, (std::streamsize)size);
				size_t readBytes = (size_t)(sourceStream->gcount());

				return readBytes;
			}

			//------------------------------------------------------------------------------
			int mp3d_callback_istream_fseek(uint64_t position, void *user_data)
			{
				std::istream *sourceStream = (std::istream *)user_data;

				if (sourceStream->eof())
				{
					sourceStream->clear();
				}

				sourceStream->seekg((std::streamoff)position, std::ios_base::beg);

				if (sourceStream->fail())
				{
					return -1;
				}
				else if (sourceStream->bad())
				{
					return -2;
				}
				return 0;
			}

			//------------------------------------------------------------------------------
			size_t mp3d_callback_istream_sharedptr_fread(void *buf, size_t size, void *user_data)
			{
				std::shared_ptr<std::istream> *sourceStreamPtr = (std::shared_ptr<std::istream> *)user_data;
				return mp3d_callback_istream_fread(buf, size, sourceStreamPtr->get());
			}

			//------------------------------------------------------------------------------
			int mp3d_callback_istream_sharedptr_fseek(uint64_t position, void *user_data)
			{
				std::shared_ptr<std::istream> *sourceStreamPtr = (std::shared_ptr<std::istream> *)user_data;
				return mp3d_callback_istream_fseek(position, sourceStreamPtr->get());
			}
		}


		//////////////////////////////////////////////////////////////////////////////// MP3SoundProvider

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(FILE *source, bool needToClose)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(needToClose ? source : nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastReadSamplePosition(0)
		{
			m_mp3IO.read = (MP3D_READ_CB)_Internal::mp3d_callback_istream_fread;
			m_mp3IO.read_data = source;
			m_mp3IO.seek = (MP3D_SEEK_CB)_Internal::mp3d_callback_istream_fseek;
			m_mp3IO.seek_data = source;

			m_mp3Object = new mp3dec_ex_t();
			if (mp3dec_ex_open_cb(m_mp3Object, &m_mp3IO, MP3D_SEEK_TO_SAMPLE))
			{
				if (needToClose)
				{
					fclose(source);
					m_ownFileSource = nullptr;
				}
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(const char *filePath)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastReadSamplePosition(0)
		{
			m_mp3Object = new mp3dec_ex_t();
			if (mp3dec_ex_open(m_mp3Object, filePath, MP3D_SEEK_TO_SAMPLE))
			{
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(const std::string &filePath)
			: MP3SoundProvider(filePath.c_str())
		{
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(std::istream *source)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastReadSamplePosition(0)
		{
			m_mp3IO.read = (MP3D_READ_CB)_Internal::mp3d_callback_istream_fread;
			m_mp3IO.read_data = source;
			m_mp3IO.seek = (MP3D_SEEK_CB)_Internal::mp3d_callback_istream_fseek;
			m_mp3IO.seek_data = source;

			m_mp3Object = new mp3dec_ex_t();
			if (mp3dec_ex_open_cb(m_mp3Object, &m_mp3IO, MP3D_SEEK_TO_SAMPLE))
			{
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(std::shared_ptr<std::istream> source)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(source)
			, m_lastReadSamplePosition(0)
		{
			m_mp3IO.read = (MP3D_READ_CB)_Internal::mp3d_callback_istream_sharedptr_fread;
			m_mp3IO.read_data = &m_sharedIStreamPtr;
			m_mp3IO.seek = (MP3D_SEEK_CB)_Internal::mp3d_callback_istream_sharedptr_fseek;
			m_mp3IO.seek_data = &m_sharedIStreamPtr;

			m_mp3Object = new mp3dec_ex_t();
			if (mp3dec_ex_open_cb(m_mp3Object, &m_mp3IO, MP3D_SEEK_TO_SAMPLE))
			{
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::MP3SoundProvider(const void *memoryBuffer, size_t bufferSize)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastReadSamplePosition(0)
		{
			m_mp3Object = new mp3dec_ex_t();
			if (mp3dec_ex_open_buf(m_mp3Object, (const uint8_t *)memoryBuffer, bufferSize, MP3D_SEEK_TO_SAMPLE))
			{
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		void MP3SoundProvider::_InitializeSoundData()
		{
			if (m_mp3Object->info.channels == 1)
			{
				m_channelNumber = 1;
				m_format = SampleFormat::kMono_16;
			}
			else if (m_mp3Object->info.channels == 2)
			{
				m_channelNumber = 2;
				m_format = SampleFormat::kStereo_16;
			}
			else
			{
				// Channel Mixing is NOT supported
				mp3dec_ex_close(m_mp3Object);
				return;
			}

			m_lengthInSample = static_cast<SampleNumberType>(m_mp3Object->samples / m_channelNumber);
			if (m_lengthInSample <= 0)
			{
				// Invalid MP3 File
				mp3dec_ex_close(m_mp3Object);
				return;
			}

			m_frequency = static_cast<SamplingFrequencyType>(m_mp3Object->info.hz);
			m_lengthInSecond = static_cast<float>((double)m_lengthInSample / (double)m_frequency);

			m_sampleByteSize = sizeof(mp3d_sample_t);

			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		MP3SoundProvider::~MP3SoundProvider()
		{
			if (m_isInitialized)
			{
				m_isInitialized = false;
				mp3dec_ex_close(m_mp3Object);
			}
			if (m_mp3Object != nullptr)
			{
				delete m_mp3Object;
				m_mp3Object = nullptr;
			}

			if (m_ownFileSource != nullptr)
			{
				fclose(m_ownFileSource);
				m_ownFileSource = nullptr;
			}
		}

		//------------------------------------------------------------------------------
		size_t MP3SoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			if (!m_isInitialized)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			size_t bufferReadBytes = 0;
			size_t minReadSize = bufferSize * 4 / 5;

			size_t toReadSamples = bufferSize / m_sampleByteSize;

			int lockResult = pthread_mutex_lock(&m_sourceFileMutex.GetLock());
			if (lockResult == 0)
			{
				pthread_mutex_t *lockCopy = &m_sourceFileMutex.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				if (m_lastReadSamplePosition != startSample)
				{
					if (mp3dec_ex_seek(m_mp3Object, static_cast<uint64_t>(startSample * m_channelNumber)) != 0)
					{
						// Seek Error
						if (outWrittenSamples != nullptr)
						{
							*outWrittenSamples = 0;
						}
						return 0;
					}
					m_lastReadSamplePosition = startSample;
				}

				do
				{
					size_t readSamples = mp3dec_ex_read(m_mp3Object, (mp3d_sample_t *)((uint8_t *)dataBuffer + bufferReadBytes), toReadSamples);
					if (readSamples >= toReadSamples)
					{
						bufferReadBytes += readSamples * m_sampleByteSize;
						toReadSamples -= readSamples;
					}
					else if (m_mp3Object->last_error == 0)
					{
						// eof
						bufferReadBytes += readSamples * m_sampleByteSize;
						toReadSamples -= readSamples;
						break;
					}
					else
					{
						// MP3 File Error
						m_isInitialized = false;
						mp3dec_ex_close(m_mp3Object);

						break;
					}
				} while (bufferReadBytes < minReadSize);

				SampleNumberType writtenSample = bufferReadBytes / GetSampleByteSize(GetFormat());
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = writtenSample;
				}
				m_lastReadSamplePosition += writtenSample;
				return bufferReadBytes;
			}
			else
			{
				// Lock Failed - Don't do anytning
			}

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = 0;
			}
			return 0;
		}
	}
}
