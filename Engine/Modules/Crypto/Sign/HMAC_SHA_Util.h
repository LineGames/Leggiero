////////////////////////////////////////////////////////////////////////////////
// Sign/HMAC_SHA_Util.h (Leggiero/Modules - Crypto)
//
// SHA hash based HMAC signing utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__SIGN__HMAC_SHA_UTIL_H
#define __LM_CRYPTO__SIGN__HMAC_SHA_UTIL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <memory>
#include <string>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Sign
		{
			// HMAC-SHA1 Sign Utility
			class HMAC_SHA1Sign
			{
			public:
				// Get binary HMAC-SHA1 signature value of given data
				static std::string CalculateSignature(const void *dataBufferToHash, size_t dataLength, const std::string &key);
				static std::string CalculateSignature(const std::string &dataToHash, const std::string &key);

				// Get hexadecimal string of HMAC-SHA1 signature for given data
				static std::string GetSignautreString(const void *dataBufferToHash, size_t dataLength, const std::string &key, bool isCapital = false);
				static std::string GetSignautreString(const std::string &dataToHash, const std::string &key, bool isCapital = false);
				static std::string GetSignautreString(const char *dataCStringToHash, const std::string &key, bool isCapital = false);
			};
		}
	}
}

#endif
