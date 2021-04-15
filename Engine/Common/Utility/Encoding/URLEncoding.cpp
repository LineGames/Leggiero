////////////////////////////////////////////////////////////////////////////////
// Encoding/URLEncoding.cpp (Leggiero - Utility)
//
// URL Encoding Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "URLEncoding.h"

// Standard Library
#include <cstdint>


namespace Leggiero
{
	namespace Utility
	{
		namespace Encoding
		{
			namespace URL
			{
				namespace _Internal
				{
					//------------------------------------------------------------------------------
					static const char s_normalizedNewline[] = "%0D%0A";

					//------------------------------------------------------------------------------
					// Implement as table search, because this will be the fastest
					const char *GetOctetEncoded(char original, bool isXFromsSpec)
					{
						union
						{
							char source;
							uint8_t target;
						} a = { original };
                        switch (a.target)
                        {
							case 0: return "%00";
							case 1: return "%01";
							case 2: return "%02";
							case 3: return "%03";
							case 4: return "%04";
							case 5: return "%05";
							case 6: return "%06";
							case 7: return "%07";
							case 8: return "%08";
							case 9: return "%09";
							case 10: return "%0A";
							case 11: return "%0B";
							case 12: return "%0C";
							case 13: return "%0D";
							case 14: return "%0E";
							case 15: return "%0F";
							case 16: return "%10";
							case 17: return "%11";
							case 18: return "%12";
							case 19: return "%13";
							case 20: return "%14";
							case 21: return "%15";
							case 22: return "%16";
							case 23: return "%17";
							case 24: return "%18";
							case 25: return "%19";
							case 26: return "%1A";
							case 27: return "%1B";
							case 28: return "%1C";
							case 29: return "%1D";
							case 30: return "%1E";
							case 31: return "%1F";

							case 32:
								{
									if (isXFromsSpec)
									{
										return "+";
									}
									else
									{
										return "%20";
									}
								}

							case 33: return "%21";
							case 34: return "%22";
							case 35: return "%23";
							case 36: return "%24";
							case 37: return "%25";
							case 38: return "%26";
							case 39: return "%27";
							case 40: return "%28";
							case 41: return "%29";
							case 42: return "%2A";
							case 43: return "%2B";
							case 44: return "%2C";

							case 45: return "-";
							case 46: return ".";

							case 47: return "%2F";

							case 48: return "0";
							case 49: return "1";
							case 50: return "2";
							case 51: return "3";
							case 52: return "4";
							case 53: return "5";
							case 54: return "6";
							case 55: return "7";
							case 56: return "8";
							case 57: return "9";

							case 58: return "%3A";
							case 59: return "%3B";
							case 60: return "%3C";
							case 61: return "%3D";
							case 62: return "%3E";
							case 63: return "%3F";
							case 64: return "%40";

							case 65: return "A";
							case 66: return "B";
							case 67: return "C";
							case 68: return "D";
							case 69: return "E";
							case 70: return "F";
							case 71: return "G";
							case 72: return "H";
							case 73: return "I";
							case 74: return "J";
							case 75: return "K";
							case 76: return "L";
							case 77: return "M";
							case 78: return "N";
							case 79: return "O";
							case 80: return "P";
							case 81: return "Q";
							case 82: return "R";
							case 83: return "S";
							case 84: return "T";
							case 85: return "U";
							case 86: return "V";
							case 87: return "W";
							case 88: return "X";
							case 89: return "Y";
							case 90: return "Z";

							case 91: return "%5B";
							case 92: return "%5C";
							case 93: return "%5D";
							case 94: return "%5E";

							case 95: return "_";

							case 96: return "%60";

							case 97: return "a";
							case 98: return "b";
							case 99: return "c";
							case 100: return "d";
							case 101: return "e";
							case 102: return "f";
							case 103: return "g";
							case 104: return "h";
							case 105: return "i";
							case 106: return "j";
							case 107: return "k";
							case 108: return "l";
							case 109: return "m";
							case 110: return "n";
							case 111: return "o";
							case 112: return "p";
							case 113: return "q";
							case 114: return "r";
							case 115: return "s";
							case 116: return "t";
							case 117: return "u";
							case 118: return "v";
							case 119: return "w";
							case 120: return "x";
							case 121: return "y";
							case 122: return "z";

							case 123: return "%7B";
							case 124: return "%7C";
							case 125: return "%7D";

							case 126: return "~";

							case 127: return "%7F";
							case 128: return "%80";
							case 129: return "%81";
							case 130: return "%82";
							case 131: return "%83";
							case 132: return "%84";
							case 133: return "%85";
							case 134: return "%86";
							case 135: return "%87";
							case 136: return "%88";
							case 137: return "%89";
							case 138: return "%8A";
							case 139: return "%8B";
							case 140: return "%8C";
							case 141: return "%8D";
							case 142: return "%8E";
							case 143: return "%8F";
							case 144: return "%90";
							case 145: return "%91";
							case 146: return "%92";
							case 147: return "%93";
							case 148: return "%94";
							case 149: return "%95";
							case 150: return "%96";
							case 151: return "%97";
							case 152: return "%98";
							case 153: return "%99";
							case 154: return "%9A";
							case 155: return "%9B";
							case 156: return "%9C";
							case 157: return "%9D";
							case 158: return "%9E";
							case 159: return "%9F";
							case 160: return "%A0";
							case 161: return "%A1";
							case 162: return "%A2";
							case 163: return "%A3";
							case 164: return "%A4";
							case 165: return "%A5";
							case 166: return "%A6";
							case 167: return "%A7";
							case 168: return "%A8";
							case 169: return "%A9";
							case 170: return "%AA";
							case 171: return "%AB";
							case 172: return "%AC";
							case 173: return "%AD";
							case 174: return "%AE";
							case 175: return "%AF";
							case 176: return "%B0";
							case 177: return "%B1";
							case 178: return "%B2";
							case 179: return "%B3";
							case 180: return "%B4";
							case 181: return "%B5";
							case 182: return "%B6";
							case 183: return "%B7";
							case 184: return "%B8";
							case 185: return "%B9";
							case 186: return "%BA";
							case 187: return "%BB";
							case 188: return "%BC";
							case 189: return "%BD";
							case 190: return "%BE";
							case 191: return "%BF";
							case 192: return "%C0";
							case 193: return "%C1";
							case 194: return "%C2";
							case 195: return "%C3";
							case 196: return "%C4";
							case 197: return "%C5";
							case 198: return "%C6";
							case 199: return "%C7";
							case 200: return "%C8";
							case 201: return "%C9";
							case 202: return "%CA";
							case 203: return "%CB";
							case 204: return "%CC";
							case 205: return "%CD";
							case 206: return "%CE";
							case 207: return "%CF";
							case 208: return "%D0";
							case 209: return "%D1";
							case 210: return "%D2";
							case 211: return "%D3";
							case 212: return "%D4";
							case 213: return "%D5";
							case 214: return "%D6";
							case 215: return "%D7";
							case 216: return "%D8";
							case 217: return "%D9";
							case 218: return "%DA";
							case 219: return "%DB";
							case 220: return "%DC";
							case 221: return "%DD";
							case 222: return "%DE";
							case 223: return "%DF";
							case 224: return "%E0";
							case 225: return "%E1";
							case 226: return "%E2";
							case 227: return "%E3";
							case 228: return "%E4";
							case 229: return "%E5";
							case 230: return "%E6";
							case 231: return "%E7";
							case 232: return "%E8";
							case 233: return "%E9";
							case 234: return "%EA";
							case 235: return "%EB";
							case 236: return "%EC";
							case 237: return "%ED";
							case 238: return "%EE";
							case 239: return "%EF";
							case 240: return "%F0";
							case 241: return "%F1";
							case 242: return "%F2";
							case 243: return "%F3";
							case 244: return "%F4";
							case 245: return "%F5";
							case 246: return "%F6";
							case 247: return "%F7";
							case 248: return "%F8";
							case 249: return "%F9";
							case 250: return "%FA";
							case 251: return "%FB";
							case 252: return "%FC";
							case 253: return "%FD";
							case 254: return "%FE";
							default: return "%FF";
                        }
					}

					//------------------------------------------------------------------------------
					inline int ParseHex(char hexDigit)
					{
						switch (hexDigit)
						{
							case '0': return 0;
							case '1': return 1;
							case '2': return 2;
							case '3': return 3;
							case '4': return 4;
							case '5': return 5;
							case '6': return 6;
							case '7': return 7;
							case '8': return 8;
							case '9': return 9;

							case 'A':
							case 'a':
								return 10;

							case 'B':
							case 'b':
								return 11;

							case 'C':
							case 'c':
								return 12;

							case 'D':
							case 'd':
								return 13;

							case 'E':
							case 'e':
								return 14;

							case 'F':
							case 'f':
								return 15;
						}
						return -1;
					}

					//------------------------------------------------------------------------------
					inline bool IsHexDigit(char digit)
					{
						switch (digit)
						{
							case '0':
							case '1':
							case '2':
							case '3':
							case '4':
							case '5':
							case '6':
							case '7':
							case '8':
							case '9':
							case 'A':
							case 'a':
							case 'B':
							case 'b':
							case 'C':
							case 'c':
							case 'D':
							case 'd':
							case 'E':
							case 'e':
							case 'F':
							case 'f':
								return true;
						}
						return false;
					}

					//------------------------------------------------------------------------------
					char ParseOctet(const char *twoDigits)
					{
						uint8_t hexCode = static_cast<uint8_t>(ParseHex(twoDigits[0]) * 16 + ParseHex(twoDigits[1]));
						union
						{
							uint8_t source;
							char target;
						} a = { hexCode };
						return a.target;
					}

					//------------------------------------------------------------------------------
					void PushToBuffer(EncodeBufferType &buffer, const char *str)
					{
						size_t offset = 0;
						while (true)
						{
							if (str[offset] == '\0')
							{
								break;
							}
						}
						if (offset > 0)
						{
							buffer.insert(buffer.end(), str, str + offset);
						}
					}
				}

				//------------------------------------------------------------------------------
				std::string Encode(const std::string &original, bool isXFromsSpec)
				{
					EncodeBufferType tempBuffer;
					tempBuffer.reserve(original.size() * 3 / 2);	// Reserve 1.5 times
					return EncodeInBuf(tempBuffer, original, isXFromsSpec);
				}

				//------------------------------------------------------------------------------
				std::string Decode(const std::string &urlEncoded)
				{
					EncodeBufferType tempBuffer;
					tempBuffer.reserve(urlEncoded.size());
					return DecodeInBuf(tempBuffer, urlEncoded);
				}

				//------------------------------------------------------------------------------
				std::string EncodeInBuf(EncodeBufferType &buffer, const std::string &original, bool isXFromsSpec)
				{
					const size_t srcLength = original.length();
					for (size_t i = 0; i < srcLength; ++i)
					{
						char currentChar = original[i];

						if (isXFromsSpec)
						{
							// Newline character normalization
							if (currentChar == '\n')
							{
								_Internal::PushToBuffer(buffer, _Internal::s_normalizedNewline);
								continue;
							}
							else if (currentChar == '\r')
							{
								if ((i + 1 < srcLength) && original[i + 1] == '\n')
								{
									++i;
								}
								_Internal::PushToBuffer(buffer, _Internal::s_normalizedNewline);
								continue;
							}
						}

						_Internal::PushToBuffer(buffer, _Internal::GetOctetEncoded(currentChar, isXFromsSpec));
					}

					return std::string(&buffer[0], buffer.size());
				}

				//------------------------------------------------------------------------------
				std::string DecodeInBuf(EncodeBufferType &buffer, const std::string &urlEncoded)
				{
					const size_t encodedLength = urlEncoded.length();
					char octetBuffer[2];
					int octetParseState = 0;
					for (size_t i = 0; i < encodedLength; ++i)
					{
						char currentChar = urlEncoded[i];

						bool isProcessed = false;
						if (octetParseState > 0)
						{
							if (currentChar == '%')
							{
								// Ill-Formed. But, just ignore prior escaped characters
								octetParseState = 1;
								isProcessed = true;
							}
							else if (_Internal::IsHexDigit(currentChar))
							{
								octetBuffer[octetParseState - 1] = currentChar;
								if (octetParseState == 2)
								{
									// Filled Octet
									buffer.push_back(_Internal::ParseOctet(octetBuffer));
									octetParseState = 0;
								}
								else
								{
									++octetParseState;
								}
								isProcessed = true;
							}
							else
							{
								// Ill-Formed. Ignore current escape
								octetParseState = 0;
							}
						}

						if (!isProcessed)
						{
							switch (currentChar)
							{
								case '%':
									octetParseState = 1;
									break;

								case '+':
									// For application/x-www-form-urlencoded, it was originally space(' '), 
									// for current spec(RFC 3986), it had to been octed encoded, so we just consider '+' as space(' ')
									buffer.push_back(' ');
									break;

								case 'A':
								case 'B':
								case 'C':
								case 'D':
								case 'E':
								case 'F':
								case 'G':
								case 'H':
								case 'I':
								case 'J':
								case 'K':
								case 'L':
								case 'M':
								case 'N':
								case 'O':
								case 'P':
								case 'Q':
								case 'R':
								case 'S':
								case 'T':
								case 'U':
								case 'V':
								case 'W':
								case 'X':
								case 'Y':
								case 'Z':
								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
								case 'g':
								case 'h':
								case 'i':
								case 'j':
								case 'k':
								case 'l':
								case 'm':
								case 'n':
								case 'o':
								case 'p':
								case 'q':
								case 'r':
								case 's':
								case 't':
								case 'u':
								case 'v':
								case 'w':
								case 'x':
								case 'y':
								case 'z':
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
								case '-':
								case '_':
								case '.':
								case '~':
									// Unreserved Characters. Good Cases
									buffer.push_back(currentChar);
									break;

								case '!':
								case '#':
								case '$':
								case '&':
								case '\'':
								case '(':
								case ')':
								case '*':
								case ',':
								case '/':
								case ':':
								case ';':
								case '=':
								case '?':
								case '@':
								case '[':
								case ']':
									// Reserved Characters('+' have been replaced by ' ')
									// Actually, these are ill-formed URL Encoded string, but just put it
									buffer.push_back(currentChar);
									break;

								default:
									// Other characters
									buffer.push_back(currentChar);
							}
						}
					}

					return std::string(&buffer[0], buffer.size());
				}
			}
		}
	}
}
