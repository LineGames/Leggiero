////////////////////////////////////////////////////////////////////////////////
// Hash/SHAUtil.h (Leggiero/Modules - Crypto)
//
// SHA Family Hash Utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__HASH__SHA_UTIL_H
#define __LM_CRYPTO__HASH__SHA_UTIL_H


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
			// Forward Declarations
			namespace _Internal
			{
				class ConcreteSHA1HashContext;
				class ConcreteSHA224HashContext;
				class ConcreteSHA256HashContext;
				class ConcreteSHA384HashContext;
				class ConcreteSHA512HashContext;
			}


			// SHA-1 Hash Utility
			class SHA1Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 20; }

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

			// SHA-1 Hasing Context for Consecutive Data
			class SHA1HashContext : public IHashContext
			{
			public:
				SHA1HashContext();
				SHA1HashContext(const void *initialDataBuffer, size_t dataLength);
				SHA1HashContext(const std::string &initialData);
				virtual ~SHA1HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 40; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteSHA1HashContext> m_concreteObject;
			};


			// SHA1-224 Hash Utility
			class SHA224Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 28; }

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

			// SHA-224 Hasing Context for Consecutive Data
			class SHA224HashContext : public IHashContext
			{
			public:
				SHA224HashContext();
				SHA224HashContext(const void *initialDataBuffer, size_t dataLength);
				SHA224HashContext(const std::string &initialData);
				virtual ~SHA224HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 28; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteSHA224HashContext> m_concreteObject;
			};


			// SHA-256 Hash Utility
			class SHA256Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 32; }

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

			// SHA-256 Hasing Context for Consecutive Data
			class SHA256HashContext : public IHashContext
			{
			public:
				SHA256HashContext();
				SHA256HashContext(const void *initialDataBuffer, size_t dataLength);
				SHA256HashContext(const std::string &initialData);
				virtual ~SHA256HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 32; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteSHA256HashContext> m_concreteObject;
			};


			// SHA-384 Hash Utility
			class SHA384Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 48; }

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

			// SHA-384 Hasing Context for Consecutive Data
			class SHA384HashContext : public IHashContext
			{
			public:
				SHA384HashContext();
				SHA384HashContext(const void *initialDataBuffer, size_t dataLength);
				SHA384HashContext(const std::string &initialData);
				virtual ~SHA384HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 48; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteSHA384HashContext> m_concreteObject;
			};


			// SHA-512 Hash Utility
			class SHA512Hash
				: public IHashUtility
			{
			public:
				// Result Digest Size
				static size_t GetHashSize() { return 64; }

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

			// SHA-512 Hasing Context for Consecutive Data
			class SHA512HashContext : public IHashContext
			{
			public:
				SHA512HashContext();
				SHA512HashContext(const void *initialDataBuffer, size_t dataLength);
				SHA512HashContext(const std::string &initialData);
				virtual ~SHA512HashContext() { }

			public:	// IHashContext
				virtual size_t GetHashLength() const override { return 64; }

				virtual void Append(const void *dataBufferToAppend, size_t dataLength) override;
				virtual void Append(const std::string &dataToAppend) override;

				virtual std::string GetHash() override;
				virtual std::string GetHashString(bool isCapital = false) override;

				virtual void Clear() override;

			protected:
				std::shared_ptr<_Internal::ConcreteSHA512HashContext> m_concreteObject;
			};
		}
	}
}

#endif
