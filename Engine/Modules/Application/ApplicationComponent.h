////////////////////////////////////////////////////////////////////////////////
// ApplicationComponent.h (Leggiero/Modules - Application)
//
// An engine component that abstracts real platform application
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__APPLICATION_COMPONENT_H
#define __LM_APPLICATION__APPLICATION_COMPONENT_H


// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>


namespace Leggiero
{
	namespace Application
	{
		// Forward Declaration of Sub-Components
		class IAppInformationProvider;
		class IDeviceInformationProvider;
		class IPlatformAppControl;
		class SystemEventDispatcher;
		class RawTouchEventDispatcher;
		class IAppTextInputController;
		class IAppWebViewController;


		// Engine Application Component
		class ApplicationComponent : public EngineComponent
		{
		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kApplication; };

		public:
			virtual IAppInformationProvider			&AppInformation() = 0;
			virtual IDeviceInformationProvider		&DeviceInformation() = 0;

			virtual IPlatformAppControl				&PlatformAppControl() = 0;

			virtual SystemEventDispatcher			&SystemEventCenter() = 0;
			virtual RawTouchEventDispatcher			&RawTouchEventCenter() = 0;

			virtual IAppTextInputController			&TextInputController() = 0;
			virtual IAppWebViewController			&WebViewController() = 0;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Application::ApplicationComponent, Leggiero::EngineComponentIdType::kApplication);

#endif
