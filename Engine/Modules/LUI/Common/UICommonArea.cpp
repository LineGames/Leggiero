////////////////////////////////////////////////////////////////////////////////
// Common/UICommonArea.cpp (Leggiero/Modules - LegacyUI)
//
// Commonly used areas implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UICommonArea.h"

// Leggiero.Graphics
#include <Graphics/Common/GraphicsReferenceState.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Component/UILayoutComponent.h"
#include "../Rendering/UIClipping.h"
#include "UITransform.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace CommonArea
		{
			namespace Operation
			{
				//////////////////////////////////////////////////////////////////////////////// Union

				//------------------------------------------------------------------------------
				Union::Union(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b)
					: m_sourceAreas({ a, b })
				{
				}

				//------------------------------------------------------------------------------
				Union::Union(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b, const std::shared_ptr<IUIArea> &c)
					: m_sourceAreas({ a, b, c })
				{
				}

				//------------------------------------------------------------------------------
				Union::Union(std::initializer_list<std::shared_ptr<IUIArea>> initializer)
					: m_sourceAreas(initializer)
				{
				}

				//------------------------------------------------------------------------------
				bool Union::IsInArea(UICoordinateType x, UICoordinateType y) const
				{
					for (const std::shared_ptr<IUIArea> &currentSubArea : m_sourceAreas)
					{
						if (currentSubArea->IsInArea(x, y))
						{
							return true;
						}
					}
					return false;
				}


				//////////////////////////////////////////////////////////////////////////////// Intersection

				//------------------------------------------------------------------------------
				Intersection::Intersection(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b)
					: m_sourceAreas({ a, b })
				{
				}

				//------------------------------------------------------------------------------
				Intersection::Intersection(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b, const std::shared_ptr<IUIArea> &c)
					: m_sourceAreas({ a, b, c })
				{
				}

				//------------------------------------------------------------------------------
				Intersection::Intersection(std::initializer_list<std::shared_ptr<IUIArea> > initializer)
					: m_sourceAreas(initializer)
				{
				}

				//------------------------------------------------------------------------------
				bool Intersection::IsInArea(UICoordinateType x, UICoordinateType y) const
				{
					for (const std::shared_ptr<IUIArea> &currentSubArea : m_sourceAreas)
					{
						if (!currentSubArea->IsInArea(x, y))
						{
							return false;
						}
					}
					return true;
				}
			}


			namespace Shape
			{
				//////////////////////////////////////////////////////////////////////////////// Rect

				//------------------------------------------------------------------------------
				Rect::Rect(UICoordinateType width, UICoordinateType height, UICoordinateType left, UICoordinateType top)
					: m_left(left), m_top(top), m_right(left + width), m_bottom(top + height)
				{
				}

				//------------------------------------------------------------------------------
				bool Rect::IsInArea(UICoordinateType x, UICoordinateType y) const
				{
					return ((x >= m_left && x <= m_right) && (y >= m_top && y <= m_bottom));
				}


				//////////////////////////////////////////////////////////////////////////////// Circle

				//------------------------------------------------------------------------------
				Circle::Circle(UICoordinateType radius, UICoordinateType x, UICoordinateType y)
					: m_radiusSquare(radius *radius), m_x(x), m_y(y)
				{
				}

				//------------------------------------------------------------------------------
				bool Circle::IsInArea(UICoordinateType x, UICoordinateType y) const
				{
					UICoordinateType dx = x - m_x;
					UICoordinateType dy = y - m_y;

					return ((dx * dx + dy * dy) <= m_radiusSquare);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// ScreenArea

			//------------------------------------------------------------------------------
			ScreenArea::ScreenArea(UICoordinateRatioType leftRatio, UICoordinateRatioType topRatio, UICoordinateRatioType rightRatio, UICoordinateRatioType bottomRatio)
				: m_leftRatio(leftRatio), m_topRatio(topRatio), m_rightRatio(rightRatio), m_bottomRatio(bottomRatio)
			{
			}

			//------------------------------------------------------------------------------
			bool ScreenArea::IsInArea(UICoordinateType x, UICoordinateType y) const
			{
				UICoordinateType absoluteLeft = (UICoordinateType)(m_leftRatio * Graphics::ReferenceState::GetReferenceViewportWidth());
				UICoordinateType absoluteTop = (UICoordinateType)(m_topRatio * Graphics::ReferenceState::GetReferenceViewportHeight());
				UICoordinateType absoluteRight = (UICoordinateType)(m_rightRatio * Graphics::ReferenceState::GetReferenceViewportWidth());
				UICoordinateType absoluteBottom = (UICoordinateType)(m_bottomRatio * Graphics::ReferenceState::GetReferenceViewportHeight());

				return ((x >= absoluteLeft && x <= absoluteRight) && (y >= absoluteTop && y <= absoluteBottom));
			}


			//////////////////////////////////////////////////////////////////////////////// ScreenAbsoluteArea

			//------------------------------------------------------------------------------
			ScreenAbsoluteArea::ScreenAbsoluteArea(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom)
				: m_left(left), m_top(top), m_right(right), m_bottom(bottom)
			{
			}

			//------------------------------------------------------------------------------
			bool ScreenAbsoluteArea::IsInArea(UICoordinateType x, UICoordinateType y) const
			{
				return ((x >= m_left && x <= m_right) && (y >= m_top && y <= m_bottom));
			}


			//////////////////////////////////////////////////////////////////////////////// ObjectBasedArea

			//------------------------------------------------------------------------------
			ObjectBasedArea::ObjectBasedArea(std::shared_ptr<UIObject> sourceObject, UICoordinateRatioType widthRatio, UICoordinateRatioType heightRatio, UICoordinateType offsetX, UICoordinateType offsetY)
				: m_sourceObject(sourceObject), m_widthRatio(widthRatio), m_heightRatio(heightRatio), m_offsetX(offsetX), m_offsetY(offsetY), m_isApplyClipping(true)
			{
			}

			//------------------------------------------------------------------------------
			bool ObjectBasedArea::IsInArea(UICoordinateType x, UICoordinateType y) const
			{
				std::shared_ptr<UIObject> sourceObjectCopy(m_sourceObject.lock());
				if (!sourceObjectCopy)
				{
					return false;
				}

				if (!sourceObjectCopy->IsVisible())
				{
					return false;
				}

				std::shared_ptr<CalculatedUILayoutInformation> tragetLayoutInfoPointer(sourceObjectCopy->GetCachedLayoutInformation());
				if (!tragetLayoutInfoPointer)
				{
					return false;
				}
				CalculatedUILayoutInformation &tragetLayoutInfo = *tragetLayoutInfoPointer;
				if (!tragetLayoutInfo.applyingTransform)
				{
					return false;
				}

				UICoordinateType widthOffset = (UICoordinateType)((m_widthRatio - 1.0) * tragetLayoutInfo.width * 0.5);
				UICoordinateType heightOffset = (UICoordinateType)((m_heightRatio - 1.0) * tragetLayoutInfo.height * 0.5);

				UIVector2D localTouchPoint = tragetLayoutInfo.applyingTransform->ApplyInverseTransform(x, y);
				localTouchPoint.x -= m_offsetX;
				localTouchPoint.y -= m_offsetY;

				return ((localTouchPoint.x >= -widthOffset && localTouchPoint.x <= tragetLayoutInfo.width + widthOffset) && (localTouchPoint.y >= -heightOffset && localTouchPoint.y <= tragetLayoutInfo.height + heightOffset))
					&& (!m_isApplyClipping || (tragetLayoutInfo.applyingClipping && tragetLayoutInfo.applyingClipping->IsPointPass(x, y)));;
			}


			//////////////////////////////////////////////////////////////////////////////// ObjectOffsetArea

			//------------------------------------------------------------------------------
			ObjectOffsetArea::ObjectOffsetArea(std::shared_ptr<IUIArea> originalArea, std::shared_ptr<UIObject> sourceObject, UICoordinateRatioType referenceRatioX, UICoordinateRatioType referenceRatioY, UICoordinateType offsetX, UICoordinateType offsetY)
				: m_originalArea(originalArea), m_sourceObject(sourceObject), m_referenceRatioX(referenceRatioX), m_referenceRatioY(referenceRatioY), m_offsetX(offsetX), m_offsetY(offsetY), m_isApplyClipping(true)
			{
			}

			//------------------------------------------------------------------------------
			bool ObjectOffsetArea::IsInArea(UICoordinateType x, UICoordinateType y) const
			{
				std::shared_ptr<UIObject> sourceObjectCopy(m_sourceObject.lock());
				if (!sourceObjectCopy)
				{
					return false;
				}

				std::shared_ptr<CalculatedUILayoutInformation> tragetLayoutInfoPointer(sourceObjectCopy->GetCachedLayoutInformation());
				if (!tragetLayoutInfoPointer)
				{
					return false;
				}
				CalculatedUILayoutInformation &tragetLayoutInfo = *tragetLayoutInfoPointer;
				if (!tragetLayoutInfo.applyingTransform)
				{
					return false;
				}

				UIVector2D localTouchPoint = tragetLayoutInfo.applyingTransform->ApplyInverseTransform(x, y);
				localTouchPoint.x -= tragetLayoutInfo.width * m_referenceRatioX + m_offsetX;
				localTouchPoint.y -= tragetLayoutInfo.height * m_referenceRatioY + m_offsetY;

				return m_originalArea->IsInArea(localTouchPoint.x, localTouchPoint.y)
					&& (!m_isApplyClipping || (tragetLayoutInfo.applyingClipping && tragetLayoutInfo.applyingClipping->IsPointPass(x, y)));
			}
		}
	}
}
