////////////////////////////////////////////////////////////////////////////////
// Encoding/URLEncoding.h (Leggiero - Utility)
//
// URL Encoding Functions
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__ENCODING__URL_ENCODING_H
#define __UTILITY__ENCODING__URL_ENCODING_H


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
			namespace URL
			{
				using EncodeBufferType = std::vector<char>;

				std::string Encode(const std::string &original, bool isXFromsSpec = false);
				std::string Decode(const std::string &urlEncoded);

				std::string EncodeInBuf(EncodeBufferType &buffer, const std::string &original, bool isXFromsSpec = false);
				std::string DecodeInBuf(EncodeBufferType &buffer, const std::string &urlEncoded);
			}
		}
	}
}

#endif
