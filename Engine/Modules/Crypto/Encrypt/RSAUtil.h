////////////////////////////////////////////////////////////////////////////////
// Encrypt/RSAUtil.h (Leggiero/Modules - Crypto)
//
// RSA Encryption Utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__ENCRYPT__RSA_UTIL_H
#define __LM_CRYPTO__ENCRYPT__RSA_UTIL_H


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
			// RSA Encryption Class
			//note: only public key interfaces exists. don't use private key in client-side
			class RSA
			{
			public:
				// Assume PKCS#1 OAEP Padding
				static std::string EncryptWithPublicKey(const void *dataBuffer, size_t dataLength, const std::string &pemKey);
				static std::string EncryptWithPublicKey(const std::string &data, const std::string &pemKey);

				// Assume PKCS#1 Padding
				static std::string DecryptWithPublicKey(const void *dataBuffer, size_t dataLength, const std::string &pemKey);
				static std::string DecryptWithPublicKey(const std::string &data, const std::string &pemKey);
			};
		}
	}
}

#endif
