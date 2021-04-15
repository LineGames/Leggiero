////////////////////////////////////////////////////////////////////////////////
// Encrypt/AESUtil_OpenSSL.cpp (Leggiero/Modules - Crypto)
//
// OpenSSL based AES encryption utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AESUtil.h"

// Standard Library
#include <cstdio>
#include <vector>

// External Library
#include <openssl/evp.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Encrypt
		{
			//////////////////////////////////////////////////////////////////////////////// AES256 ECB Mode

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 1);

				int encryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &encryptedSize, static_cast<const unsigned char *>(dataBuffer), (int)dataLength);

				int finalSize;
				EVP_CipherFinal_ex(ctx, &resultBuffer[encryptedSize], &finalSize);
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Encrypt(const std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 1);

				int encryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &encryptedSize, (const unsigned char *)data.c_str(), (int)dataLength);

				int finalSize;
				EVP_CipherFinal_ex(ctx, &resultBuffer[encryptedSize], &finalSize);
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)encryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::EncryptInPlace(std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				data.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 1);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int encryptedSize = 0;
				for (size_t pos = 0; pos + 16 < dataLength; pos += 16)
				{
					int currentEncryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentEncryptedSize, (const unsigned char *)(data.c_str() + pos), 16);
					data.replace(pos, currentEncryptedSize, std::string((char *)blockBuffer, 16));
					encryptedSize += currentEncryptedSize;
				}
				size_t remainDataSize = dataLength % 16;
				if (remainDataSize > 0)
				{
					int currentEncryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentEncryptedSize, (const unsigned char *)(data.c_str() + dataLength - remainDataSize), (int)remainDataSize);
					if (currentEncryptedSize > 0)
					{
						data.replace(dataLength - remainDataSize, currentEncryptedSize, std::string((char *)blockBuffer, kBlockBufferSize));
						encryptedSize += currentEncryptedSize;
					}
				}

				int finalSize;
				EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize);
				data.replace(encryptedSize, finalSize, std::string((char *)blockBuffer, kBlockBufferSize));
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				data.resize(encryptedSize);

				return data.length();
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 0);

				int decryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &decryptedSize, static_cast<const unsigned char *>(dataBuffer), (int)dataLength);

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, &resultBuffer[decryptedSize], &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return "";
				}
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)decryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Decrypt(const std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 0);

				int decryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &decryptedSize, (const unsigned char *)data.c_str(), (int)dataLength);

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, &resultBuffer[decryptedSize], &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return "";
				}
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)decryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key)
			{
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 0);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int decryptedSize = 0;
				for (size_t pos = 0; pos < dataLength; pos += 16)
				{
					int currentDecryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentDecryptedSize, ((const unsigned char *)dataBuffer + pos), 16);
					if (currentDecryptedSize > 0)
					{
						memcpy((char *)dataBuffer + decryptedSize, blockBuffer, currentDecryptedSize);
						decryptedSize += currentDecryptedSize;
					}
				}

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return 0;
				}
				if (finalSize > 0)
				{
					memcpy((char *)dataBuffer + decryptedSize, blockBuffer, finalSize);
					decryptedSize += finalSize;
				}

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return decryptedSize;
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::DecryptInPlace(std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), nullptr, (const unsigned char *)key.c_str(), nullptr, 0);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int decryptedSize = 0;
				for (size_t pos = 0; pos < dataLength; pos += 16)
				{
					int currentDecryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentDecryptedSize, (const unsigned char *)(data.c_str() + pos), 16);
					data.replace(pos, currentDecryptedSize, std::string((char *)blockBuffer, kBlockBufferSize));
					decryptedSize += currentDecryptedSize;
				}

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return 0;
				}
				data.replace(decryptedSize, finalSize, std::string((char *)blockBuffer, kBlockBufferSize));
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				data.resize(decryptedSize);

				return data.length();
			}


			//////////////////////////////////////////////////////////////////////////////// AES256 CBC Mode

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 1);

				int encryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &encryptedSize, static_cast<const unsigned char *>(dataBuffer), (int)dataLength);

				int finalSize;
				EVP_CipherFinal_ex(ctx, &resultBuffer[encryptedSize], &finalSize);
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Encrypt(const std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 1);

				int encryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &encryptedSize, (const unsigned char *)data.c_str(), (int)dataLength);

				int finalSize;
				EVP_CipherFinal_ex(ctx, &resultBuffer[encryptedSize], &finalSize);
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)encryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::EncryptInPlace(std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				data.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 1);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int encryptedSize = 0;
				for (size_t pos = 0; pos + 16 < dataLength; pos += 16)
				{
					int currentEncryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentEncryptedSize, (const unsigned char *)(data.c_str() + pos), 16);
					data.replace(pos, currentEncryptedSize, std::string((char *)blockBuffer, 16));
					encryptedSize += currentEncryptedSize;
				}
				size_t remainDataSize = dataLength % 16;
				if (remainDataSize > 0)
				{
					int currentEncryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentEncryptedSize, (const unsigned char *)(data.c_str() + dataLength - remainDataSize), (int)remainDataSize);
					if (currentEncryptedSize > 0)
					{
						data.replace(dataLength - remainDataSize, currentEncryptedSize, std::string((char *)blockBuffer, kBlockBufferSize));
						encryptedSize += currentEncryptedSize;
					}
				}

				int finalSize;
				EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize);
				data.replace(encryptedSize, finalSize, std::string((char *)blockBuffer, kBlockBufferSize));
				encryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				data.resize(encryptedSize);

				return data.length();
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 0);

				int decryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &decryptedSize, static_cast<const unsigned char *>(dataBuffer), (int)dataLength);

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, &resultBuffer[decryptedSize], &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return "";
				}
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)decryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Decrypt(const std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 0);

				int decryptedSize = 0;
				EVP_CipherUpdate(ctx, &resultBuffer[0], &decryptedSize, (const unsigned char *)data.c_str(), (int)dataLength);

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, &resultBuffer[decryptedSize], &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return "";
				}
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return std::string((const char *)&resultBuffer[0], (size_t)decryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 0);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int decryptedSize = 0;
				for (size_t pos = 0; pos < dataLength; pos += 16)
				{
					int currentDecryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentDecryptedSize, ((const unsigned char *)dataBuffer + pos), 16);
					if (currentDecryptedSize > 0)
					{
						memcpy((char *)dataBuffer + decryptedSize, blockBuffer, currentDecryptedSize);
						decryptedSize += currentDecryptedSize;
					}
				}

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return 0;
				}
				if (finalSize > 0)
				{
					memcpy((char *)dataBuffer + decryptedSize, blockBuffer, finalSize);
					decryptedSize += finalSize;
				}

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				return decryptedSize;
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::DecryptInPlace(std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX_init(ctx);

				EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char *)key.c_str(), (const unsigned char *)iv.c_str(), 0);

				constexpr size_t kBlockBufferSize = 32;
				unsigned char blockBuffer[32];

				int decryptedSize = 0;
				for (size_t pos = 0; pos < dataLength; pos += 16)
				{
					int currentDecryptedSize = 0;
					EVP_CipherUpdate(ctx, blockBuffer, &currentDecryptedSize, (const unsigned char *)(data.c_str() + pos), 16);
					data.replace(pos, currentDecryptedSize, std::string((char *)blockBuffer, kBlockBufferSize));
					decryptedSize += currentDecryptedSize;
				}

				int finalSize;
				if (EVP_CipherFinal_ex(ctx, blockBuffer, &finalSize) == 0)
				{
					// Invalid Data
					EVP_CIPHER_CTX_cleanup(ctx);
					EVP_CIPHER_CTX_free(ctx);
					return 0;
				}
				data.replace(decryptedSize, finalSize, std::string((char *)blockBuffer, kBlockBufferSize));
				decryptedSize += finalSize;

				EVP_CIPHER_CTX_cleanup(ctx);
				EVP_CIPHER_CTX_free(ctx);

				data.resize(decryptedSize);

				return data.length();
			}
		}
	}
}
