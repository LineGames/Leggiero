////////////////////////////////////////////////////////////////////////////////
// Hash/SHAUtil_iOS.mm (Leggiero/Modules - Crypto)
//
// CommonCrypto based iOS SHA family hash utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SHAUtil.h"

// Standard Library
#include <cstdio>

// External Library
#import <CommonCrypto/CommonDigest.h>

// Leggiero.Utility
#include <Utility/Encoding/HexString.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			//////////////////////////////////////////////////////////////////////////////// SHA-1 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA1Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[CC_SHA1_DIGEST_LENGTH];
				CC_SHA1((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)CC_SHA1_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA1Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[CC_SHA1_DIGEST_LENGTH];
				CC_SHA1((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)CC_SHA1_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA1Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA1Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA1Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate hash checksum of a given file
			std::string SHA1Hash::CalculateFileHash(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::string("");
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;
				CC_SHA1_CTX hashContext;

				CC_SHA1_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					CC_SHA1_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[CC_SHA1_DIGEST_LENGTH];
				CC_SHA1_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)CC_SHA1_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-1 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteSHA1HashContext
				{
				public:
					ConcreteSHA1HashContext()
					{
						CC_SHA1_Init(&m_context);
					}

					virtual ~ConcreteSHA1HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						CC_SHA1_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[CC_SHA1_DIGEST_LENGTH];
						CC_SHA1_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)CC_SHA1_DIGEST_LENGTH);
					}

					void Clear()
					{
						CC_SHA1_Init(&m_context);
					}

				protected:
					CC_SHA1_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			SHA1HashContext::SHA1HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA1HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			SHA1HashContext::SHA1HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA1HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			SHA1HashContext::SHA1HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA1HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void SHA1HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void SHA1HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string SHA1HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string SHA1HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void SHA1HashContext::Clear()
			{
				m_concreteObject->Clear();
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-224 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA224Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[CC_SHA224_DIGEST_LENGTH];
				CC_SHA224((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)CC_SHA224_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA224Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[CC_SHA224_DIGEST_LENGTH];
				CC_SHA224((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)CC_SHA224_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA224Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA224_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA224Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA224_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA224Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA224_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate hash checksum of a given file
			std::string SHA224Hash::CalculateFileHash(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::string("");
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;
				CC_SHA256_CTX hashContext;

				CC_SHA224_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					CC_SHA224_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[CC_SHA224_DIGEST_LENGTH];
				CC_SHA224_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)CC_SHA224_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-224 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteSHA224HashContext
				{
				public:
					ConcreteSHA224HashContext()
					{
						CC_SHA224_Init(&m_context);
					}

					virtual ~ConcreteSHA224HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						CC_SHA224_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[CC_SHA224_DIGEST_LENGTH];
						CC_SHA224_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)CC_SHA224_DIGEST_LENGTH);
					}

					void Clear()
					{
						CC_SHA224_Init(&m_context);
					}

				protected:
					CC_SHA256_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			SHA224HashContext::SHA224HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA224HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			SHA224HashContext::SHA224HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA224HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			SHA224HashContext::SHA224HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA224HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void SHA224HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void SHA224HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string SHA224HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string SHA224HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA224_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void SHA224HashContext::Clear()
			{
				m_concreteObject->Clear();
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-256 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA256Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[CC_SHA256_DIGEST_LENGTH];
				CC_SHA256((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)CC_SHA256_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA256Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[CC_SHA256_DIGEST_LENGTH];
				CC_SHA256((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)CC_SHA256_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA256Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA256_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA256Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA256_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA256Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA256_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate hash checksum of a given file
			std::string SHA256Hash::CalculateFileHash(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::string("");
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;
				CC_SHA256_CTX hashContext;

				CC_SHA256_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					CC_SHA256_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[CC_SHA256_DIGEST_LENGTH];
				CC_SHA256_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)CC_SHA256_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-256 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteSHA256HashContext
				{
				public:
					ConcreteSHA256HashContext()
					{
						CC_SHA256_Init(&m_context);
					}

					virtual ~ConcreteSHA256HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						CC_SHA256_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[CC_SHA256_DIGEST_LENGTH];
						CC_SHA256_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)CC_SHA256_DIGEST_LENGTH);
					}

					void Clear()
					{
						CC_SHA256_Init(&m_context);
					}

				protected:
					CC_SHA256_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			SHA256HashContext::SHA256HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA256HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			SHA256HashContext::SHA256HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA256HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			SHA256HashContext::SHA256HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA256HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void SHA256HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void SHA256HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string SHA256HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string SHA256HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA256_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void SHA256HashContext::Clear()
			{
				m_concreteObject->Clear();
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-384 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA384Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[CC_SHA384_DIGEST_LENGTH];
				CC_SHA384((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)CC_SHA384_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA384Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[CC_SHA384_DIGEST_LENGTH];
				CC_SHA384((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)CC_SHA384_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA384Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA384_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA384Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA384_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA384Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA384_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate hash checksum of a given file
			std::string SHA384Hash::CalculateFileHash(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::string("");
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;
				CC_SHA512_CTX hashContext;

				CC_SHA384_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					CC_SHA384_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[CC_SHA384_DIGEST_LENGTH];
				CC_SHA384_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)CC_SHA384_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-384 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteSHA384HashContext
				{
				public:
					ConcreteSHA384HashContext()
					{
						CC_SHA384_Init(&m_context);
					}

					virtual ~ConcreteSHA384HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						CC_SHA384_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[CC_SHA384_DIGEST_LENGTH];
						CC_SHA384_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)CC_SHA384_DIGEST_LENGTH);
					}

					void Clear()
					{
						CC_SHA384_Init(&m_context);
					}

				protected:
					CC_SHA512_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			SHA384HashContext::SHA384HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA384HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			SHA384HashContext::SHA384HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA384HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			SHA384HashContext::SHA384HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA384HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void SHA384HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void SHA384HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string SHA384HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string SHA384HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA384_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void SHA384HashContext::Clear()
			{
				m_concreteObject->Clear();
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-512 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA512Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[CC_SHA512_DIGEST_LENGTH];
				CC_SHA512((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)CC_SHA512_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary hash digest of given data
			std::string SHA512Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[CC_SHA512_DIGEST_LENGTH];
				CC_SHA512((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)CC_SHA512_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA512Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA512_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA512Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA512_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hash hex string
			std::string SHA512Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA512_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate hash checksum of a given file
			std::string SHA512Hash::CalculateFileHash(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::string("");
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;
				CC_SHA512_CTX hashContext;

				CC_SHA512_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					CC_SHA512_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[CC_SHA512_DIGEST_LENGTH];
				CC_SHA512_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)CC_SHA512_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// SHA-512 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteSHA512HashContext
				{
				public:
					ConcreteSHA512HashContext()
					{
						CC_SHA512_Init(&m_context);
					}

					virtual ~ConcreteSHA512HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						CC_SHA512_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[CC_SHA512_DIGEST_LENGTH];
						CC_SHA512_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)CC_SHA512_DIGEST_LENGTH);
					}

					void Clear()
					{
						CC_SHA512_Init(&m_context);
					}

				protected:
					CC_SHA512_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			SHA512HashContext::SHA512HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA512HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			SHA512HashContext::SHA512HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA512HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			SHA512HashContext::SHA512HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteSHA512HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void SHA512HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void SHA512HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string SHA512HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string SHA512HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA512_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void SHA512HashContext::Clear()
			{
				m_concreteObject->Clear();
			}
		}
	}
}
