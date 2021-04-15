////////////////////////////////////////////////////////////////////////////////
// Hash/HashContext.h (Leggiero/Modules - Crypto)
//
// Cryptographic hashing context interface for consecutive data
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__HASH__HASH_CONTEXT_H
#define __LM_CRYPTO__HASH__HASH_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <string>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			// Interface of context hashing consecutive data
			class IHashContext
			{
			public:
				virtual ~IHashContext() { }

			public:
				virtual size_t GetHashLength() const = 0;

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) = 0;
				virtual void Append(const std::string &dataToAppend) = 0;

				virtual std::string GetHash() = 0;
				virtual std::string GetHashString(bool isCapital = false) = 0;

				virtual void Clear() = 0;
			};
		}
	}
}

#endif
