////////////////////////////////////////////////////////////////////////////////
// ProtectedKey/IProtectedKey.h (Leggiero/Modules - Crypto)
//
// In-memory protected key interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__PROTECTED_KEY__I_PROTECTED_KEY_H
#define __LM_CRYPTO__PROTECTED_KEY__I_PROTECTED_KEY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <cstddef>
#include <cstdint>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Security
		{
			// Interface of protected key
			class IProtectedKey
			{
			public:
				virtual ~IProtectedKey() { }

			public:
				// Get length of the key
				virtual size_t GetLength() const = 0;

				// Get key byte value at given offset
				virtual uint8_t GetAt(size_t idx) const = 0;

				// Copy key to given buffer
				virtual void FillKey(void *buffer, size_t length, size_t keyOffset = 0) const = 0;

				// Do XOR on given data using the key
				virtual void XORKey(void *buffer, size_t length, size_t keyOffset = 0) const = 0;
			};
		}
	}
}

#endif
