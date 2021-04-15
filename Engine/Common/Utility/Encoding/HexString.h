////////////////////////////////////////////////////////////////////////////////
// Encoding/HexString.h (Leggiero - Utility)
//
// Binary - Hexadecimal string encoding utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__ENCODING__HEX_STRING_H
#define __UTILITY__ENCODING__HEX_STRING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>


namespace Leggiero
{
	namespace Utility
	{
		namespace Encoding
		{
			namespace HexString
			{
				std::string ToHexString(const std::string &binaryString, bool isCapital = false);
				std::string ToHexString(const void *dataBuffer, size_t dataLength, bool isCapital = false);

				size_t WriteHexString(const std::string &binaryString, char *buffer, size_t bufferLength, bool isCapital = false);
				size_t WriteHexString(const void *dataBuffer, size_t dataLength, char *buffer, size_t bufferLength, bool isCapital = false);

				size_t WriteHexString(const std::string &binaryString, std::vector<char> &buffer, bool isCapital = false);
				size_t WriteHexString(const void *dataBuffer, size_t dataLength, std::vector<char> &buffer, bool isCapital = false);

				std::string FromHexString(const std::string &hexString);
				std::string FromHexString(const char *stringBuffer, size_t stringLength);

				size_t ParseHexString(const std::string &hexString, void *buffer, size_t bufferLength);
				size_t ParseHexString(const char *stringBuffer, size_t stringLength, void *buffer, size_t bufferLength);

				size_t ParseHexString(const std::string &hexString, std::vector<uint8_t> &buffer);
				size_t ParseHexString(const char *stringBuffer, size_t stringLength, std::vector<uint8_t> &buffer);

				// Un-safe hex manipulation utilites
				class Unsafe
				{
				public:
					// Hope to faster than sprintf
					template <typename BufType>
					static void WriteOctetHex(BufType buf, size_t bufOffset, uint8_t hextData, bool isCapital)
					{
						switch (hextData)
						{
							case 0: buf[bufOffset] = '0'; buf[bufOffset + 1] = '0'; break;
							case 1: buf[bufOffset] = '0'; buf[bufOffset + 1] = '1'; break;
							case 2: buf[bufOffset] = '0'; buf[bufOffset + 1] = '2'; break;
							case 3: buf[bufOffset] = '0'; buf[bufOffset + 1] = '3'; break;
							case 4: buf[bufOffset] = '0'; buf[bufOffset + 1] = '4'; break;
							case 5: buf[bufOffset] = '0'; buf[bufOffset + 1] = '5'; break;
							case 6: buf[bufOffset] = '0'; buf[bufOffset + 1] = '6'; break;
							case 7: buf[bufOffset] = '0'; buf[bufOffset + 1] = '7'; break;
							case 8: buf[bufOffset] = '0'; buf[bufOffset + 1] = '8'; break;
							case 9: buf[bufOffset] = '0'; buf[bufOffset + 1] = '9'; break;
							case 10: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 11: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 12: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 13: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 14: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 15: buf[bufOffset] = '0'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 16: buf[bufOffset] = '1'; buf[bufOffset + 1] = '0'; break;
							case 17: buf[bufOffset] = '1'; buf[bufOffset + 1] = '1'; break;
							case 18: buf[bufOffset] = '1'; buf[bufOffset + 1] = '2'; break;
							case 19: buf[bufOffset] = '1'; buf[bufOffset + 1] = '3'; break;
							case 20: buf[bufOffset] = '1'; buf[bufOffset + 1] = '4'; break;
							case 21: buf[bufOffset] = '1'; buf[bufOffset + 1] = '5'; break;
							case 22: buf[bufOffset] = '1'; buf[bufOffset + 1] = '6'; break;
							case 23: buf[bufOffset] = '1'; buf[bufOffset + 1] = '7'; break;
							case 24: buf[bufOffset] = '1'; buf[bufOffset + 1] = '8'; break;
							case 25: buf[bufOffset] = '1'; buf[bufOffset + 1] = '9'; break;
							case 26: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 27: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 28: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 29: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 30: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 31: buf[bufOffset] = '1'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 32: buf[bufOffset] = '2'; buf[bufOffset + 1] = '0'; break;
							case 33: buf[bufOffset] = '2'; buf[bufOffset + 1] = '1'; break;
							case 34: buf[bufOffset] = '2'; buf[bufOffset + 1] = '2'; break;
							case 35: buf[bufOffset] = '2'; buf[bufOffset + 1] = '3'; break;
							case 36: buf[bufOffset] = '2'; buf[bufOffset + 1] = '4'; break;
							case 37: buf[bufOffset] = '2'; buf[bufOffset + 1] = '5'; break;
							case 38: buf[bufOffset] = '2'; buf[bufOffset + 1] = '6'; break;
							case 39: buf[bufOffset] = '2'; buf[bufOffset + 1] = '7'; break;
							case 40: buf[bufOffset] = '2'; buf[bufOffset + 1] = '8'; break;
							case 41: buf[bufOffset] = '2'; buf[bufOffset + 1] = '9'; break;
							case 42: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 43: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 44: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 45: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 46: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 47: buf[bufOffset] = '2'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 48: buf[bufOffset] = '3'; buf[bufOffset + 1] = '0'; break;
							case 49: buf[bufOffset] = '3'; buf[bufOffset + 1] = '1'; break;
							case 50: buf[bufOffset] = '3'; buf[bufOffset + 1] = '2'; break;
							case 51: buf[bufOffset] = '3'; buf[bufOffset + 1] = '3'; break;
							case 52: buf[bufOffset] = '3'; buf[bufOffset + 1] = '4'; break;
							case 53: buf[bufOffset] = '3'; buf[bufOffset + 1] = '5'; break;
							case 54: buf[bufOffset] = '3'; buf[bufOffset + 1] = '6'; break;
							case 55: buf[bufOffset] = '3'; buf[bufOffset + 1] = '7'; break;
							case 56: buf[bufOffset] = '3'; buf[bufOffset + 1] = '8'; break;
							case 57: buf[bufOffset] = '3'; buf[bufOffset + 1] = '9'; break;
							case 58: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 59: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 60: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 61: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 62: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 63: buf[bufOffset] = '3'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 64: buf[bufOffset] = '4'; buf[bufOffset + 1] = '0'; break;
							case 65: buf[bufOffset] = '4'; buf[bufOffset + 1] = '1'; break;
							case 66: buf[bufOffset] = '4'; buf[bufOffset + 1] = '2'; break;
							case 67: buf[bufOffset] = '4'; buf[bufOffset + 1] = '3'; break;
							case 68: buf[bufOffset] = '4'; buf[bufOffset + 1] = '4'; break;
							case 69: buf[bufOffset] = '4'; buf[bufOffset + 1] = '5'; break;
							case 70: buf[bufOffset] = '4'; buf[bufOffset + 1] = '6'; break;
							case 71: buf[bufOffset] = '4'; buf[bufOffset + 1] = '7'; break;
							case 72: buf[bufOffset] = '4'; buf[bufOffset + 1] = '8'; break;
							case 73: buf[bufOffset] = '4'; buf[bufOffset + 1] = '9'; break;
							case 74: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 75: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 76: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 77: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 78: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 79: buf[bufOffset] = '4'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 80: buf[bufOffset] = '5'; buf[bufOffset + 1] = '0'; break;
							case 81: buf[bufOffset] = '5'; buf[bufOffset + 1] = '1'; break;
							case 82: buf[bufOffset] = '5'; buf[bufOffset + 1] = '2'; break;
							case 83: buf[bufOffset] = '5'; buf[bufOffset + 1] = '3'; break;
							case 84: buf[bufOffset] = '5'; buf[bufOffset + 1] = '4'; break;
							case 85: buf[bufOffset] = '5'; buf[bufOffset + 1] = '5'; break;
							case 86: buf[bufOffset] = '5'; buf[bufOffset + 1] = '6'; break;
							case 87: buf[bufOffset] = '5'; buf[bufOffset + 1] = '7'; break;
							case 88: buf[bufOffset] = '5'; buf[bufOffset + 1] = '8'; break;
							case 89: buf[bufOffset] = '5'; buf[bufOffset + 1] = '9'; break;
							case 90: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 91: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 92: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 93: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 94: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 95: buf[bufOffset] = '5'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 96: buf[bufOffset] = '6'; buf[bufOffset + 1] = '0'; break;
							case 97: buf[bufOffset] = '6'; buf[bufOffset + 1] = '1'; break;
							case 98: buf[bufOffset] = '6'; buf[bufOffset + 1] = '2'; break;
							case 99: buf[bufOffset] = '6'; buf[bufOffset + 1] = '3'; break;
							case 100: buf[bufOffset] = '6'; buf[bufOffset + 1] = '4'; break;
							case 101: buf[bufOffset] = '6'; buf[bufOffset + 1] = '5'; break;
							case 102: buf[bufOffset] = '6'; buf[bufOffset + 1] = '6'; break;
							case 103: buf[bufOffset] = '6'; buf[bufOffset + 1] = '7'; break;
							case 104: buf[bufOffset] = '6'; buf[bufOffset + 1] = '8'; break;
							case 105: buf[bufOffset] = '6'; buf[bufOffset + 1] = '9'; break;
							case 106: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 107: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 108: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 109: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 110: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 111: buf[bufOffset] = '6'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 112: buf[bufOffset] = '7'; buf[bufOffset + 1] = '0'; break;
							case 113: buf[bufOffset] = '7'; buf[bufOffset + 1] = '1'; break;
							case 114: buf[bufOffset] = '7'; buf[bufOffset + 1] = '2'; break;
							case 115: buf[bufOffset] = '7'; buf[bufOffset + 1] = '3'; break;
							case 116: buf[bufOffset] = '7'; buf[bufOffset + 1] = '4'; break;
							case 117: buf[bufOffset] = '7'; buf[bufOffset + 1] = '5'; break;
							case 118: buf[bufOffset] = '7'; buf[bufOffset + 1] = '6'; break;
							case 119: buf[bufOffset] = '7'; buf[bufOffset + 1] = '7'; break;
							case 120: buf[bufOffset] = '7'; buf[bufOffset + 1] = '8'; break;
							case 121: buf[bufOffset] = '7'; buf[bufOffset + 1] = '9'; break;
							case 122: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 123: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 124: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 125: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 126: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 127: buf[bufOffset] = '7'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 128: buf[bufOffset] = '8'; buf[bufOffset + 1] = '0'; break;
							case 129: buf[bufOffset] = '8'; buf[bufOffset + 1] = '1'; break;
							case 130: buf[bufOffset] = '8'; buf[bufOffset + 1] = '2'; break;
							case 131: buf[bufOffset] = '8'; buf[bufOffset + 1] = '3'; break;
							case 132: buf[bufOffset] = '8'; buf[bufOffset + 1] = '4'; break;
							case 133: buf[bufOffset] = '8'; buf[bufOffset + 1] = '5'; break;
							case 134: buf[bufOffset] = '8'; buf[bufOffset + 1] = '6'; break;
							case 135: buf[bufOffset] = '8'; buf[bufOffset + 1] = '7'; break;
							case 136: buf[bufOffset] = '8'; buf[bufOffset + 1] = '8'; break;
							case 137: buf[bufOffset] = '8'; buf[bufOffset + 1] = '9'; break;
							case 138: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 139: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 140: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 141: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 142: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 143: buf[bufOffset] = '8'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 144: buf[bufOffset] = '9'; buf[bufOffset + 1] = '0'; break;
							case 145: buf[bufOffset] = '9'; buf[bufOffset + 1] = '1'; break;
							case 146: buf[bufOffset] = '9'; buf[bufOffset + 1] = '2'; break;
							case 147: buf[bufOffset] = '9'; buf[bufOffset + 1] = '3'; break;
							case 148: buf[bufOffset] = '9'; buf[bufOffset + 1] = '4'; break;
							case 149: buf[bufOffset] = '9'; buf[bufOffset + 1] = '5'; break;
							case 150: buf[bufOffset] = '9'; buf[bufOffset + 1] = '6'; break;
							case 151: buf[bufOffset] = '9'; buf[bufOffset + 1] = '7'; break;
							case 152: buf[bufOffset] = '9'; buf[bufOffset + 1] = '8'; break;
							case 153: buf[bufOffset] = '9'; buf[bufOffset + 1] = '9'; break;
							case 154: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 155: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 156: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 157: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 158: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 159: buf[bufOffset] = '9'; buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 160: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '0'; break;
							case 161: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '1'; break;
							case 162: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '2'; break;
							case 163: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '3'; break;
							case 164: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '4'; break;
							case 165: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '5'; break;
							case 166: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '6'; break;
							case 167: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '7'; break;
							case 168: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '8'; break;
							case 169: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = '9'; break;
							case 170: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 171: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 172: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 173: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 174: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 175: buf[bufOffset] = (isCapital ? 'A' : 'a'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 176: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '0'; break;
							case 177: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '1'; break;
							case 178: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '2'; break;
							case 179: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '3'; break;
							case 180: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '4'; break;
							case 181: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '5'; break;
							case 182: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '6'; break;
							case 183: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '7'; break;
							case 184: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '8'; break;
							case 185: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = '9'; break;
							case 186: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 187: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 188: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 189: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 190: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 191: buf[bufOffset] = (isCapital ? 'B' : 'b'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 192: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '0'; break;
							case 193: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '1'; break;
							case 194: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '2'; break;
							case 195: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '3'; break;
							case 196: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '4'; break;
							case 197: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '5'; break;
							case 198: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '6'; break;
							case 199: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '7'; break;
							case 200: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '8'; break;
							case 201: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = '9'; break;
							case 202: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 203: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 204: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 205: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 206: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 207: buf[bufOffset] = (isCapital ? 'C' : 'c'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 208: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '0'; break;
							case 209: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '1'; break;
							case 210: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '2'; break;
							case 211: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '3'; break;
							case 212: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '4'; break;
							case 213: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '5'; break;
							case 214: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '6'; break;
							case 215: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '7'; break;
							case 216: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '8'; break;
							case 217: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = '9'; break;
							case 218: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 219: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 220: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 221: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 222: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 223: buf[bufOffset] = (isCapital ? 'D' : 'd'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 224: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '0'; break;
							case 225: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '1'; break;
							case 226: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '2'; break;
							case 227: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '3'; break;
							case 228: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '4'; break;
							case 229: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '5'; break;
							case 230: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '6'; break;
							case 231: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '7'; break;
							case 232: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '8'; break;
							case 233: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = '9'; break;
							case 234: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 235: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 236: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 237: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 238: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 239: buf[bufOffset] = (isCapital ? 'E' : 'e'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
							case 240: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '0'; break;
							case 241: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '1'; break;
							case 242: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '2'; break;
							case 243: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '3'; break;
							case 244: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '4'; break;
							case 245: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '5'; break;
							case 246: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '6'; break;
							case 247: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '7'; break;
							case 248: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '8'; break;
							case 249: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = '9'; break;
							case 250: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'A' : 'a'); break;
							case 251: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'B' : 'b'); break;
							case 252: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'C' : 'c'); break;
							case 253: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'D' : 'd'); break;
							case 254: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'E' : 'e'); break;
							case 255: buf[bufOffset] = (isCapital ? 'F' : 'f'); buf[bufOffset + 1] = (isCapital ? 'F' : 'f'); break;
						}
					}

					template <typename BufType>
					static uint8_t ReadOctetHex(BufType buf, size_t bufOffset)
					{
						uint8_t res = 0;
						switch (buf[bufOffset])
						{
							case '1': res += 0x10; break;
							case '2': res += 0x20; break;
							case '3': res += 0x30; break;
							case '4': res += 0x40; break;
							case '5': res += 0x50; break;
							case '6': res += 0x60; break;
							case '7': res += 0x70; break;
							case '8': res += 0x80; break;
							case '9': res += 0x90; break;

							case 'A':
							case 'a':
								res += 0xA0;
								break;

							case 'B':
							case 'b':
								res += 0xB0;
								break;

							case 'C':
							case 'c':
								res += 0xC0;
								break;

							case 'D':
							case 'd':
								res += 0xD0;
								break;

							case 'E':
							case 'e':
								res += 0xE0;
								break;

							case 'F':
							case 'f':
								res += 0xF0;
								break;
						}
						switch (buf[bufOffset + 1])
						{
							case '1': res += 0x1; break;
							case '2': res += 0x2; break;
							case '3': res += 0x3; break;
							case '4': res += 0x4; break;
							case '5': res += 0x5; break;
							case '6': res += 0x6; break;
							case '7': res += 0x7; break;
							case '8': res += 0x8; break;
							case '9': res += 0x9; break;

							case 'A':
							case 'a':
								res += 0xA;
								break;

							case 'B':
							case 'b':
								res += 0xB;
								break;

							case 'C':
							case 'c':
								res += 0xC;
								break;

							case 'D':
							case 'd':
								res += 0xD;
								break;

							case 'E':
							case 'e':
								res += 0xE;
								break;

							case 'F':
							case 'f':
								res += 0xF;
								break;
						}
						return res;
					}
				};

				template <size_t DATA_LENGTH>
				inline std::string GetFixedLengthHexString(const std::string &binaryString, bool isCapital = false)
				{
					char hexCString[DATA_LENGTH * 2 + 1];
					for (size_t i = 0; i < DATA_LENGTH; ++i)
					{
						union
						{
							char source;
							uint8_t target;
						} a = { binaryString[i] };
						Unsafe::WriteOctetHex(hexCString, i * 2, a.target, isCapital);
					}
					hexCString[DATA_LENGTH * 2] = '\0';
					return hexCString;
				}

				template <size_t DATA_LENGTH>
				inline std::string GetFixedLengthHexString(const void *dataBuffer, bool isCapital = false)
				{
					char hexCString[DATA_LENGTH * 2 + 1];
					const uint8_t *byteBuffer = (const uint8_t *)dataBuffer;
					for (size_t i = 0; i < DATA_LENGTH; ++i)
					{
						Unsafe::WriteOctetHex(hexCString, i * 2, byteBuffer[i], isCapital);
					}
					hexCString[DATA_LENGTH * 2] = '\0';
					return hexCString;
				}
			}
		}
	}
}

#endif
