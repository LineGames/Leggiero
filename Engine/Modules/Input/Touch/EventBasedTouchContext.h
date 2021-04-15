////////////////////////////////////////////////////////////////////////////////
// Touch/EventBasedTouchContext.h (Leggiero/Modules - Input)
//
// Touch processing context based on events
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_INPUT__TOUCH__EVENT_BASED_TOUCH_CONTEXT_H
#define __LM_INPUT__TOUCH__EVENT_BASED_TOUCH_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <unordered_map>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Input
#include "../IImmediateTouchInputEventObserver.h"
#include "TouchEvent.h"
#include "LastTouchState.h"


namespace Leggiero
{
	namespace Input
	{
		// Forward Declaration
		class TouchInputComponent;


		namespace Touch
		{
			// Touch Processing Context
			class EventBasedTouchContext
				: public IImmediateTouchInputEventObserver
			{
			public:
				static constexpr size_t kDefaultQueueLimit = 128;
				static constexpr size_t kQueueLimitUnlimited = 0;

			public:
				EventBasedTouchContext(TouchInputComponent *baseInputComponent, size_t queueLengthLimit = kDefaultQueueLimit);
				virtual ~EventBasedTouchContext();

			public:	// Context Interface
				void StartProcessing();
				void StopProcessing();
				bool IsProcessing() const { return m_isSubscribing.load(); }

				void ClearContextState();

			public:	// Event Interface
				TouchEvent DequeueEvent();

				size_t GetQueueLengthLimit() const { return m_queueLengthLimit; }
				void SetQueueLengthLimit(size_t queueLengthLimit) { m_queueLengthLimit = queueLengthLimit; }

			public:	// State Interface
				bool IsTouchActive(TouchIdType touchId);
				LastTouchState GetActiveTouchState(TouchIdType touchId);
				std::vector<LastTouchState> GetAllActiveTouchState();

			public:	// IImmediateTouchInputEventObserver
				// Handle Touch Down Event
				virtual void OnTouchDown(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime) override;

				// Handle Touch Move Event
				virtual void OnTouchMoved(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime) override;

				// Handle Touch Up Event
				virtual void OnTouchUp(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime) override;

				// Handle Touch Cancel Event by App Interrupt, or ...
				virtual void OnTouchCanceled(TouchIdType touchId, GameTimeClockType::time_point eventTime) override;

			protected:
				void _TrimEventQueue();

			protected:
				TouchInputComponent *m_touchInputComponent;

				Utility::Threading::SafePthreadLock m_subscribeLock;
				std::atomic_bool					m_isSubscribing;

				std::unordered_map<TouchIdType, LastTouchState> m_activeTouchMap;
				Utility::Threading::SafePthreadRWLock			m_activeTouchMapLock;

				moodycamel::ConcurrentQueue<TouchEvent> m_queuedEvents;

				size_t m_queueLengthLimit;
			};
		}
	}
}

#endif
