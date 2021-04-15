////////////////////////////////////////////////////////////////////////////////
// Provider/OggSoundProvider.cpp (Leggiero/Modules - Sound)
//
// Ogg Vorbis sound provider implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OggSoundProvider.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace _Internal
		{
			// Memory Buffer Structure
			struct OGGRawMemoryBuffer
			{
				const void *buffer;
				size_t size;
				size_t position;
			};


			namespace OggVorbisImpl
			{
				//////////////////////////////////////////////////////////////////////////////// callback functions for istream

				//------------------------------------------------------------------------------
				size_t ov_callback_istream_fread(void *ptr, size_t size, size_t count, void *source)
				{
					std::istream *sourceStream = (std::istream *)source;
					if (sourceStream->eof())
					{
						return 0;
					}

					sourceStream->read((char *)ptr, (std::streamsize)(size * count));
					size_t readBytes = (size_t)(sourceStream->gcount());
					size_t overBytes = readBytes % size;
					if (overBytes != 0)
					{
						readBytes -= overBytes;
						sourceStream->seekg((std::streamoff)(-(ogg_int64_t)overBytes), std::ios_base::cur);
					}

					return (readBytes / size);
				}

				//------------------------------------------------------------------------------
				int ov_callback_istream_fseek(void *source, ogg_int64_t offset, int origin)
				{
					std::istream *sourceStream = (std::istream *)source;

					if (sourceStream->eof())
					{
						sourceStream->clear();
					}

					switch (origin)
					{
						case SEEK_SET:
							{
								sourceStream->seekg((std::streamoff)offset, std::ios_base::beg);
							}
							break;

						case SEEK_CUR:
							{
								sourceStream->seekg((std::streamoff)offset, std::ios_base::cur);
							}
							break;

						case SEEK_END:
							{
								sourceStream->seekg((std::streamoff)offset, std::ios_base::end);
							}
							break;
					}

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
				long int ov_callback_istream_ftell(void *source)
				{
					std::istream *sourceStream = (std::istream *)source;
					return (long int)(sourceStream->tellg());
				}

				//------------------------------------------------------------------------------
				size_t ov_callback_istream_sharedptr_fread(void *ptr, size_t size, size_t count, void *source)
				{
					std::shared_ptr<std::istream> *sourceStreamPtr = (std::shared_ptr<std::istream> *)source;
					return ov_callback_istream_fread(ptr, size, count, sourceStreamPtr->get());
				}

				//------------------------------------------------------------------------------
				int ov_callback_istream_sharedptr_fseek(void *source, ogg_int64_t offset, int origin)
				{
					std::shared_ptr<std::istream> *sourceStreamPtr = (std::shared_ptr<std::istream> *)source;
					return ov_callback_istream_fseek(sourceStreamPtr->get(), offset, origin);
				}

				//------------------------------------------------------------------------------
				long int ov_callback_istream_sharedptr_ftell(void *source)
				{
					std::shared_ptr<std::istream> *sourceStreamPtr = (std::shared_ptr<std::istream> *)source;
					return ov_callback_istream_ftell(sourceStreamPtr->get());
				}


				//////////////////////////////////////////////////////////////////////////////// callback functions for memory buffer

				//------------------------------------------------------------------------------
				size_t ov_callback_membuf_fread(void *ptr, size_t size, size_t count, void *source)
				{
					_Internal::OGGRawMemoryBuffer *sourceBuffer = (_Internal::OGGRawMemoryBuffer *)source;

					size_t toRead = size * count;
					size_t effectiveCount = count;
					size_t remain = sourceBuffer->size - sourceBuffer->position;
					if (remain < toRead)
					{
						effectiveCount = remain / size;
						toRead = size * effectiveCount;
					}
					if (toRead == 0)
					{
						return 0;
					}

					memcpy(ptr, (const char *)sourceBuffer->buffer + sourceBuffer->position, toRead);
					sourceBuffer->position += toRead;
					return effectiveCount;
				}

				//------------------------------------------------------------------------------
				int ov_callback_membuf_fseek(void *source, ogg_int64_t offset, int origin)
				{
					_Internal::OGGRawMemoryBuffer *sourceBuffer = (_Internal::OGGRawMemoryBuffer *)source;

					switch (origin)
					{
						case SEEK_SET:
							{
								if (sourceBuffer->size < (size_t)offset)
								{
									return -1;
								}
								sourceBuffer->position = (size_t)offset;
							}
							break;

						case SEEK_CUR:
							{
								if (sourceBuffer->size < sourceBuffer->position + (size_t)offset)
								{
									return -1;
								}
								sourceBuffer->position += (size_t)offset;
							}
							break;

						case SEEK_END:
							{
								if (sourceBuffer->size < (size_t)offset)
								{
									return -1;
								}
								sourceBuffer->position = sourceBuffer->size - (size_t)offset;
							}
							break;
					}

					return 0;
				}

				//------------------------------------------------------------------------------
				long int ov_callback_membuf_ftell(void *source)
				{
					_Internal::OGGRawMemoryBuffer *sourceBuffer = (_Internal::OGGRawMemoryBuffer *)source;
					return (long int)(sourceBuffer->position);
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////////// OggSoundProvider

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(FILE *source, bool needToClose)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(needToClose ? source : nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastBitstream(0)
			, m_lastReadSamplePosition(0)
		{
			if (needToClose)
			{
				if (ov_open_callbacks(source, &m_sourceVorbisFile, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
				{
					// OggVorbis File Error
					fclose(source);
					return;
				}
			}
			else
			{
				if (ov_open_callbacks(source, &m_sourceVorbisFile, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
				{
					// OggVorbis File Error
					return;
				}
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(const char *filePath)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastBitstream(0)
			, m_lastReadSamplePosition(0)
		{
			m_ownFileSource = fopen(filePath, "rb");
			if (m_ownFileSource == nullptr)
			{
				// Cannot open a file
				return;
			}

			if (ov_open_callbacks(m_ownFileSource, &m_sourceVorbisFile, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
			{
				// OggVorbis File Error
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(const std::string &filePath)
			: OggSoundProvider(filePath.c_str())
		{
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(std::istream *source)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastBitstream(0)
			, m_lastReadSamplePosition(0)
		{
			static ov_callbacks LEGGIERO_OV_CALLBACKS_CPP_ISTREAM_NOCLOSE = {
				(size_t(*)(void *, size_t, size_t, void *))  _Internal::OggVorbisImpl::ov_callback_istream_fread,
				(int(*)(void *, ogg_int64_t, int))           _Internal::OggVorbisImpl::ov_callback_istream_fseek,
				(int(*)(void *))                             NULL,
				(long int(*)(void *))                        _Internal::OggVorbisImpl::ov_callback_istream_ftell
			};

			if (ov_open_callbacks(source, &m_sourceVorbisFile, NULL, 0, LEGGIERO_OV_CALLBACKS_CPP_ISTREAM_NOCLOSE) < 0)
			{
				// OggVorbis File Error
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(std::shared_ptr<std::istream> source)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(source)
			, m_lastBitstream(0)
			, m_lastReadSamplePosition(0)
		{
			static ov_callbacks LEGGIERO_OV_CALLBACKS_CPP_ISTREAM_SHAREDPTR_NOCLOSE = {
				(size_t(*)(void *, size_t, size_t, void *))  _Internal::OggVorbisImpl::ov_callback_istream_sharedptr_fread,
				(int(*)(void *, ogg_int64_t, int))           _Internal::OggVorbisImpl::ov_callback_istream_sharedptr_fseek,
				(int(*)(void *))                             NULL,
				(long int(*)(void *))                        _Internal::OggVorbisImpl::ov_callback_istream_sharedptr_ftell
			};

			if (ov_open_callbacks(&m_sharedIStreamPtr, &m_sourceVorbisFile, NULL, 0, LEGGIERO_OV_CALLBACKS_CPP_ISTREAM_SHAREDPTR_NOCLOSE) < 0)
			{
				// OggVorbis File Error
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::OggSoundProvider(const void *memoryBuffer, size_t bufferSize)
			: m_format(SampleFormat::kMono_8), m_frequency(44100), m_lengthInSecond(0.0f), m_isInitialized(false)
			, m_ownFileSource(nullptr), m_sharedIStreamPtr(nullptr)
			, m_lastBitstream(0)
			, m_lastReadSamplePosition(0)
			, m_bufferSharedPtr(std::make_shared<_Internal::OGGRawMemoryBuffer>())
		{
			m_bufferSharedPtr->buffer = memoryBuffer;
			m_bufferSharedPtr->size = bufferSize;
			m_bufferSharedPtr->position = 0;

			static ov_callbacks LEGGIERO_OV_CALLBACKS_RAWMEMBUFFER_NOCLOSE = {
				(size_t(*)(void *, size_t, size_t, void *))  _Internal::OggVorbisImpl::ov_callback_membuf_fread,
				(int(*)(void *, ogg_int64_t, int))           _Internal::OggVorbisImpl::ov_callback_membuf_fseek,
				(int(*)(void *))                             NULL,
				(long int(*)(void *))                        _Internal::OggVorbisImpl::ov_callback_membuf_ftell
			};

			if (ov_open_callbacks(m_bufferSharedPtr.get(), &m_sourceVorbisFile, NULL, 0, LEGGIERO_OV_CALLBACKS_RAWMEMBUFFER_NOCLOSE) < 0)
			{
				// OggVorbis File Error
				return;
			}

			_InitializeSoundData();
		}

		//------------------------------------------------------------------------------
		void OggSoundProvider::_InitializeSoundData()
		{
			m_lengthInSample = ov_pcm_total(&m_sourceVorbisFile, -1);
			if (m_lengthInSample == OV_EINVAL)
			{
				// Invalid Ogg File
				ov_clear(&m_sourceVorbisFile);
				return;
			}

			m_lengthInSecond = ov_time_total(&m_sourceVorbisFile, -1);

			vorbis_info *info = ov_info(&m_sourceVorbisFile, -1);
			m_frequency = static_cast<SamplingFrequencyType>(info->rate);
			if (info->channels == 1)
			{
				m_channelNumber = 1;
				m_format = SampleFormat::kMono_16;
			}
			else if (info->channels == 2)
			{
				m_channelNumber = 2;
				m_format = SampleFormat::kStereo_16;
			}
			else
			{
				// Channel Mixing is NOT supported
				ov_clear(&m_sourceVorbisFile);
				return;
			}

			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		OggSoundProvider::~OggSoundProvider()
		{
			if (m_isInitialized)
			{
				m_isInitialized = false;
				ov_clear(&m_sourceVorbisFile);
			}

			if (m_ownFileSource != nullptr)
			{
				fclose(m_ownFileSource);
				m_ownFileSource = nullptr;
			}
		}

		//------------------------------------------------------------------------------
		size_t OggSoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
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

			int lockResult = pthread_mutex_lock(&m_sourceFileMutex.GetLock());
			if (lockResult == 0)
			{
				pthread_mutex_t *lockCopy = &m_sourceFileMutex.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				if (m_lastReadSamplePosition != startSample)
				{
					if (ov_pcm_seek(&m_sourceVorbisFile, (ogg_int64_t)startSample) != 0)
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
					long readResult = ov_read(&m_sourceVorbisFile, (char *)dataBuffer + bufferReadBytes, (int)(bufferSize - bufferReadBytes), 0, 2, 1, &m_lastBitstream);
					if (readResult > 0)
					{
						bufferReadBytes += (size_t)readResult;
					}
					else if (readResult == 0)
					{
						break;
					}
					else
					{
						// OggVorbis File Error
						m_isInitialized = false;
						ov_clear(&m_sourceVorbisFile);

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
