////////////////////////////////////////////////////////////////////////////////
// SystemEventDispatcher.h (Leggiero/Modules - Application)
//
// System Event Dispatcher Sub-Component Base Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__SYSTEM_EVENT_DISPATCHER_H
#define __LM_APPLICATION__SYSTEM_EVENT_DISPATCHER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/EventNotifier.h>


namespace Leggiero
{
	namespace Application
	{
		// Forward Declaration of Observer Interfaces
		class IApplicationEventObserver;
		class ISystemCommandEventObserver;
		class ISystemEventObserver;
		class IGraphicEventObserver;


		// System Event Dispatcher Base Class
		class SystemEventDispatcher
		{
		public:
			virtual ~SystemEventDispatcher() { }

		public:
			void RegisterApplicationEventObserver(IApplicationEventObserver *observer);
			void UnRegisterApplicationEventObserver(IApplicationEventObserver *observer);

			void RegisterCommandEventObserver(ISystemCommandEventObserver *observer);
			void UnRegisterCommandEventObserver(ISystemCommandEventObserver *observer);

			void RegisterSystemEventObserver(ISystemEventObserver *observer);
			void UnRegisterSystemEventObserver(ISystemEventObserver *observer);

			void RegisterGraphicEventObserver(IGraphicEventObserver *observer);
			void UnRegisterGraphicEventObserver(IGraphicEventObserver *observer);

		protected:
			Utility::DesignPattern::EventNotifier<IApplicationEventObserver *>		m_applicationEventNotifier;
			Utility::DesignPattern::EventNotifier<ISystemCommandEventObserver *>	m_commandEventNotifier;
			Utility::DesignPattern::EventNotifier<ISystemEventObserver *>			m_systemEventNotifier;
			Utility::DesignPattern::EventNotifier<IGraphicEventObserver *>			m_graphicEventNotifier;
		};
	}
}

#endif
