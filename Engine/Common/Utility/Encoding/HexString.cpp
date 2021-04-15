////////////////////////////////////////////////////////////////////////////////
// Encoding/HexString.cpp (Leggiero - Utility)
//
// Hex String Encoding Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HexString.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Encoding
		{
			namespace HexString
			{
				//------------------------------------------------------------------------------
				std::string ToHexString(const std::string &binaryString, bool isCapital)
				{
					return ToHexString(binaryString.c_str(), binaryString.length(), isCapital);
				}

				//------------------------------------------------------------------------------
				std::string ToHexString(const void *dataBuffer, size_t dataLength, bool isCapital)
				{
					const uint8_t *byteBuffer = (const uint8_t *)dataBuffer;
					std::string resultBuffer(dataLength * 2, '\0');
					for (size_t i = 0; i < dataLength; ++i)
					{
						Unsafe::WriteOctetHex(resultBuffer, i * 2, byteBuffer[i], isCapital);
					}
					return resultBuffer;
				}

				//------------------------------------------------------------------------------
				size_t WriteHexString(const std::string &binaryString, char *buffer, size_t bufferLength, bool isCapital)
				{
					return WriteHexString(binaryString.c_str(), binaryString.length(), buffer, bufferLength, isCapital);
				}

				//------------------------------------------------------------------------------
				size_t WriteHexString(const void *dataBuffer, size_t dataLength, char *buffer, size_t bufferLength, bool isCapital)
				{
					const uint8_t *byteBuffer = (const uint8_t *)dataBuffer;
					size_t writingLength = dataLength * 2;
					if (bufferLength < writingLength)
					{
						writingLength = bufferLength;
						if ((writingLength % 2) == 1)
						{
							--writingLength;
						}
					}
					size_t sourceLength = writingLength / 2;
					for (size_t i = 0; i < sourceLength; ++i)
					{
						Unsafe::WriteOctetHex(buffer, i * 2, byteBuffer[i], isCapital);
					}
					return writingLength;
				}

				//------------------------------------------------------------------------------
				size_t WriteHexString(const std::string &binaryString, std::vector<char> &buffer, bool isCapital)
				{
					return WriteHexString(binaryString.c_str(), binaryString.length(), buffer, isCapital);
				}

				//------------------------------------------------------------------------------
				size_t WriteHexString(const void *dataBuffer, size_t dataLength, std::vector<char> &buffer, bool isCapital)
				{
					const uint8_t *byteBuffer = (const uint8_t *)dataBuffer;
					buffer.resize(dataLength * 2);
					for (size_t i = 0; i < dataLength; ++i)
					{
						Unsafe::WriteOctetHex(buffer, i * 2, byteBuffer[i], isCapital);
					}
					return buffer.size();
				}

				//------------------------------------------------------------------------------
				std::string FromHexString(const std::string &hexString)
				{
					return FromHexString(hexString.c_str(), hexString.length());
				}

				//------------------------------------------------------------------------------
				std::string FromHexString(const char *stringBuffer, size_t stringLength)
				{
					std::string resultBuffer(stringLength / 2, '\0');
					for (size_t i = 0; i < stringLength; i += 2)
					{
						union
						{
							uint8_t source;
							char target;
						} a = { Unsafe::ReadOctetHex(stringBuffer, i) };
						resultBuffer[i / 2] = a.target;
					}
					return resultBuffer;
				}

				//------------------------------------------------------------------------------
				size_t ParseHexString(const std::string &hexString, void *buffer, size_t bufferLength)
				{
					return ParseHexString(hexString.c_str(), hexString.length(), buffer, bufferLength);
				}

				//------------------------------------------------------------------------------
				size_t ParseHexString(const char *stringBuffer, size_t stringLength, void *buffer, size_t bufferLength)
				{
					uint8_t *byteTargetBuffer = (uint8_t *)buffer;
					size_t writingLength = stringLength / 2;
					if (bufferLength < writingLength)
					{
						writingLength = bufferLength;
					}
					for (size_t i = 0; i < writingLength; ++i)
					{
						byteTargetBuffer[i] = Unsafe::ReadOctetHex(stringBuffer, i * 2);
					}
					return writingLength;
				}

				//------------------------------------------------------------------------------
				size_t ParseHexString(const std::string &hexString, std::vector<uint8_t> &buffer)
				{
					return ParseHexString(hexString.c_str(), hexString.length(), buffer);
				}

				//------------------------------------------------------------------------------
				size_t ParseHexString(const char *stringBuffer, size_t stringLength, std::vector<uint8_t> &buffer)
				{
					buffer.resize(stringLength / 2);
					for (size_t i = 0; i < stringLength; i += 2)
					{
						buffer[i / 2] = Unsafe::ReadOctetHex(stringBuffer, i);
					}
					return buffer.size();
				}
			}
		}
	}
}
