////////////////////////////////////////////////////////////////////////////////
// String/AsciiStringUtility.h (Leggiero - Utility)
//
// ASCII C++ String Utilities
//
// Just basic utilities for the engine. Not supporting Unicode and complex
// functions. Although there are many good string libraries for C++, we
// reinvented a wheel to avoid embeding such a huge stuffs with those libraries.
// We cannot ensure the performance of this library(maybe poor), but is is
// enough for many non-performance-critical parts.
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__STRING__ASCII_STRING_UTILITY_H
#define __UTILITY__STRING__ASCII_STRING_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <initializer_list>
#include <string>
#include <vector>


namespace Leggiero
{
	namespace Utility
	{
		namespace String
		{
			// ASCII String Utilities
			class ASCIIStringUtility
			{
			public:	// Types
				using ProcessingString = std::string;
				using ProcessingChar = char;

			public:	// Constants
				static constexpr char kNULL = '\0';
				static const std::string kEmpty;
				static const char *kWhitespaces;

			public:	// Replace
				static std::string StringReplace(const std::string &originalString, const char *toReplace, const char *replaced);
				static std::string StringReplace(const std::string &originalString, const std::string &toReplace, const std::string &replaced);

				static std::string FilterString(const std::string &originalString, const char *filteringCharacters = nullptr, const char *replaceChar = nullptr);
				static std::string FilterString(const std::string &originalString, std::initializer_list<char> filteringCharacters, const char *replaceChar = nullptr);

			public:	// Trimming
				static std::string StringLTrim(const std::string &originalString, const char *trimmingCharacters = nullptr);
				static std::string StringLTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters);
				static std::string StringRTrim(const std::string &originalString, const char *trimmingCharacters = nullptr);
				static std::string StringRTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters);
				static std::string StringTrim(const std::string &originalString, const char *trimmingCharacters = nullptr);
				static std::string StringTrim(const std::string &originalString, std::initializer_list<char> trimmingCharacters);

			public:	// Case
				static std::string ToLower(const std::string &originalString);
				static std::string ToUpper(const std::string &originalString);

			public:	// Compare
				static bool IsSameStringNonCaseSensitive(const std::string &string1, const std::string &string2);

				static bool IsStartWith(const std::string &stringToCheck, const std::string &startString);
				static bool IsStartWithNonCaseSensitive(const std::string &stringToCheck, const std::string &startString);

				static bool IsFinishWith(const std::string &stringToCheck, const std::string &finishString);
				static bool IsFinishWithNonCaseSensitive(const std::string &stringToCheck, const std::string &finishString);

			public:	// Tokenize
				static std::vector<std::string> Tokenize(const std::string &originalString, const char *delimiters = nullptr, bool isIgnoreEmpty = false);
				static std::vector<std::string> Tokenize(const std::string &originalString, std::initializer_list<char> delimiters, bool isIgnoreEmpty = false);
				static std::vector<std::string> TokenizeByString(const std::string &originalString, const std::string &delimiterString, bool isIgnoreEmpty = false);

			public:	// Join
				static std::string Join(const std::vector<std::string> &stringsToJoin, const std::string &separator = kEmpty, bool isTrailingSeparator = false, bool isAheadingSeparator = false);
				static std::string JoinWithBuffer(const std::vector<std::string> &stringsToJoin, std::vector<char> &buffer, const std::string &separator = kEmpty, bool isTrailingSeparator = false, bool isAheadingSeparator = false);
			};
		}
	}
}

#endif
