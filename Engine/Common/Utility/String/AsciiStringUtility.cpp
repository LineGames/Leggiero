////////////////////////////////////////////////////////////////////////////////
// String/AsciiStringUtility.cpp (Leggiero - Utility)
//
// ASCII String Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsciiStringUtility.h"

// Standard Library
#include <cctype>
#include <unordered_set>


namespace Leggiero
{
	namespace Utility
	{
		namespace String
		{
			//------------------------------------------------------------------------------
			const std::string ASCIIStringUtility::kEmpty;
			const char *ASCIIStringUtility::kWhitespaces = " \t\r\n\v\f";

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringReplace(const std::string &originalString, const char *toReplace, const char *replaced)
			{
				std::string workingString(originalString);

				size_t replaceLength = strlen(toReplace);
				size_t foundPos = workingString.find(toReplace);
				while (foundPos != std::string::npos)
				{
					workingString.replace(foundPos, replaceLength, replaced);
					foundPos = workingString.find(toReplace, foundPos);
				}

				return workingString;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringReplace(const std::string &originalString, const std::string &toReplace, const std::string &replaced)
			{
				std::string workingString(originalString);

				size_t replaceLength = replaced.length();
				size_t foundPos = workingString.find(toReplace);
				while (foundPos != std::string::npos)
				{
					workingString.replace(foundPos, replaceLength, replaced);
					foundPos = workingString.find(toReplace, foundPos);
				}

				return workingString;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::FilterString(const std::string &originalString, const char *filteringCharacters, const char *replaceChar)
			{
				std::string filteredString;
				filteredString.reserve(originalString.length() + 1);

				if (filteringCharacters == nullptr)
				{
					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (originalString[i] != '\0' && !isspace(originalString[i]))
						{
							filteredString += originalString[i];
						}
						else if (replaceChar != nullptr)
						{
							filteredString += (*replaceChar);
						}
					}
				}
				else
				{
					std::unordered_set<char> filteringCharacterSet;
					size_t characterCount = strlen(filteringCharacters);
					for (size_t i = 0; i < characterCount; ++i)
					{
						filteringCharacterSet.insert(filteringCharacters[i]);
					}

					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (filteringCharacterSet.find(originalString[i]) != filteringCharacterSet.end())
						{
							filteredString += originalString[i];
						}
						else if (replaceChar != nullptr)
						{
							filteredString += (*replaceChar);
						}
					}
				}
				return filteredString;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::FilterString(const std::string &originalString, std::initializer_list<char> filteringCharacters, const char *replaceChar)
			{
				std::string filteredString;
				filteredString.reserve(originalString.length() + 1);

				std::unordered_set<char> filteringCharacterSet(filteringCharacters);
				for (size_t i = 0; i < originalString.size(); ++i)
				{
					if (filteringCharacterSet.find(originalString[i]) != filteringCharacterSet.end())
					{
						filteredString += originalString[i];
					}
					else if (replaceChar != nullptr)
					{
						filteredString += (*replaceChar);
					}
				}
				return filteredString;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringLTrim(const std::string &originalString, const char *trimmingCharacters)
			{
				if (trimmingCharacters == nullptr)
				{
					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (originalString[i] != '\0' && !isspace(originalString[i]))
						{
							return std::string(originalString.begin() + i, originalString.end());
						}
					}
				}
				else
				{
					std::unordered_set<char> trimmingCharacterSet;
					size_t characterCount = strlen(trimmingCharacters);
					for (size_t i = 0; i < characterCount; ++i)
					{
						trimmingCharacterSet.insert(trimmingCharacters[i]);
					}

					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (trimmingCharacterSet.find(originalString[i]) != trimmingCharacterSet.end())
						{
							return std::string(originalString.begin() + i, originalString.end());
						}
					}
				}
				return kEmpty;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringLTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters)
			{
				std::unordered_set<char> trimmingCharacterSet(trimmingCharacters);
				for (size_t i = 0; i < originalString.size(); ++i)
				{
					if (trimmingCharacterSet.find(originalString[i]) != trimmingCharacterSet.end())
					{
						return std::string(originalString.begin() + i, originalString.end());
					}
				}
				return kEmpty;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringRTrim(const std::string &originalString, const char *trimmingCharacters)
			{
				if (trimmingCharacters == nullptr)
				{
					size_t originalLength = originalString.size();
					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (originalString[originalLength - i - 1] != '\0' && !isspace(originalString[originalLength - i - 1]))
						{
							return std::string(originalString.begin(), originalString.begin() + (originalLength - i));
						}
					}
				}
				else
				{
					std::unordered_set<char> trimmingCharacterSet;
					size_t characterCount = strlen(trimmingCharacters);
					for (size_t i = 0; i < characterCount; ++i)
					{
						trimmingCharacterSet.insert(trimmingCharacters[i]);
					}

					size_t originalLength = originalString.size();
					for (size_t i = 0; i < originalString.size(); ++i)
					{
						if (trimmingCharacterSet.find(originalString[originalLength - i - 1]) != trimmingCharacterSet.end())
						{
							return std::string(originalString.begin(), originalString.begin() + (originalLength - i));
						}
					}
				}
				return kEmpty;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringRTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters)
			{
				std::unordered_set<char> trimmingCharacterSet(trimmingCharacters);
				size_t originalLength = originalString.size();
				for (size_t i = 0; i < originalString.size(); ++i)
				{
					if (trimmingCharacterSet.find(originalString[originalLength - i - 1]) != trimmingCharacterSet.end())
					{
						return std::string(originalString.begin(), originalString.begin() + (originalLength - i));
					}
				}
				return kEmpty;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringTrim(const std::string &originalString, const char *trimmingCharacters)
			{
				return StringLTrim(StringRTrim(originalString, trimmingCharacters), trimmingCharacters);
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::StringTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters)
			{
				return StringLTrim(StringRTrim(originalString, trimmingCharacters), trimmingCharacters);
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::ToLower(const std::string &originalString)
			{
				std::string workingString(originalString);
				const size_t stringLength = workingString.length();

				for (size_t i = 0; i < stringLength; ++i)
				{
					switch (workingString[i])
					{
						case 'A': workingString[i] = 'a'; break;
						case 'B': workingString[i] = 'b'; break;
						case 'C': workingString[i] = 'c'; break;
						case 'D': workingString[i] = 'd'; break;
						case 'E': workingString[i] = 'e'; break;
						case 'F': workingString[i] = 'f'; break;
						case 'G': workingString[i] = 'g'; break;
						case 'H': workingString[i] = 'h'; break;
						case 'I': workingString[i] = 'i'; break;
						case 'J': workingString[i] = 'j'; break;
						case 'K': workingString[i] = 'k'; break;
						case 'L': workingString[i] = 'l'; break;
						case 'M': workingString[i] = 'm'; break;
						case 'N': workingString[i] = 'n'; break;
						case 'O': workingString[i] = 'o'; break;
						case 'P': workingString[i] = 'p'; break;
						case 'Q': workingString[i] = 'q'; break;
						case 'R': workingString[i] = 'r'; break;
						case 'S': workingString[i] = 's'; break;
						case 'T': workingString[i] = 't'; break;
						case 'U': workingString[i] = 'u'; break;
						case 'V': workingString[i] = 'v'; break;
						case 'W': workingString[i] = 'w'; break;
						case 'X': workingString[i] = 'x'; break;
						case 'Y': workingString[i] = 'y'; break;
						case 'Z': workingString[i] = 'z'; break;
					}
				}

				return workingString;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::ToUpper(const std::string &originalString)
			{
				std::string workingString(originalString);
				const size_t stringLength = workingString.length();

				for (size_t i = 0; i < stringLength; ++i)
				{
					switch (workingString[i])
					{
						case 'a': workingString[i] = 'A'; break;
						case 'b': workingString[i] = 'B'; break;
						case 'c': workingString[i] = 'C'; break;
						case 'd': workingString[i] = 'D'; break;
						case 'e': workingString[i] = 'E'; break;
						case 'f': workingString[i] = 'F'; break;
						case 'g': workingString[i] = 'G'; break;
						case 'h': workingString[i] = 'H'; break;
						case 'i': workingString[i] = 'I'; break;
						case 'j': workingString[i] = 'J'; break;
						case 'k': workingString[i] = 'K'; break;
						case 'l': workingString[i] = 'L'; break;
						case 'm': workingString[i] = 'M'; break;
						case 'n': workingString[i] = 'N'; break;
						case 'o': workingString[i] = 'O'; break;
						case 'p': workingString[i] = 'P'; break;
						case 'q': workingString[i] = 'Q'; break;
						case 'r': workingString[i] = 'R'; break;
						case 's': workingString[i] = 'S'; break;
						case 't': workingString[i] = 'T'; break;
						case 'u': workingString[i] = 'U'; break;
						case 'v': workingString[i] = 'V'; break;
						case 'w': workingString[i] = 'W'; break;
						case 'x': workingString[i] = 'X'; break;
						case 'y': workingString[i] = 'Y'; break;
						case 'z': workingString[i] = 'Z'; break;
					}
				}

				return workingString;
			}

			//------------------------------------------------------------------------------
			bool ASCIIStringUtility::IsSameStringNonCaseSensitive(const std::string &string1, const std::string &string2)
			{
				size_t length1 = string1.length();
				size_t length2 = string2.length();
				if (length1 != length2)
				{
					return false;
				}

				for (size_t i = 0; i < length1; ++i)
				{
					if (tolower(string1[i]) != tolower(string2[i]))
					{
						return false;
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			bool ASCIIStringUtility::IsStartWith(const std::string &stringToCheck, const std::string &startString)
			{
				size_t checkLength = startString.length();
				if (checkLength == 0)
				{
					return true;
				}
				if (stringToCheck.length() < checkLength)
				{
					return false;
				}

				for (size_t i = 0; i < checkLength; ++i)
				{
					if (stringToCheck[i] != startString[i])
					{
						return false;
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			bool ASCIIStringUtility::IsStartWithNonCaseSensitive(const std::string &stringToCheck, const std::string &startString)
			{
				size_t checkLength = startString.length();
				if (checkLength == 0)
				{
					return true;
				}
				if (stringToCheck.length() < checkLength)
				{
					return false;
				}

				for (size_t i = 0; i < checkLength; ++i)
				{
					if (tolower(stringToCheck[i]) != tolower(startString[i]))
					{
						return false;
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			bool ASCIIStringUtility::IsFinishWith(const std::string &stringToCheck, const std::string &finishString)
			{
				size_t checkLength = finishString.length();
				if (checkLength == 0)
				{
					return true;
				}
				size_t stringLength = stringToCheck.length();
				if (stringLength < checkLength)
				{
					return false;
				}

				for (size_t i = 1; i <= checkLength; ++i)
				{
					if (stringToCheck[stringLength - i] != finishString[checkLength - i])
					{
						return false;
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			bool ASCIIStringUtility::IsFinishWithNonCaseSensitive(const std::string &stringToCheck, const std::string &finishString)
			{
				size_t checkLength = finishString.length();
				if (checkLength == 0)
				{
					return true;
				}
				size_t stringLength = stringToCheck.length();
				if (stringLength < checkLength)
				{
					return false;
				}

				for (size_t i = 1; i <= checkLength; ++i)
				{
					if (tolower(stringToCheck[stringLength - i]) != tolower(finishString[checkLength - i]))
					{
						return false;
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			std::vector<std::string> ASCIIStringUtility::Tokenize(const std::string &originalString, const char *delimiters, bool isIgnoreEmpty)
			{
				if (originalString.empty())
				{
					return std::vector<std::string>();
				}

				std::vector<std::string> tokenized;
				std::vector<char> tokenBuffer; tokenBuffer.reserve(originalString.length());

				std::unordered_set<char> delimeterCharacterSet;
				size_t characterCount = strlen(delimiters);
				for (size_t i = 0; i < characterCount; ++i)
				{
					delimeterCharacterSet.insert(delimiters[i]);
				}

				size_t originalStringLength = originalString.length();
				for (size_t i = 0; i < originalStringLength; ++i)
				{
					if (delimeterCharacterSet.find(originalString[i]) != delimeterCharacterSet.end())
					{
						if (!isIgnoreEmpty || !tokenBuffer.empty())
						{
							tokenized.push_back(std::string(&tokenBuffer[0], tokenBuffer.size()));
							tokenBuffer.clear();
						}
					}
					else
					{
						tokenBuffer.push_back(originalString[i]);
					}
				}
				if (!isIgnoreEmpty || !tokenBuffer.empty())
				{
					tokenized.push_back(std::string(&tokenBuffer[0], tokenBuffer.size()));
				}

				return tokenized;
			}

			//------------------------------------------------------------------------------
			std::vector<std::string> ASCIIStringUtility::Tokenize(const std::string &originalString, std::initializer_list<char> delimiters, bool isIgnoreEmpty)
			{
				if (originalString.empty())
				{
					return std::vector<std::string>();
				}

				std::vector<std::string> tokenized;
				std::vector<char> tokenBuffer; tokenBuffer.reserve(originalString.length());

				std::unordered_set<char> delimeterCharacterSet(delimiters);

				size_t originalStringLength = originalString.length();
				for (size_t i = 0; i < originalStringLength; ++i)
				{
					if (delimeterCharacterSet.find(originalString[i]) != delimeterCharacterSet.end())
					{
						if (!isIgnoreEmpty || !tokenBuffer.empty())
						{
							tokenized.push_back(std::string(&tokenBuffer[0], tokenBuffer.size()));
							tokenBuffer.clear();
						}
					}
					else
					{
						tokenBuffer.push_back(originalString[i]);
					}
				}
				if (!isIgnoreEmpty || !tokenBuffer.empty())
				{
					tokenized.push_back(std::string(&tokenBuffer[0], tokenBuffer.size()));
				}

				return tokenized;
			}

			//------------------------------------------------------------------------------
			std::vector<std::string> ASCIIStringUtility::TokenizeByString(const std::string &originalString, const std::string &delimiterString, bool isIgnoreEmpty)
			{
				if (originalString.empty())
				{
					return std::vector<std::string>();
				}
				if (delimiterString.empty())
				{
					return std::vector<std::string>({ originalString });
				}
				if (delimiterString.length() == 1)
				{
					return Tokenize(originalString, delimiterString.c_str(), isIgnoreEmpty);
				}

				std::vector<std::string> tokenized;
				std::vector<char> tokenBuffer; tokenBuffer.reserve(originalString.length());

				size_t originalStringLength = originalString.length();
				for (size_t i = 0; i < originalStringLength; ++i)
				{
					tokenBuffer.push_back(originalString[i]);

					bool isDelimiterFound = true;
					if (tokenBuffer.size() >= delimiterString.length())
					{
						for (size_t j = 0; j < delimiterString.length(); ++j)
						{
							if (tokenBuffer[tokenBuffer.size() - j - 1] != delimiterString[delimiterString.length() - j - 1])
							{
								isDelimiterFound = false;
								break;
							}
						}
					}
					else
					{
						isDelimiterFound = false;
					}

					if (isDelimiterFound)
					{
						std::string tokenString(&tokenBuffer[0], tokenBuffer.size() - delimiterString.length());
						if (!isIgnoreEmpty || !tokenString.empty())
						{
							tokenized.push_back(tokenString);
						}
						tokenBuffer.clear();
					}
				}
				if (!isIgnoreEmpty || !tokenBuffer.empty())
				{
					tokenized.push_back(std::string(&tokenBuffer[0], tokenBuffer.size()));
				}

				return tokenized;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::Join(const std::vector<std::string> &stringsToJoin, const std::string &separator, bool isTrailingSeparator, bool isAheadingSeparator)
			{
				if (stringsToJoin.empty())
				{
					return kEmpty;
				}

				const size_t separatorLength = separator.length();

				size_t bufferSize = 0;
				for (const std::string &currentString : stringsToJoin)
				{
					bufferSize += currentString.length();
				}
				bufferSize += separatorLength * (stringsToJoin.size() - 1);
				if (isTrailingSeparator)
				{
					bufferSize += separatorLength;
				}
				if (isAheadingSeparator)
				{
					bufferSize += separatorLength;
				}

				std::string resultBuffer(bufferSize, '\0');

				size_t currentPointer = 0;
				if (isAheadingSeparator)
				{
					for (size_t i = 0; i < separatorLength; ++i)
					{
						resultBuffer[currentPointer + i] = separator[i];
					}
					currentPointer += separatorLength;
				}

				bool isFirst = true;
				for (const std::string &currentString : stringsToJoin)
				{
					if (isFirst)
					{
						isFirst = false;
					}
					else
					{
						for (size_t i = 0; i < separatorLength; ++i)
						{
							resultBuffer[currentPointer + i] = separator[i];
						}
						currentPointer += separatorLength;
					}

					const size_t currentStringLength = currentString.length();
					for (size_t i = 0; i < currentStringLength; ++i)
					{
						resultBuffer[currentPointer + i] = currentString[i];
					}
					currentPointer += currentStringLength;
				}

				if (isTrailingSeparator)
				{
					for (size_t i = 0; i < separatorLength; ++i)
					{
						resultBuffer[currentPointer + i] = separator[i];
					}
					currentPointer += separatorLength;
				}

				return resultBuffer;
			}

			//------------------------------------------------------------------------------
			std::string ASCIIStringUtility::JoinWithBuffer(const std::vector<std::string> &stringsToJoin, std::vector<char> &buffer, const std::string &separator, bool isTrailingSeparator, bool isAheadingSeparator)
			{
				if (stringsToJoin.empty())
				{
					return kEmpty;
				}

				const size_t separatorLength = separator.length();

				size_t bufferSize = 0;
				for (const std::string &currentString : stringsToJoin)
				{
					bufferSize += currentString.length();
				}
				bufferSize += separatorLength * (stringsToJoin.size() - 1);
				if (isTrailingSeparator)
				{
					bufferSize += separatorLength;
				}
				if (isAheadingSeparator)
				{
					bufferSize += separatorLength;
				}

				buffer.resize(bufferSize);

				size_t currentPointer = 0;
				if (isAheadingSeparator && separatorLength > 0)
				{
					memcpy(&buffer[currentPointer], separator.c_str(), separatorLength);
					currentPointer += separatorLength;
				}

				bool isFirst = true;
				for (const std::string &currentString : stringsToJoin)
				{
					if (isFirst)
					{
						isFirst = false;
					}
					else
					{
						if (separatorLength > 0)
						{
							memcpy(&buffer[currentPointer], separator.c_str(), separatorLength);
							currentPointer += separatorLength;
						}
					}

					const size_t currentStringLength = currentString.length();
					memcpy(&buffer[currentPointer], currentString.c_str(), currentStringLength);
					currentPointer += currentStringLength;
				}

				if (isTrailingSeparator && separatorLength > 0)
				{
					memcpy(&buffer[currentPointer], separator.c_str(), separatorLength);
					currentPointer += separatorLength;
				}

				return std::string(&buffer[0], bufferSize);;
			}
		}
	}
}
