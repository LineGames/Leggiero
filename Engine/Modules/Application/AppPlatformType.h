////////////////////////////////////////////////////////////////////////////////
// AppPlatformType.h (Leggiero/Modules - Application)
//
// Definition of Application Platform Type
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__APP_PLATFORM_TYPE_H
#define __LM_APPLICATION__APP_PLATFORM_TYPE_H



namespace Leggiero
{
	namespace Application
	{
		// Application Platform Type
		enum class AppPlatformType
		{
			kINVALID, 

			kIOS, 
			kAndroid, 
			kWinPC, 
		};
	}
}

#endif
