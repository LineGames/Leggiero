////////////////////////////////////////////////////////////////////////////////
// Data/BufferReader.h (Leggiero - Utility)
//
// Stateful Reader Class for in-memory data buffer
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__DATA__BUFFER_READER_H
#define __UTILITY__DATA__BUFFER_READER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdlib>
#include <limits>
#include <string>


namespace Leggiero
{
	namespace Utility
	{
		namespace Data
		{
			// Data Reader form Buffer
			class BufferReader
			{
			public:
				BufferReader(const void *buffer, size_t initialSize = std::numeric_limits<size_t>::max());
				virtual ~BufferReader();

			public:
				size_t GetSize() const { return m_size; }
				bool IsBufferSizeLimited() const { return (m_size < std::numeric_limits<size_t>::max()); }

				const void *GetBuffer() const { return m_buffer; }
				const void *GetBufferReadingPosition() const { return ((const char *)m_buffer + m_currentOffset); }

				void Rewind() { m_currentOffset = 0; }
				bool Offset(int offset) { m_currentOffset += offset; if (m_currentOffset > m_size) { m_currentOffset = m_size; return false; } return true; }

				bool IsFinished() const { return (m_currentOffset < m_size); }
				size_t GetRemainSize() const { return (m_currentOffset <= m_size) ? (m_size - m_currentOffset) : 0; }

			public:
				template <typename T>
				T Peek() const { return *(const T *)((const char *)m_buffer + m_currentOffset); }

				template <typename T>
				T Read() { T value = *(const T *)((const char *)m_buffer + m_currentOffset); Offset(sizeof(T)); return value; }

				template <typename T>
				bool CanRead() const { return ((m_currentOffset + sizeof(T)) <= m_size); }

				template <typename T>
				bool UnRead() { if (m_currentOffset < sizeof(T)) return false; m_currentOffset -= sizeof(T); return true; }

				template <typename T>
				bool PeekToBuffer(void *buffer) const { if (m_currentOffset + sizeof(T) > m_size) { return false; } *((T *)buffer) = *(const T *)((const char *)m_buffer + m_currentOffset); return true; }

				template <typename T>
				bool ReadToBuffer(void *buffer) { if (m_currentOffset + sizeof(T) > m_size) { return false; } *((T *)buffer) = *(const T *)((const char *)m_buffer + m_currentOffset); m_currentOffset += sizeof(T); return true; }

				template <typename T>
				bool Skip(size_t count = 1) { return Offset((int)(sizeof(T) * count)); }

			public:
				std::string PeekRaw(size_t size, bool isSafe = false);
				std::string ReadRaw(size_t size, bool isSafe = false);

				void PeekRawToBuffer(void *buffer, size_t size, bool isSafe = false);
				void ReadRawToBuffer(void *buffer, size_t size, bool isSafe = false);

			public:	// With custom length format
				std::string ReadLengthedString();
				void SkipLengthedString();

			protected:
				const void *m_buffer;
				size_t		m_size;

				size_t		m_currentOffset;
			};
		}
	}
}

#endif
