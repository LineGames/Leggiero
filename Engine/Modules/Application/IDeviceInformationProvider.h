////////////////////////////////////////////////////////////////////////////////
// IDeviceInformationProvider.h (Leggiero/Modules - Application)
//
// Interface for Device Information Provider Sub-Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__I_DEVICE_INFORMATION_PROVIDER_H
#define __LM_APPLICATION__I_DEVICE_INFORMATION_PROVIDER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Math/BasicRect.h>

// Leggiero.Application
#include "AppPlatformType.h"
#include "DeviceCommonTypes.h"


namespace Leggiero
{
	namespace Application
	{
		// Device Information Provider Interface
		class IDeviceInformationProvider
		{
		public:
			virtual DeviceScreenCoordType GetScreenPixelWidth() const = 0;
			virtual DeviceScreenCoordType GetScreenPixelHeight() const = 0;
			virtual DeviceScreenPixelCoordType GetScreenIntegerPixelWidth() const = 0;
			virtual DeviceScreenPixelCoordType GetScreenIntegerPixelHeight() const = 0;

			virtual DeviceScreenCoordType GetApproximateScreenXPPI() const = 0;
			virtual DeviceScreenCoordType GetApproximateScreenYPPI() const = 0;

			virtual Utility::Math::BasicRect<DeviceScreenCoordType> GetSafeArea() const = 0;

			virtual GameDataString GetDeviceName() const = 0;
			virtual bool IsTablet() const = 0;
		};
	}
}

#endif
