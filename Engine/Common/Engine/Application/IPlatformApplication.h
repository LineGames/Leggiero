////////////////////////////////////////////////////////////////////////////////
// Application/EngineModuleException.h (Leggiero - Engine)
//
// Common Interface for Platform(Windows, iOS, Android ..) Application
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__I_PLATFORM_APPLICATION_H
#define __ENGINE__APPLICATION__I_PLATFORM_APPLICATION_H


namespace Leggiero
{
	// Forward Declaration of Application Component
	namespace Application
	{
		class ApplicationComponent;
	}

	class EngineComponent;


	// Leggiero Platform Application Interface
	class IPlatformApplication
	{
	public:
		IPlatformApplication() { }
		virtual ~IPlatformApplication() { }

	public:
		virtual Application::ApplicationComponent *GetApplicationComponent() = 0;
		virtual EngineComponent *GetApplicationEngineComponent() = 0;
	};
}

#endif
