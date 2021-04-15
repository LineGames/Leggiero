////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionReader.h (Leggiero/Modules - LegacyUI)
//
// Parsing utility function for UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_READER_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_READER_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Reader
			{
				namespace XML
				{
					// Read object name from given XML Element
					VariableNameType ReadObjectNameFromElement(ParsingXMLElementType *elem);

					// Read a description object from given XML Element
					std::shared_ptr<IDescriptionObject> ReadObjectFromElement(ParsingXMLElementType *elem, DescriptionManager &loadingManager);
				}
			}
		}
	}
}

#endif
