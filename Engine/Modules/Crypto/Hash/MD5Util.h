////////////////////////////////////////////////////////////////////////////////
// Hash/MD5Util.h (Leggiero/Modules - Crypto)
//
// MD5 Hash Utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__HASH__MD5_UTIL_H
#define __LM_CRYPTO__HASH__MD5_UTIL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <memory>
#include <string>

// Leggiero.Crypto
#include "HashUtility.h"
#include "HashContext.h"


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			// MD5 Hash Utility
			class MD5Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 16; }

				// Calculate binary hash digest of given data
				static std::string CalculateHash(const void *dataBufferToHash, size_t dataLength);
				static std::string CalculateHash(const std::string &dataToHash);

				// Get hash hex string
				static std::string GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital = false);
				static std::string GetHashString(const std::string &dataToHash, bool isCapital = false);
				static std::string GetHashString(const char *dataCStringToHash, bool isCapital = false);

				// Calculate hash checksum of a given file
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
				class ConcreteMD5HashContext;
			}

			// MD5 Hasing Context for Consecutive Data
			class MD5HashContext
				: public IHashContext
			{
			public:
				MD5HashContext();
				MD5HashContext(const void *initialDataBuffer, size_t dataLength);
				MD5HashContext(const std::string &initialData);
				virtual ~MD5HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 16; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteMD5HashContext> m_concreteObject;
			};
		}
	}
}

#endif
