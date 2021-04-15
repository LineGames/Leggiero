////////////////////////////////////////////////////////////////////////////////
// Description/UIPrefabDescription_Reader.h (Leggiero/Modules - LegacyUI)
//
// Parsing utility function for UI Description of Prefabs
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_PREFAB_DESCRIPTION_READER_H
#define __LM_LUI__DESCRIPTION__UI_PREFAB_DESCRIPTION_READER_H


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
			namespace Prefab
			{
				// Forward Declarations
				class IUIObjectPrefab;
				class IUIComponentPrefab;


				namespace Reader
				{
					namespace XML
					{
						// Read an Object Prefab from given XML Element
						std::shared_ptr<IUIObjectPrefab> ReadObjectPrefabFromElement(ParsingXMLElementType *elem);

						// Read a Component Prefab from given XML Element
						std::shared_ptr<IUIComponentPrefab> ReadComponentPrefabFromElement(ParsingXMLElementType *elem);
					}
				}
			}
		}
	}
}

#endif
