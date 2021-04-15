////////////////////////////////////////////////////////////////////////////////
// Sign/HMAC_SHA_Util_iOS.mm (Leggiero/Modules - Crypto)
//
// CommonCrypto based iOS SHA base HMAC sign utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HMAC_SHA_Util.h"

// Standard Library
#include <cstdio>

// External Library
#import <CommonCrypto/CommonDigest.h>
#import <CommonCrypto/CommonHMAC.h>

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
				unsigned char hmacSignature[CC_SHA1_DIGEST_LENGTH];
				CCHmac(kCCHmacAlgSHA1, key.c_str(), key.length(), dataBufferToHash, dataLength, hmacSignature);
				return std::string((const char *)hmacSignature, (size_t)CC_SHA1_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get binary HMAC-SHA1 signature value of given data
			std::string HMAC_SHA1Sign::CalculateSignature(const std::string &dataToHash, const std::string &key)
			{
				unsigned char hmacSignature[CC_SHA1_DIGEST_LENGTH];
				CCHmac(kCCHmacAlgSHA1, key.c_str(), key.length(), dataToHash.c_str(), dataToHash.length(), hmacSignature);
				return std::string((const char *)hmacSignature, (size_t)CC_SHA1_DIGEST_LENGTH);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const void *dataBufferToHash, size_t dataLength, const std::string &key, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateSignature(dataBufferToHash, dataLength, key), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const std::string &dataToHash, const std::string &key, bool isCapital)
			{
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateSignature(dataToHash, key), isCapital);
			}

			//------------------------------------------------------------------------------
			// Get hexadecimal string of HMAC-SHA1 signature for given data
			std::string HMAC_SHA1Sign::GetSignautreString(const char *dataCStringToHash, const std::string &key, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<CC_SHA1_DIGEST_LENGTH>(CalculateSignature(dataCStringToHash, dataLength, key), isCapital);
			}
		}
	}
}
