////////////////////////////////////////////////////////////////////////////////
// DeviceCommonTypes.h (Leggiero/Modules - Application)
//
// Device/Platform Related Common Type Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__DEVICE_COMMON_TYPES_H
#define __LM_APPLICATION__DEVICE_COMMON_TYPES_H


// Leggiero.Utility
#include <Utility/Math/Vector.h>


namespace Leggiero
{
	namespace Application
	{
		using DeviceTouchIdType = unsigned long;

		using DeviceScreenCoordType = float;
		using DeviceScreenPixelCoordType = int;

		using DeviceScreenPosition = Utility::Math::Vector2D<DeviceScreenCoordType>;
	}
}

#endif
