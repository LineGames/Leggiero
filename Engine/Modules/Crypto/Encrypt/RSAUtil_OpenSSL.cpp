////////////////////////////////////////////////////////////////////////////////
// Encrypt/RSAUtil_OpenSSL.cpp (Leggiero/Modules - Crypto)
//
// OpenSSL based RSA encryption utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RSAUtil.h"

// Standard Library
#include <cstdio>
#include <vector>

// External Library
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Encrypt
		{
			//////////////////////////////////////////////////////////////////////////////// RSA

			//------------------------------------------------------------------------------
			std::string RSA::EncryptWithPublicKey(const void *dataBuffer, size_t dataLength, const std::string &pemKey)
			{
				if (dataLength == 0)
				{
					return "";
				}

				std::vector<char> pemKeyCopy(pemKey.length() + 1, '\0');
				for (size_t i = 0; i < pemKey.length(); ++i)
				{
					pemKeyCopy[i] = pemKey[i];
				}
				BIO *keyBio = BIO_new_mem_buf(&pemKeyCopy[0], -1);

				::RSA *rsa = nullptr;
				PEM_read_bio_RSA_PUBKEY(keyBio, &rsa, nullptr, nullptr);
				if (rsa == nullptr)
				{
					// Invalid RSA Key
					BIO_free_all(keyBio);
					return "";
				}

				int size = RSA_size(rsa);
				int blockContentsMaxSize = size - 42;
				if (blockContentsMaxSize < 1)
				{
					// Invalid RSA Key
					RSA_free(rsa);
					BIO_free_all(keyBio);
					return "";
				}

				int blockCount = ((int)(dataLength - 1) / blockContentsMaxSize) + 1;
				size_t maximumDataSize = (size_t)(blockCount * size);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				bool isSuccess = true;
				for (int i = 0; i < blockCount; ++i)
				{
					int flen = blockContentsMaxSize;
					if (i == blockCount - 1)
					{
						flen = (int)dataLength - i * blockContentsMaxSize;
					}

					int result = RSA_public_encrypt(flen, static_cast<const unsigned char *>(dataBuffer) + i * blockContentsMaxSize, &resultBuffer[encryptedSize], rsa, RSA_PKCS1_OAEP_PADDING);
					if (result < 0)
					{
						// RSA Error Occured
						isSuccess = false;
						break;
					}
					encryptedSize += (size_t)result;
				}

				RSA_free(rsa);
				BIO_free_all(keyBio);

				if (!isSuccess)
				{
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string RSA::EncryptWithPublicKey(const std::string &data, const std::string &pemKey)
			{
				size_t dataLength = data.length();
				if (dataLength == 0)
				{
					return "";
				}

				std::vector<char> pemKeyCopy(pemKey.length() + 1, '\0');
				for (size_t i = 0; i < pemKey.length(); ++i)
				{
					pemKeyCopy[i] = pemKey[i];
				}
				BIO *keyBio = BIO_new_mem_buf(&pemKeyCopy[0], -1);

				::RSA *rsa = nullptr;
				PEM_read_bio_RSA_PUBKEY(keyBio, &rsa, nullptr, nullptr);
				if (rsa == nullptr)
				{
					// Invalid RSA Key
					BIO_free_all(keyBio);
					return "";
				}

				int size = RSA_size(rsa);
				int blockContentsMaxSize = size - 42;
				if (blockContentsMaxSize < 1)
				{
					// Invalid RSA Key
					RSA_free(rsa);
					BIO_free_all(keyBio);
					return "";
				}

				int blockCount = ((int)(dataLength - 1) / blockContentsMaxSize) + 1;
				size_t maximumDataSize = (size_t)(blockCount * size);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				bool isSuccess = true;
				for (int i = 0; i < blockCount; ++i)
				{
					int flen = blockContentsMaxSize;
					if (i == blockCount - 1)
					{
						flen = (int)dataLength - i * blockContentsMaxSize;
					}

					int result = RSA_public_encrypt(flen, (const unsigned char *)data.c_str() + i * blockContentsMaxSize, &resultBuffer[encryptedSize], rsa, RSA_PKCS1_OAEP_PADDING);
					if (result < 0)
					{
						// RSA Error Occured
						isSuccess = false;
						break;
					}
					encryptedSize += (size_t)result;
				}

				RSA_free(rsa);
				BIO_free_all(keyBio);

				if (!isSuccess)
				{
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string RSA::DecryptWithPublicKey(const void *dataBuffer, size_t dataLength, const std::string &pemKey)
			{
				if (dataLength == 0)
				{
					return "";
				}

				std::vector<char> pemKeyCopy(pemKey.length() + 1, '\0');
				for (size_t i = 0; i < pemKey.length(); ++i)
				{
					pemKeyCopy[i] = pemKey[i];
				}
				BIO *keyBio = BIO_new_mem_buf(&pemKeyCopy[0], -1);

				::RSA *rsa = nullptr;
				PEM_read_bio_RSA_PUBKEY(keyBio, &rsa, nullptr, nullptr);
				if (rsa == nullptr)
				{
					// Invalid RSA Key
					BIO_free_all(keyBio);
					return "";
				}

				int size = RSA_size(rsa);
				int blockCount = (int)dataLength / size;
				size_t maximumDataSize = (size_t)(blockCount * size);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				bool isSuccess = true;
				for (int i = 0; i < blockCount; ++i)
				{
					int result = RSA_public_decrypt(size, static_cast<const unsigned char *>(dataBuffer) + i * size, &resultBuffer[decryptedSize], rsa, RSA_PKCS1_PADDING);
					if (result < 0)
					{
						// RSA Error Occured
						isSuccess = false;
						break;
					}
					decryptedSize += (size_t)result;
				}

				RSA_free(rsa);
				BIO_free_all(keyBio);

				if (!isSuccess)
				{
					return "";
				}

				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string RSA::DecryptWithPublicKey(const std::string &data, const std::string &pemKey)
			{
				size_t dataLength = data.length();
				if (dataLength == 0)
				{
					return "";
				}

				std::vector<char> pemKeyCopy(pemKey.length() + 1, '\0');
				for (size_t i = 0; i < pemKey.length(); ++i)
				{
					pemKeyCopy[i] = pemKey[i];
				}
				BIO *keyBio = BIO_new_mem_buf(&pemKeyCopy[0], -1);

				::RSA *rsa = nullptr;
				PEM_read_bio_RSA_PUBKEY(keyBio, &rsa, nullptr, nullptr);
				if (rsa == nullptr)
				{
					// Invalid RSA Key
					BIO_free_all(keyBio);
					return "";
				}

				int size = RSA_size(rsa);
				int blockCount = (int)dataLength / size;
				size_t maximumDataSize = (size_t)(blockCount * size);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				bool isSuccess = true;
				for (int i = 0; i < blockCount; ++i)
				{
					int result = RSA_public_decrypt(size, (const unsigned char *)data.c_str() + i * size, &resultBuffer[decryptedSize], rsa, RSA_PKCS1_PADDING);
					if (result < 0)
					{
						// RSA Error Occured
						isSuccess = false;
						break;
					}
					decryptedSize += (size_t)result;
				}

				RSA_free(rsa);
				BIO_free_all(keyBio);

				if (!isSuccess)
				{
					return "";
				}

				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}
		}
	}
}
