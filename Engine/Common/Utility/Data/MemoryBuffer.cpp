////////////////////////////////////////////////////////////////////////////////
// Data/MemoryBuffer.cpp (Leggiero - Utility)
//
// Memory Buffer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "MemoryBuffer.h"

// Standard Library
#include <cstring>


namespace Leggiero
{
	namespace Utility
	{
		namespace Data
		{
			//////////////////////////////////////////////////////////////////////////////// MemoryBuffer

			//------------------------------------------------------------------------------
			MemoryBuffer::MemoryBuffer(size_t initialSize)
				: m_buffer(nullptr), m_currentBufferSize(initialSize)
			{
				if (initialSize > 0)
				{
					m_buffer = (void *)malloc(initialSize);
				}
			}

			//------------------------------------------------------------------------------
			MemoryBuffer::~MemoryBuffer()
			{
				if (m_buffer != nullptr)
				{
					free(m_buffer);
				}
				m_buffer = nullptr;
			}

			//------------------------------------------------------------------------------
			void MemoryBuffer::ReserveBufferToSize(size_t needSize)
			{
				if (needSize == 0)
				{
					return;
				}

				if (needSize > m_currentBufferSize)
				{
					if (m_buffer != nullptr)
					{
						free(m_buffer);
					}
					m_buffer = (void *)malloc(needSize);
					m_currentBufferSize = needSize;
				}
			}
		}
	}
}
