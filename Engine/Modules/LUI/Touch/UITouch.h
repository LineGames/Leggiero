////////////////////////////////////////////////////////////////////////////////
// Touch/UITouch.h (Leggiero/Modules - LegacyUI)
//
// UI Processing Touch
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__TOUCH__UI_TOUCH_H
#define __LM_LUI__TOUCH__UI_TOUCH_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

// Leggiero.Input
#include <Input/Touch/TouchEvent.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIObject;
		class UITouchNegotiator;


		// UI Touch
		class UITouch
			: public std::enable_shared_from_this<UITouch>
		{
			friend class UITouchNegotiator;

		public:
			UITouch(Input::TouchIdType touchId, UICoordinateType startX, UICoordinateType startY, GameTimeClockType::time_point startTime);
			virtual ~UITouch();

		public:
			// Subscribe events from the touch.
			// While not subscribing, can only receive Down and In events.
			// When isStrong is false, OnTouchOut, OnTouchCovered event implicitly means un-subcribing.
			void Subscribe(std::shared_ptr<UIObject> &subscriber, bool isStrong);

			void UnSubscribe(std::shared_ptr<UIObject> &subscriber);

			bool ClaimPrimary(std::shared_ptr<UIObject> &subscriber);
			void ResignPrimary(std::shared_ptr<UIObject> &subscriber);

			bool IsSubscribed(const std::shared_ptr<UIObject> &subscriber) const;
			bool IsOwnPrimary(const std::shared_ptr<UIObject> &subscriber) const;
			bool IsPrimaryOccupied() const;

		public:
			bool IsTouchEnded() const { return !m_isAlive; }

			Input::TouchIdType GetTouchId() const { return m_touchId; }

			GameTimeClockType::time_point GetTouchStartTime() const { return m_startTime; }
			UICoordinateType GetTouchStartX() const { return m_startX; }
			UICoordinateType GetTouchStartY() const { return m_startY; }
			UIVector2D GetTouchStartPosition() const { return UIVector2D(m_startX, m_startY); }

			GameTimeClockType::time_point GetLastTouchChangeTime() const { return m_lastChangeTime; }
			UICoordinateType GetLastTouchX() const { return m_lastX; }
			UICoordinateType GetLastTouchY() const { return m_lastY; }
			UIVector2D GetLastTouchPosition() const { return UIVector2D(m_lastX, m_lastY); }

			GameTimeClockType::time_point GetCurrentTouchChangeTime() const { return m_currentChangeTime; }
			UICoordinateType GetCurrentTouchX() const { return m_currentX; }
			UICoordinateType GetCurrentTouchY() const { return m_currentY; }
			UIVector2D GetCurrentTouchPosition() const { return UIVector2D(m_currentX, m_currentY); }

			void UpdateVirtualTouchChangeTime(GameTimeClockType::time_point currentTime) { m_currentChangeTime = currentTime; }

		public:
			void UpdateTouchDataUsingEvent(const Input::Touch::TouchEvent &touchEvent);

		protected:
			void _ProcessTouchMoveEvent(std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects);
			void _ProcessTouchUpEvent();
			void _ProcessTouchCancelEvent();

			void _UpdateTouchState(std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, std::unordered_set<UIObjectIdType> &inTreeObjectIdTable);

			bool _NotifyMoveToStrongSubscriber(std::shared_ptr<UIObject> &targetObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, bool isRealMoved);
			bool _NotifyMoveToWeekSubscriber(std::shared_ptr<UIObject> &targetObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, bool isRealMoved);

		protected:
			bool m_isAlive;

			Input::TouchIdType		m_touchId;

			GameTimeClockType::time_point	m_startTime;
			UICoordinateType				m_startX;
			UICoordinateType				m_startY;

			GameTimeClockType::time_point	m_lastChangeTime;
			UICoordinateType				m_lastX;
			UICoordinateType				m_lastY;

			GameTimeClockType::time_point	m_currentChangeTime;
			UICoordinateType				m_currentX;
			UICoordinateType				m_currentY;

		protected:
			std::shared_ptr<UIObject> m_primarySubscriber;
			std::list<std::tuple<bool, std::weak_ptr<UIObject> > > m_subscribers;

			std::unordered_set<UIObjectIdType> m_lastTouchInObjects;
			std::unordered_set<UIObjectIdType> m_onceInObjects;
		};
	}
}

#endif
