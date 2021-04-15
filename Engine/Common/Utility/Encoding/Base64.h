////////////////////////////////////////////////////////////////////////////////
// Encoding/Base64.h (Leggiero - Utility)
//
// Base64 Encoding Helper Functions
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__ENCODING__BASE64_H
#define __UTILITY__ENCODING__BASE64_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>
#include <vector>


namespace Leggiero
{
	namespace Utility
	{
		namespace Encoding
		{
			namespace Base64
			{
				using EncodeBufferType = std::vector<char>;

				inline constexpr std::size_t CalculateEncodedSize(std::size_t dataSize) { return ((dataSize + 2) / 3) * 4; }
				inline constexpr std::size_t CalculateDecodedSize(std::size_t base64Size) { return base64Size * 3 / 4; }

				std::string Encode(const std::string &data);
				std::string Decode(const std::string &base64Data);
				std::string EncodeInBuf(EncodeBufferType &buffer, const std::string &data);
				std::string DecodeInBuf(EncodeBufferType &buffer, const std::string &base64Data);

				std::string EncodeBase64URLSafe(const std::string &data);
				std::string DecodeBase64URLSafe(const std::string &base64Data);
				std::string EncodeBase64URLSafeInBuf(EncodeBufferType &buffer, const std::string &data);
				std::string DecodeBase64URLSafeInBuf(EncodeBufferType &buffer, const std::string &base64Data);
			}
		}
	}
}

#endif
