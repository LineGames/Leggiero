////////////////////////////////////////////////////////////////////////////////
// Data/BufferReader.cpp (Leggiero - Utility)
//
// Buffer Reader Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "BufferReader.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Data
		{
			//////////////////////////////////////////////////////////////////////////////// BufferReader

			//------------------------------------------------------------------------------
			BufferReader::BufferReader(const void *buffer, size_t initialSize)
				: m_buffer(buffer), m_size(initialSize), m_currentOffset(0)
			{

			}

			//------------------------------------------------------------------------------
			BufferReader::~BufferReader()
			{

			}

			//------------------------------------------------------------------------------
			std::string BufferReader::PeekRaw(size_t size, bool isSafe)
			{
				if (size == 0)
				{
					return std::string("");
				}

				if (isSafe)
				{
					if (m_currentOffset + size > m_size)
					{
						return std::string("");
					}
				}

				return std::string((const char *)m_buffer + m_currentOffset, size);
			}

			//------------------------------------------------------------------------------
			std::string BufferReader::ReadRaw(size_t size, bool isSafe)
			{
				if (size == 0)
				{
					return std::string("");
				}

				if (isSafe)
				{
					if (m_currentOffset + size > m_size)
					{
						return std::string("");
					}
				}

				size_t oldOffset = m_currentOffset;
				m_currentOffset += size;

				return std::string((const char *)m_buffer + oldOffset, size);
			}

			//------------------------------------------------------------------------------
			void BufferReader::PeekRawToBuffer(void *buffer, size_t size, bool isSafe)
			{
				if (size == 0)
				{
					return;
				}

				if (isSafe)
				{
					if (m_currentOffset + size > m_size)
					{
						return;
					}
				}

				memcpy(buffer, (const char *)m_buffer + m_currentOffset, size);
			}

			//------------------------------------------------------------------------------
			void BufferReader::ReadRawToBuffer(void *buffer, size_t size, bool isSafe)
			{
				if (size == 0)
				{
					return;
				}

				if (isSafe)
				{
					if (m_currentOffset + size > m_size)
					{
						return;
					}
				}

				memcpy(buffer, (const char *)m_buffer + m_currentOffset, size);
				m_currentOffset += size;
			}

			//------------------------------------------------------------------------------
			std::string BufferReader::ReadLengthedString()
			{
				size_t effectiveSize = 0;

				uint8_t size1 = Read<uint8_t>();
				if (size1 == 0xff)
				{
					uint16_t size2 = Read<uint16_t>();
					if (size2 == 0xffff)
					{
						uint32_t size3 = Read<uint32_t>();
						effectiveSize = (size_t)size3;
					}
					else
					{
						effectiveSize = (size_t)size2;
					}
				}
				else
				{
					effectiveSize = (size_t)size1;
				}

				size_t oldOffset = m_currentOffset;
				m_currentOffset += effectiveSize;

				return std::string((const char *)m_buffer + oldOffset, effectiveSize);
			}

			//------------------------------------------------------------------------------
			void BufferReader::SkipLengthedString()
			{
				size_t effectiveSize = 0;

				uint8_t size1 = Read<uint8_t>();
				if (size1 == 0xff)
				{
					uint16_t size2 = Read<uint16_t>();
					if (size2 == 0xffff)
					{
						uint32_t size3 = Read<uint32_t>();
						effectiveSize = (size_t)size3;
					}
					else
					{
						effectiveSize = (size_t)size2;
					}
				}
				else
				{
					effectiveSize = (size_t)size1;
				}

				m_currentOffset += effectiveSize;
			}
		}
	}
}
