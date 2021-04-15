////////////////////////////////////////////////////////////////////////////////
// TouchInputComponent.h (Leggiero/Modules - Input)
//
// Engine Component to Handle Touch Inputs
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_INPUT__TOUCH_INPUT_COMPONENT_H
#define __LM_INPUT__TOUCH_INPUT_COMPONENT_H


// Standard Library
#include <set>

// Leggiero.Utility
#include <Utility/Sugar/EventNotifier.h>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.Application
#include <Application/RawTouchEventObserver.h>

// Leggiero.Input
#include "InputCommonTypes.h"


namespace Leggiero
{
	// Forward Declarations for Dependent Components
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Input
	{
		// Forward Declarations
		class IImmediateTouchInputEventObserver;


		// Touch Input Component
		class TouchInputComponent
			: public EngineComponent
			, public Application::IRawTouchInputEventObserver
		{
		public:
			TouchInputComponent();
			virtual ~TouchInputComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kTouchInput; };

			// Initialize the Component
			virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

			// Safely Shutdown Component
			virtual void ShutdownComponent() override;

			// Get type Id list of dependent components needed by this component
			virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

		public:	// IRawTouchInputEventObserver
			virtual void OnTouchDown(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) override;
			virtual void OnTouchMoved(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) override;
			virtual void OnTouchUp(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) override;
			virtual void OnTouchCanceled(Application::DeviceTouchIdType touchId, GameTimeClockType::time_point eventTime) override;

		public:	// State Interface
			bool IsTouchDowned(TouchIdType touchId);
			
		public:	// Immediate Event Notify Interface
			void RegisterImmediateEventObserver(IImmediateTouchInputEventObserver *observer);
			void UnRegisterImmediateEventObserver(IImmediateTouchInputEventObserver *observer);

		protected:
			Application::ApplicationComponent	*m_applicationComponent;

		protected:
			std::set<TouchIdType> m_downedTouches;

			Utility::DesignPattern::EventNotifier<IImmediateTouchInputEventObserver *> m_immediateEventNotifier;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Input::TouchInputComponent, Leggiero::EngineComponentIdType::kTouchInput);

#endif
