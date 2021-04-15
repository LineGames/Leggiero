////////////////////////////////////////////////////////////////////////////////
// Encoding/Base64.cpp (Leggiero - Utility)
//
// Base64 Helper Functions Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Base64.h"

// Standard Library
#include <cstddef>
#include <vector>

// External Library
#include <b64/encode.h>
#include <b64/decode.h>


namespace Leggiero
{
	namespace Utility
	{
		namespace Encoding
		{
			namespace Base64
			{
				//------------------------------------------------------------------------------
				std::string Encode(const std::string &data)
				{
					EncodeBufferType tempBuffer;
					return EncodeInBuf(tempBuffer, data);
				}

				//------------------------------------------------------------------------------
				std::string Decode(const std::string &base64Data)
				{
					EncodeBufferType tempBuffer;
					return DecodeInBuf(tempBuffer, base64Data);
				}

				//------------------------------------------------------------------------------
				std::string EncodeInBuf(EncodeBufferType &buffer, const std::string &data)
				{
					if (data.empty())
					{
						return "";
					}
					buffer.resize(CalculateEncodedSize(data.length()) + 1);
					base64::encoder E;
					int length = E.encode(data.c_str(), (int)data.length(), &buffer[0]);
					length += E.encode_end(&buffer[length]);
					return std::string(&buffer[0], (size_t)length);
				}

				//------------------------------------------------------------------------------
				std::string DecodeInBuf(EncodeBufferType &buffer, const std::string &base64Data)
				{
					if (base64Data.length() == 0)
					{
						return "";
					}
					if (base64Data.length() % 4 != 0)
					{
						std::string paddedString(base64Data + std::string((size_t)(4 - (base64Data.length() % 4)), '='));
						buffer.resize(CalculateDecodedSize(paddedString.length()) + 1);
						base64::decoder D;
						int length = D.decode(paddedString.c_str(), (int)paddedString.length(), &buffer[0]);
						return std::string(&buffer[0], (size_t)length);
					}
					else
					{
						buffer.resize(CalculateDecodedSize(base64Data.length()) + 1);
						base64::decoder D;
						int length = D.decode(base64Data.c_str(), (int)base64Data.length(), &buffer[0]);
						return std::string(&buffer[0], (size_t)length);
					}
				}

				//------------------------------------------------------------------------------
				std::string EncodeBase64URLSafe(const std::string &data)
				{
					EncodeBufferType tempBuffer;
					return EncodeBase64URLSafeInBuf(tempBuffer, data);
				}

				//------------------------------------------------------------------------------
				std::string DecodeBase64URLSafe(const std::string &base64Data)
				{
					EncodeBufferType tempBuffer;
					return DecodeBase64URLSafeInBuf(tempBuffer, base64Data);
				}

				//------------------------------------------------------------------------------
				std::string EncodeBase64URLSafeInBuf(EncodeBufferType &buffer, const std::string &data)
				{
					if (data.empty())
					{
						return "";
					}
					buffer.resize(CalculateEncodedSize(data.length()) + 1);
					base64::encoder E;
					int length = E.encode(data.c_str(), (int)data.length(), &buffer[0]);
					length += E.encode_end(&buffer[length]);
					// Subs
					for (int i = 0; i < length; ++i)
					{
						switch (buffer[i])
						{
							case '+': buffer[i] = '-'; break;
							case '/': buffer[i] = '_'; break;
						}
					}
					if (length < 3)
					{
						return std::string(&buffer[0], (size_t)length);
					}
					// Trim
					while (buffer[length - 1] == '=' && length > 1)
					{
						--length;
					}
					return std::string(&buffer[0], (size_t)length);
				}

				//------------------------------------------------------------------------------
				std::string DecodeBase64URLSafeInBuf(EncodeBufferType &buffer, const std::string &base64Data)
				{
					if (base64Data.empty())
					{
						return "";
					}
					std::string filteredData(base64Data);
					size_t originalDataLength = base64Data.length();
					switch (base64Data.length() % 4)
					{
						case 2: filteredData += "=="; break;
						case 3: filteredData += '='; break;
					}
					for (int i = 0; i < originalDataLength; ++i)
					{
						switch (filteredData[i])
						{
							case '-': filteredData[i] = '+'; break;
							case '_': filteredData[i] = '/'; break;
						}
					}
					buffer.resize(CalculateDecodedSize(filteredData.length()) + 1);
					base64::decoder D;
					int length = D.decode(filteredData.c_str(), (int)filteredData.length(), &buffer[0]);
					return std::string(&buffer[0], (size_t)length);
				}
			}
		}
	}
}
