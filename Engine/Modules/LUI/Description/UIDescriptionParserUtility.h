////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionParserUtility.h (Leggiero/Modules - LegacyUI)
//
// Parsing utility functions for UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_PARSER_UTILITY_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_PARSER_UTILITY_H


// Standard Library
#include <memory>
#include <string>
#include <tuple>
#include <vector>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Parsing
			{
				// Parsing Utility Class
				class ParsingUtility
				{
				public:
					static std::tuple<bool, std::string, std::string> ExtractFunctionStyleCall(const std::string &expressionStr);
				};
			}
		}
	}
}

#endif
