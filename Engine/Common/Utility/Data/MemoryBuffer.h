////////////////////////////////////////////////////////////////////////////////
// Data/MemoryBuffer.h (Leggiero - Utility)
//
// In-Memory Buffer to hold data
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__DATA__MEMORY_BUFFER_H
#define __UTILITY__DATA__MEMORY_BUFFER_H


// Standard Library
#include <cstdlib>


namespace Leggiero
{
	namespace Utility
	{
		namespace Data
		{
			// Memory Buffer
			class MemoryBuffer
			{
			public:
				MemoryBuffer(size_t initialSize = 0);
				virtual ~MemoryBuffer();

			public:
				size_t GetCurrentSize() const { return m_currentBufferSize; }
				void *GetBuffer() { return m_buffer; }

				void ReserveBufferToSize(size_t needSize);

			protected:
				void *m_buffer;
				size_t	m_currentBufferSize;
			};
		}
	}
}

#endif
