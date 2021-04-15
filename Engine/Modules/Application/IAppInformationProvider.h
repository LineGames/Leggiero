////////////////////////////////////////////////////////////////////////////////
// IAppInformationProvider.h (Leggiero/Modules - Application)
//
// Interface for Application General Information Provider Sub-Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__I_APP_INFORMATION_PROVIDER_H
#define __LM_APPLICATION__I_APP_INFORMATION_PROVIDER_H


// Standard Library
#include <string>

// Leggiero.Application
#include "AppPlatformType.h"
#include "DeviceCommonTypes.h"


namespace Leggiero
{
	namespace Application
	{
		// Application General Information Provider Interface
		class IAppInformationProvider
		{
		public:
			virtual DeviceScreenCoordType GetPixelWidth() const = 0;
			virtual DeviceScreenCoordType GetPixelHeight() const = 0;
			virtual DeviceScreenPixelCoordType GetIntegerPixelWidth() const = 0;
			virtual DeviceScreenPixelCoordType GetIntegerPixelHeight() const = 0;

			virtual AppPlatformType GetPlatformType() const = 0;

			virtual const char *GetSystemLocaleString() const = 0;

			virtual std::string GetAppReferrer() { return ""; }
		};
	}
}

#endif
