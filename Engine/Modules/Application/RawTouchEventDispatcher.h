////////////////////////////////////////////////////////////////////////////////
// RawTouchEventDispatcher.h (Leggiero/Modules - Application)
//
// System Raw Touch Event Dispatcher Sub-Component Base Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__RAW_TOUCH_EVENT_DISPATCHER_H
#define __LM_APPLICATION__RAW_TOUCH_EVENT_DISPATCHER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/EventNotifier.h>


namespace Leggiero
{
	namespace Application
	{
		// Forward Declaration of Observer Interface
		class IRawTouchInputEventObserver;


		// Raw Touch Event Dispatcher Base Class
		class RawTouchEventDispatcher
		{
		public:
			virtual ~RawTouchEventDispatcher() { }

		public:
			void RegisterRawTouchEventObserver(IRawTouchInputEventObserver *observer);
			void UnRegisterRawTouchEventObserver(IRawTouchInputEventObserver *observer);

		protected:
			Utility::DesignPattern::EventNotifier<IRawTouchInputEventObserver *>	m_touchEventNotifier;
		};
	}
}

#endif
