////////////////////////////////////////////////////////////////////////////////
// Sign/HMAC_SHA_Util_OpenSSL.cpp (Leggiero/Modules - Crypto)
//
// OpenSSL based SHA base HMAC sign utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HMAC_SHA_Util.h"

// Standard Library
#include <cstdio>

// External Library
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

// Leggiero.Utility
#include <Utility/Encoding/HexString.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Sign
		{
			//////////////////////////////////////////////////////////////////////////////// HMAC-SHA1 Sign Utility

			//------------------------------------------------------------------------------
			// Get binary HMAC-SHA1 signature value of given data
			std::string HMAC_SHA1Sign::CalculateSignature(const void *dataBufferToHash, size_t dataLength, const std::string &key)
			{
				unsigned char digest[EVP_MAX_MD_SIZE];
				::HMAC(EVP_sha1(), key.c_str(), (int)key.length(), static_cast<const unsigned char *>(dataBufferToHash), (int)dataLength, digest, nullptr);
				return std::string((const char *)digest, (size_t)SHA_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get binary HMAC-SHA1 signature value of given data
			std::string HMAC_SHA1Sign::CalculateSignature(const std::string &dataToHash, const std::string &key)
			{
				unsigned char digest[EVP_MAX_MD_SIZE];
				::HMAC(EVP_sha1(), key.c_str(), (int)key.length(), (const unsigned char *)dataToHash.c_str(), (int)dataToHash.length(), digest, nullptr);
				return std::string((const char *)digest, (size_t)SHA_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const void *dataBufferToHash, size_t dataLength, const std::string &key, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<SHA_DIGEST_LENGTH>(CalculateSignature(dataBufferToHash, dataLength, key), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const std::string &dataToHash, const std::string &key, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<SHA_DIGEST_LENGTH>(CalculateSignature(dataToHash, key), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const char *dataCStringToHash, const std::string &key, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<SHA_DIGEST_LENGTH>(CalculateSignature(dataCStringToHash, dataLength, key), isCapital);
			}
		}
	}
}
