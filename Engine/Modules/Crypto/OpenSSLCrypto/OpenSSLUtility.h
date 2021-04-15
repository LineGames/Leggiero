////////////////////////////////////////////////////////////////////////////////
// OpenSSLCrypto/OpenSSLUtility.h (Leggiero/Modules - Crypto)
//
// OpenSSL based cryptography utility
// Recommended for internal use
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__OPENSSLCRYPTO__OPENSSL_UTILITY_H
#define __LM_CRYPTO__OPENSSLCRYPTO__OPENSSL_UTILITY_H


namespace Leggiero
{
	namespace Crypto
	{
		namespace OpenSSL
		{
			void InitializeOpenSSLCryptoSystem();
			void FinalizeOpenSSLCryptoSystem();
		}
	}
}

#endif
