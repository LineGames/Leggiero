////////////////////////////////////////////////////////////////////////////////
// Hash/HashUtility.h (Leggiero/Modules - Crypto)
//
// Cryptographic hashing utility class interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__HASH__HASH_UTILITY_H
#define __LM_CRYPTO__HASH__HASH_UTILITY_H


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
			// Interface of hash utility class
			class IHashUtility
			{
			public:
				virtual ~IHashUtility() { }

			public:
				// Get digest byte size
				virtual size_t GetHashSizeOf() const = 0;

				// Calculate binary hash digest of given data
				virtual std::string CalculateHashOf(const void *dataBufferToHash, size_t dataLength) = 0;
				virtual std::string CalculateHashOf(const std::string &dataToHash) = 0;

				// Get hash hex string
				virtual std::string GetHashStringOf(const void *dataBufferToHash, size_t dataLength, bool isCapital = false) = 0;
				virtual std::string GetHashStringOf(const std::string &dataToHash, bool isCapital = false) = 0;
				virtual std::string GetHashStringOf(const char *dataCStringToHash, bool isCapital = false) = 0;

				// Calculate hash checksum of a given file
				virtual std::string CalculateFileHashOf(const std::string &filePath) = 0;
			};
		}
	}
}

#endif
