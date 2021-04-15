////////////////////////////////////////////////////////////////////////////////
// TouchInputComponent.cpp (Leggiero/Modules - Input)
//
// Touch Input Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TouchInputComponent.h"

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/RawTouchEventDispatcher.h>

// Leggiero.Input
#include "IImmediateTouchInputEventObserver.h"


DEFINE_GET_COMPONENT_INTERFACE(Leggiero::Input::TouchInputComponent, Leggiero::EngineComponentIdType::kTouchInput);


namespace Leggiero
{
	namespace Input
	{
		//////////////////////////////////////////////////////////////////////////////// TouchInputComponent

		//------------------------------------------------------------------------------
		TouchInputComponent::TouchInputComponent()
		{

		}

		//------------------------------------------------------------------------------
		TouchInputComponent::~TouchInputComponent()
		{

		}

		//------------------------------------------------------------------------------
		// Initialize the Component
		void TouchInputComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
		{
			m_applicationComponent->RawTouchEventCenter().RegisterRawTouchEventObserver(this);
		}

		//------------------------------------------------------------------------------
		// Safely Shutdown Component
		void TouchInputComponent::ShutdownComponent()
		{
			m_applicationComponent->RawTouchEventCenter().UnRegisterRawTouchEventObserver(this);
		}

		//------------------------------------------------------------------------------
		// Get type Id list of dependent components needed by this component
		const std::vector<EngineComponentIdType> &TouchInputComponent::GetDependentComponents() const
		{
			static std::vector<EngineComponentIdType> dependentComponentList {
				EngineComponentIdType::kApplication
			};
			return dependentComponentList;
		}

		//------------------------------------------------------------------------------
		// Inject Dependency to the Component.
		// All dependency injections will be done before the initialization.
		void TouchInputComponent::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
		{
			switch (componentId)
			{
				case EngineComponentIdType::kApplication:
					{
						m_applicationComponent = dynamic_cast<Application::ApplicationComponent *>(dependentComponent);
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::OnTouchDown(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime)
		{
			TouchIdType convertedTouchId = static_cast<TouchIdType>(touchId);
			TouchCoordType convertedX = static_cast<TouchCoordType>(x);
			TouchCoordType convertedY = static_cast<TouchCoordType>(y);

			m_downedTouches.insert(convertedTouchId);

			m_immediateEventNotifier.NotifyEvent(
				[convertedTouchId, convertedX, convertedY, eventTime](IImmediateTouchInputEventObserver *observer) mutable
				{
					observer->OnTouchDown(convertedTouchId, convertedX, convertedY, eventTime);
				});
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::OnTouchMoved(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime)
		{
			TouchIdType convertedTouchId = static_cast<TouchIdType>(touchId);
			TouchCoordType convertedX = static_cast<TouchCoordType>(x);
			TouchCoordType convertedY = static_cast<TouchCoordType>(y);

			m_immediateEventNotifier.NotifyEvent(
				[convertedTouchId, convertedX, convertedY, eventTime](IImmediateTouchInputEventObserver *observer) mutable
				{
					observer->OnTouchMoved(convertedTouchId, convertedX, convertedY, eventTime);
				});
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::OnTouchUp(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType x, Application::DeviceScreenCoordType y, GameTimeClockType::time_point eventTime)
		{
			TouchIdType convertedTouchId = static_cast<TouchIdType>(touchId);
			TouchCoordType convertedX = static_cast<TouchCoordType>(x);
			TouchCoordType convertedY = static_cast<TouchCoordType>(y);

			m_downedTouches.erase(convertedTouchId);

			m_immediateEventNotifier.NotifyEvent(
				[convertedTouchId, convertedX, convertedY, eventTime](IImmediateTouchInputEventObserver *observer) mutable
				{
					observer->OnTouchUp(convertedTouchId, convertedX, convertedY, eventTime);
				});
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::OnTouchCanceled(Application::DeviceTouchIdType touchId, GameTimeClockType::time_point eventTime)
		{
			TouchIdType convertedTouchId = static_cast<TouchIdType>(touchId);

			m_downedTouches.erase(convertedTouchId);

			m_immediateEventNotifier.NotifyEvent(
				[convertedTouchId, eventTime](IImmediateTouchInputEventObserver *observer) mutable
				{
					observer->OnTouchCanceled(convertedTouchId, eventTime);
				});
		}

		//------------------------------------------------------------------------------
		bool TouchInputComponent::IsTouchDowned(TouchIdType touchId)
		{
			return (m_downedTouches.find(touchId) != m_downedTouches.end());
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::RegisterImmediateEventObserver(IImmediateTouchInputEventObserver *observer)
		{
			m_immediateEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void TouchInputComponent::UnRegisterImmediateEventObserver(IImmediateTouchInputEventObserver *observer)
		{
			m_immediateEventNotifier.UnRegisterObserver(observer);
		}
	}
}
