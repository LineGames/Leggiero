////////////////////////////////////////////////////////////////////////////////
// SystemEventDispatcher.cpp (Leggiero/Modules - Application)
//
// System Event Dispatcher Base Implementation
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "SystemEventDispatcher.h"


namespace Leggiero
{
	namespace Application
	{
		//------------------------------------------------------------------------------
		void SystemEventDispatcher::RegisterApplicationEventObserver(IApplicationEventObserver *observer)
		{
			m_applicationEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::UnRegisterApplicationEventObserver(IApplicationEventObserver *observer)
		{
			m_applicationEventNotifier.UnRegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::RegisterCommandEventObserver(ISystemCommandEventObserver *observer)
		{
			m_commandEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::UnRegisterCommandEventObserver(ISystemCommandEventObserver *observer)
		{
			m_commandEventNotifier.UnRegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::RegisterSystemEventObserver(ISystemEventObserver *observer)
		{
			m_systemEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::UnRegisterSystemEventObserver(ISystemEventObserver *observer)
		{
			m_systemEventNotifier.UnRegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::RegisterGraphicEventObserver(IGraphicEventObserver *observer)
		{
			m_graphicEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void SystemEventDispatcher::UnRegisterGraphicEventObserver(IGraphicEventObserver *observer)
		{
			m_graphicEventNotifier.UnRegisterObserver(observer);
		}
	}
}
