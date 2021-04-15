////////////////////////////////////////////////////////////////////////////////
// Encrypt/AESUtil.h (Leggiero/Modules - Crypto)
//
// AES-256 Encryption Utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__ENCRYPT__AES_UTIL_H
#define __LM_CRYPTO__ENCRYPT__AES_UTIL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <memory>
#include <string>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Encrypt
		{
			// 256-bit key AES Encryption Class (Assume PKCS#7 Padding)
			class AES256
			{
			public:
				// ECB Mode
				class ECB
				{
				public:
					static std::string Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key);
					static std::string Encrypt(const std::string &data, const std::string &key);

					static size_t EncryptInPlace(std::string &data, const std::string &key);

					static std::string Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key);
					static std::string Decrypt(const std::string &data, const std::string &key);

					static size_t DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key);
					static size_t DecryptInPlace(std::string &data, const std::string &key);
				};


				// CBC Mode
				class CBC
				{
				public:
					static std::string Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv);
					static std::string Encrypt(const std::string &data, const std::string &key, const std::string &iv);

					static size_t EncryptInPlace(std::string &data, const std::string &key, const std::string &iv);

					static std::string Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv);
					static std::string Decrypt(const std::string &data, const std::string &key, const std::string &iv);

					static size_t DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv);
					static size_t DecryptInPlace(std::string &data, const std::string &key, const std::string &iv);
				};
			};
		}
	}
}

#endif
