////////////////////////////////////////////////////////////////////////////////
// Hash/CRC32Util.h (Leggiero/Modules - Crypto)
//
// CRC32 Checksum Utility
// Actually, CRC is not a cryptographic hash.
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__HASH__CRC32_UTIL_H
#define __LM_CRYPTO__HASH__CRC32_UTIL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

// Leggiero.Crypto
#include "HashUtility.h"
#include "HashContext.h"


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			// CRC32 Checksum Utility
			class CRC32Checksum
				: public IHashUtility
			{
			public:
				// Calculate CRC32 checksum of given data
				static uint32_t CalculateCRC32(const void *dataBufferToHash, size_t dataLength);
				static uint32_t CalculateCRC32(const std::string &dataToHash);

				// Calculate CRC32 checksum of a given file
				static std::tuple<bool, uint32_t> CalculateFileCRC32(const std::string &filePath);

			public:
				// Result Digest Size
				static size_t GetHashSize() { return 4; }

				// Calculate binary CRC32 checksum of given data
				static std::string CalculateHash(const void *dataBufferToHash, size_t dataLength);
				static std::string CalculateHash(const std::string &dataToHash);

				// Get checksum as hex string
				static std::string GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital = false);
				static std::string GetHashString(const std::string &dataToHash, bool isCapital = false);
				static std::string GetHashString(const char *dataCStringToHash, bool isCapital = false);

				// Calculate CRC32 checksum of a given file
				static std::string CalculateFileHash(const std::string &filePath);

			public:	// IHashUtility
				// Get digest byte size
				virtual size_t GetHashSizeOf() const override { return GetHashSize(); }

				// Calculate binary hash digest of given data
				virtual std::string CalculateHashOf(const void *dataBufferToHash, size_t dataLength) override { return CalculateHash(dataBufferToHash, dataLength); }
				virtual std::string CalculateHashOf(const std::string &dataToHash) override { return CalculateHash(dataToHash); }

				// Get hash hex string
				virtual std::string GetHashStringOf(const void *dataBufferToHash, size_t dataLength, bool isCapital = false) override { return GetHashString(dataBufferToHash, dataLength, isCapital); }
				virtual std::string GetHashStringOf(const std::string &dataToHash, bool isCapital = false) override { return GetHashString(dataToHash, isCapital); }
				virtual std::string GetHashStringOf(const char *dataCStringToHash, bool isCapital = false) override { return GetHashString(dataCStringToHash, isCapital); }

				// Calculate hash checksum of a given file
				virtual std::string CalculateFileHashOf(const std::string &filePath) override { return CalculateFileHash(filePath); }
			};


			// Forward Declaration
			namespace _Internal
			{
				class ConcreteCRC32Context;
			}

			// CRC32 Summing Context for Consecutive Data
			class CRC32Context
				: public IHashContext
			{
			public:
				CRC32Context();
				CRC32Context(const void *initialDataBuffer, size_t dataLength);
				CRC32Context(const std::string &initialData);
				virtual ~CRC32Context() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 4; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			public:
				virtual uint32_t GetChecksum();

			protected:
				std::shared_ptr<_Internal::ConcreteCRC32Context> m_concreteObject;
			};
		}
	}
}

#endif
