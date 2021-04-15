////////////////////////////////////////////////////////////////////////////////
// Hash/MD5Util_OpenSSL.cpp (Leggiero/Modules - Crypto)
//
// OpenSSL based MD5 hash utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "MD5Util.h"

// Standard Library
#include <cstdio>

// External Library
#include <openssl/md5.h>

// Leggiero.Utility
#include <Utility/Encoding/HexString.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			//////////////////////////////////////////////////////////////////////////////// MD5 Hash Utility

			//------------------------------------------------------------------------------
			// Calculate binary MD5 digest of given data
			std::string MD5Hash::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				unsigned char digest[MD5_DIGEST_LENGTH];
				MD5((unsigned char *)dataBufferToHash, dataLength, digest);
				return std::string((const char *)digest, (size_t)MD5_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate binary MD5 digest of given data
			std::string MD5Hash::CalculateHash(const std::string &dataToHash)
			{
				unsigned char digest[MD5_DIGEST_LENGTH];
				MD5((unsigned char *)dataToHash.c_str(), dataToHash.length(), digest);
				return std::string((const char *)digest, (size_t)MD5_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Calculate MD5 Hash as hex string
			std::string MD5Hash::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<MD5_DIGEST_LENGTH>(CalculateHash(dataBufferToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate MD5 Hash as hex string
			std::string MD5Hash::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<MD5_DIGEST_LENGTH>(CalculateHash(dataToHash), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate MD5 Hash as hex string
			std::string MD5Hash::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<MD5_DIGEST_LENGTH>(CalculateHash(dataCStringToHash, dataLength), isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate MD5 Checksum of a given file
			std::string MD5Hash::CalculateFileHash(const std::string &filePath)
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
				MD5_CTX hashContext;

				MD5_Init(&hashContext);

				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					MD5_Update(&hashContext, buf, readBytes);
				}
				fclose(targetFile);

				unsigned char digest[MD5_DIGEST_LENGTH];
				MD5_Final(digest, &hashContext);

				return std::string((const char *)digest, (size_t)MD5_DIGEST_LENGTH);
			}


			//////////////////////////////////////////////////////////////////////////////// MD5 Hash Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteMD5HashContext
				{
				public:
					ConcreteMD5HashContext()
					{
						MD5_Init(&m_context);
					}

					virtual ~ConcreteMD5HashContext() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						MD5_Update(&m_context, dataBufferToAppend, dataLength);
					}

					std::string GetHash()
					{
						unsigned char digest[MD5_DIGEST_LENGTH];
						MD5_Final(digest, &m_context);
						return std::string((const char *)digest, (size_t)MD5_DIGEST_LENGTH);
					}

					void Clear()
					{
						MD5_Init(&m_context);
					}

				protected:
					MD5_CTX m_context;
				};
			}

			//------------------------------------------------------------------------------
			MD5HashContext::MD5HashContext()
				: m_concreteObject(std::make_shared<_Internal::ConcreteMD5HashContext>())
			{
			}

			//------------------------------------------------------------------------------
			MD5HashContext::MD5HashContext(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteMD5HashContext>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			MD5HashContext::MD5HashContext(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteMD5HashContext>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void MD5HashContext::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void MD5HashContext::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string MD5HashContext::GetHash()
			{
				return m_concreteObject->GetHash();
			}

			//------------------------------------------------------------------------------
			std::string MD5HashContext::GetHashString(bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<MD5_DIGEST_LENGTH>(m_concreteObject->GetHash(), isCapital);
			}

			//------------------------------------------------------------------------------
			void MD5HashContext::Clear()
			{
				m_concreteObject->Clear();
			}
		}
	}
}
