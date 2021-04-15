////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionSystemUnit.h (Leggiero/Modules - LegacyUI)
//
// System Unit of UI Description
// Provides standard interface for system values in UI Description.
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_SYSTEM_UNIT_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_SYSTEM_UNIT_H


// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescriptionUnit.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace LUI
	{
		namespace Description
		{
			// System Unit
			class SystemUnit
				: public DescriptionUnit
			{
			public:
				static const UnitNameType kSystemUnitName;

			public:
				SystemUnit(Application::ApplicationComponent *appComponent);
				virtual ~SystemUnit();

			public:
				void ReloadSystemValues();
				void LoadSystemValuesFrom(Application::ApplicationComponent *appComponent);

			protected:
				bool								m_isValueLoaded;
				Application::ApplicationComponent	*m_appComponent;
			};
		}
	}
}

#endif
