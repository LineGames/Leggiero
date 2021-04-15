﻿////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSimpleVerticalScrollPanel.h (Leggiero/Modules - LegacyUI)
//
// Vertically scrollable simple panel element
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_VERTICAL_SCROLL_PANEL_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_VERTICAL_SCROLL_PANEL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UITouch;


		namespace Element
		{
			// Vertical Scroll Panel Element
			class UIElementSimpleVerticalScrollPanel
				: public UIObject
			{
			protected:
				static constexpr int kInertiaSpeedSampleFrame = 3;

			public:
				using PositionValueType = float;
				using OnPositionChangeHandlerType = std::function<void(std::shared_ptr<UIElementSimpleVerticalScrollPanel>, PositionValueType)>;

			public:
				UIElementSimpleVerticalScrollPanel(std::shared_ptr<UIManager> ownerManager, UICoordinateType panelWidth,
					UICoordinateType boundedHeight, std::shared_ptr<UIObject> contentsObject, UICoordinateType contentsHeight,
					UICoordinateType scrollMovingThreshold = static_cast<UICoordinateType>(0.0f), bool isClipping = true,
					OnPositionChangeHandlerType onPositionChanged = ms_dummyOnPositionChangeHandler);
				virtual ~UIElementSimpleVerticalScrollPanel();

			protected:
				UIElementSimpleVerticalScrollPanel(std::shared_ptr<UIManager> ownerManager);

			public:	// UIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void _UpdateFrameSelf(GameTimeClockType::duration frameInterval) override;

			public:
				void SetOnPositionChangedHandler(OnPositionChangeHandlerType handler) { m_onPositionChangedHandlerFunc = handler; }
				void DiscardOnPositionChangedHandler() { m_onPositionChangedHandlerFunc = ms_dummyOnPositionChangeHandler; }

				void SetPosition(PositionValueType positionValue, bool isNotifyToHandler = false);
				void Stop();

				bool IsTouchedMoving() const { return m_isTouchedMoving; }

				PositionValueType GetCurrentPosition() const { return m_currentPosition; }
				PositionValueType GetMaximumValue() const;
				bool IsScrolling() const { return m_isScrolling; }

				UICoordinateType GetCurrentContentsHeight() const { return m_contentsHeight; }
				void SetContentsHeight(UICoordinateType height);

				UICoordinateType GetBoundedHeight() const { return m_boundedHeight; }

				std::shared_ptr<UIObject> GetContentsUIObject() { return m_contentsObject; }

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchedMovingState(bool isTouchedMoving);

				void _UpdateTouchScrolledValue();

			protected:
				virtual std::shared_ptr<UIObject> _AllocateCloningObject() const override;

			protected:
				UICoordinateType m_width;
				UICoordinateType m_boundedHeight;
				UICoordinateType m_contentsHeight;

				UICoordinateType m_scrollMovingThreshold;

				PositionValueType m_currentPosition;
				PositionValueType m_touchStartPosition;

				PositionValueType m_maximumValue;

				bool m_isCullingSubObjects;

			protected:
				std::shared_ptr<UIObject>	m_touchVirtualObject;
				std::shared_ptr<UIObject>	m_contentsObject;

				bool						m_isScrolling;

			protected:
				static OnPositionChangeHandlerType	ms_dummyOnPositionChangeHandler;
				OnPositionChangeHandlerType			m_onPositionChangedHandlerFunc;

				std::shared_ptr<UITouch> m_processingTouch;

				bool m_isTouchedMoving;

				bool m_isRecoverToBoundary;
				PositionValueType m_currentInertiaSpeed;
				float m_accumulatedRecoverTime;
				float m_recoverConstantA;
				float m_recoverConstantB;
				bool m_isRecoverToZero;

				struct SpeedSampleEntry
				{
				public:
					float elapsedTime;
					float moveAmount;

					SpeedSampleEntry(float elapsedTime = 0.0f, float moveAmount = 0.0f)
						: elapsedTime(elapsedTime), moveAmount(moveAmount)
					{ }
				};

				SpeedSampleEntry m_speedDataSample[kInertiaSpeedSampleFrame];
				int m_speedDataSampleCount;
				GameTimeClockType::time_point m_lastTouchProcessTime;

				void _AppendSpeedSampleData(float elapsedTime, float moveAmount);
			};
		}
	}
}

#endif
