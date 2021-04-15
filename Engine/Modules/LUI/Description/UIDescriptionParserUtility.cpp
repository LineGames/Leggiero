////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionParserUtility.cpp (Leggiero/Modules - LegacyUI)
//
// Parsing Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionParserUtility.h"

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Parsing
			{
				//------------------------------------------------------------------------------
				std::tuple<bool, std::string, std::string> ParsingUtility::ExtractFunctionStyleCall(const std::string &expressionStr)
				{
					int parenthesesNumber = 0;
					int firstParenthesesPosition = -1;
					int firstParenthesesFinishPosition = -1;
					for (size_t i = 0; i < expressionStr.length(); ++i)
					{
						if (expressionStr[i] == '(')
						{
							++parenthesesNumber;
							if (parenthesesNumber == 1 && firstParenthesesPosition == -1)
							{
								firstParenthesesPosition = static_cast<int>(i);
							}
						}
						else if (expressionStr[i] == ')')
						{
							--parenthesesNumber;
							if (parenthesesNumber < 0)
							{
								// Ill-formed
								return std::make_tuple(false, std::string(), std::string());
							}
							if (parenthesesNumber == 0 && firstParenthesesFinishPosition == -1)
							{
								firstParenthesesFinishPosition = static_cast<int>(i);
							}
						}
					}

					if (firstParenthesesPosition != -1)
					{
						if (firstParenthesesFinishPosition == -1)
						{
							// Ill-formed
							return std::make_tuple(false, std::string(), std::string());
						}

						std::string constructorString(Utility::String::ASCIIStringUtility::StringTrim(expressionStr.substr(0, firstParenthesesPosition)));
						std::string argumentString(Utility::String::ASCIIStringUtility::StringTrim(expressionStr.substr(firstParenthesesPosition + 1, firstParenthesesFinishPosition - firstParenthesesPosition - 1)));

						return std::make_tuple(true, constructorString, argumentString);
					}

					// No function call
					return std::make_tuple(false, std::string(), std::string());
				}
			}
		}
	}
}
