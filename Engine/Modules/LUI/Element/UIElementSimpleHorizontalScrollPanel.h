////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSimpleHorizontalScrollPanel.h (Leggiero/Modules - LegacyUI)
//
// Horizontally scrollable simple panel element
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_HORIZONTAL_SCROLL_PANEL_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_HORIZONTAL_SCROLL_PANEL_H


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
			// Horizontal Scroll Panel
			class UIElementSimpleHorizontalScrollPanel
				: public UIObject
			{
			protected:
				static constexpr int kInertiaSpeedSampleFrame = 3;

			public:
				using PositionValueType = float;
				using OnPositionChangeHandlerType = std::function<void(std::shared_ptr<UIElementSimpleHorizontalScrollPanel>, PositionValueType)>;

			public:
				UIElementSimpleHorizontalScrollPanel(std::shared_ptr<UIManager> ownerManager, UICoordinateType panelHeight,
					UICoordinateType boundedWidth, std::shared_ptr<UIObject> contentsObject, UICoordinateType contentsWidth,
					UICoordinateType scrollMovingThreshold = static_cast<UICoordinateType>(0.0f), bool isClipping = true,
					OnPositionChangeHandlerType onPositionChanged = ms_dummyOnPositionChangeHandler);
				virtual ~UIElementSimpleHorizontalScrollPanel();

			protected:
				UIElementSimpleHorizontalScrollPanel(std::shared_ptr<UIManager> ownerManager);

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

				UICoordinateType GetCurrentContentsWidth() const { return m_contentsWidth; }
				void SetContentsWidth(UICoordinateType width);

				UICoordinateType GetBoundedWidth() const { return m_boundedWidth; }

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
				UICoordinateType m_height;
				UICoordinateType m_boundedWidth;
				UICoordinateType m_contentsWidth;

				UICoordinateType m_scrollMovingThreshold;

				PositionValueType m_currentPosition;
				PositionValueType m_touchStartPosition;

				PositionValueType m_maximumValue;

				bool m_isCullingSubObjects;

			protected:
				std::shared_ptr<UIObject>	m_touchVirtualObject;
				std::shared_ptr<UIObject>	m_contentsObject;

				bool							m_isScrolling;

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
