////////////////////////////////////////////////////////////////////////////////
// RawTouchEventDispatcher.cpp (Leggiero/Modules - Application)
//
// Raw Touch Event Dispatcher Base Implementation
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "RawTouchEventDispatcher.h"


namespace Leggiero
{
	namespace Application
	{
		//------------------------------------------------------------------------------
		void RawTouchEventDispatcher::RegisterRawTouchEventObserver(IRawTouchInputEventObserver *observer)
		{
			m_touchEventNotifier.RegisterObserver(observer);
		}

		//------------------------------------------------------------------------------
		void RawTouchEventDispatcher::UnRegisterRawTouchEventObserver(IRawTouchInputEventObserver *observer)
		{
			m_touchEventNotifier.UnRegisterObserver(observer);
		}
	}
}
