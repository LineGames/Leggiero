////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionColor.h (Leggiero/Modules - LegacyUI)
//
// UI Description about Color
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_COLOR_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_COLOR_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Reader
			{
				ColorARGBValueType ReadColorValueFromCString(const char *stringValue);


				namespace XML
				{
					// Read Color Value from an XML Element
					ColorARGBValueType ReadColorFromElement(ParsingXMLElementType *elem);
				}
			}
		}
	}
}

#endif
