////////////////////////////////////////////////////////////////////////////////
// Component/UITransformComponent.cpp (Leggiero/Modules - LegacyUI)
//
// UI Transform Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITransformComponent.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "UISizeSubComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIFixedTransformComponent

		//------------------------------------------------------------------------------
		UIFixedTransformComponent::UIFixedTransformComponent(std::shared_ptr<IUITransform> baseTransform)
			: m_baseTransform(baseTransform)
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UIFixedTransformComponent::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<UIFixedTransformComponent> createdComponent(std::make_shared<UIFixedTransformComponent>(m_baseTransform));

			return createdComponent;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> UIFixedTransformComponent::GetEffectiveTransform()
		{
			if (m_baseTransform)
			{
				return m_baseTransform;
			}
			m_baseTransform = std::make_shared<IdentityTransform>();
			return m_baseTransform;
		}


		//////////////////////////////////////////////////////////////////////////////// UIParametericTransformComponent

		//------------------------------------------------------------------------------
		UIParametericTransformComponent::UIParametericTransformComponent()
			: m_translate(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))
			, m_anchor(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))
			, m_rotate(static_cast<UICoordinateType>(0.0f))
			, m_scale(static_cast<UICoordinateType>(1.0f), static_cast<UICoordinateType>(1.0f))
			, m_skew(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))
			, m_perspective(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))
			, m_isTransformDirty(true)
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UIParametericTransformComponent::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<UIParametericTransformComponent> createdComponent(std::make_shared<UIParametericTransformComponent>());
			createdComponent->m_translate = m_translate;
			createdComponent->m_anchor = m_anchor;
			createdComponent->m_rotate = m_rotate;
			createdComponent->m_scale = m_scale;
			createdComponent->m_skew = m_skew;
			createdComponent->m_perspective = m_perspective;

			return createdComponent;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> UIParametericTransformComponent::GetEffectiveTransform()
		{
			if (m_isTransformDirty)
			{
				_BakeCachedTransform();
			}
			return m_bakedTransform;
		}

		//------------------------------------------------------------------------------
		void UIParametericTransformComponent::_BakeCachedTransform()
		{
			bool isTranslate = !(fabs(m_translate.x) < kFloatEpsilon && fabs(m_translate.y) < kFloatEpsilon);
			bool isMatrixTransform = !(fabs(m_rotate) < kFloatEpsilon
				&& fabs(m_scale.x - 1.0f) < kFloatEpsilon && fabs(m_scale.y - 1.0f) < kFloatEpsilon
				&& fabs(m_skew.x) < kFloatEpsilon && fabs(m_skew.y) < kFloatEpsilon);
			bool isPerspective = !(fabs(m_perspective.x) < kFloatEpsilon && fabs(m_perspective.y) < kFloatEpsilon);

			if (isMatrixTransform && isPerspective)
			{
				bool isSameTransformExists = false;
				if (m_bakedTransform)
				{
					std::shared_ptr<UICombinedTransform> bakedCombinedTransform = std::dynamic_pointer_cast<UICombinedTransform>(m_bakedTransform);
					if (bakedCombinedTransform)
					{
						if (bakedCombinedTransform->GetPre() && bakedCombinedTransform->GetPost()
							&& std::dynamic_pointer_cast<UIPerspectiveTransform>(bakedCombinedTransform->GetPre())
							&& std::dynamic_pointer_cast<UIParameterTransform>(bakedCombinedTransform->GetPost()))
							isSameTransformExists = true;
					}
				}
				if (!isSameTransformExists)
				{
					m_bakedTransform = std::make_shared<UICombinedTransform>(
						std::make_shared<UIPerspectiveTransform>(),
						std::make_shared<UIParameterTransform>()
						);
				}

				std::shared_ptr<UICombinedTransform> tempBakedCombindTransformHolder = std::dynamic_pointer_cast<UICombinedTransform>(m_bakedTransform);
				UICombinedTransform &combinedTransform = *tempBakedCombindTransformHolder;
				std::shared_ptr<UIPerspectiveTransform> tempPrePerspectiveTransformHolder = std::dynamic_pointer_cast<UIPerspectiveTransform>(combinedTransform.GetPre());
				UIPerspectiveTransform &prePerspectiveTransform = *tempPrePerspectiveTransformHolder;
				prePerspectiveTransform.SetAnchor(m_anchor.x, m_anchor.y);
				prePerspectiveTransform.SetPerspective(m_perspective.x, m_perspective.y);
				std::shared_ptr<UIParameterTransform> tempPostParametricTransformHolder;
				UIParameterTransform &postParametricTransform = *tempPostParametricTransformHolder;
				postParametricTransform.SetTranslate(m_translate.x, m_translate.y);
				postParametricTransform.SetAnchor(m_anchor.x, m_anchor.y);
				postParametricTransform.SetRotate(m_rotate);
				postParametricTransform.SetScale(m_scale.x, m_scale.y);
				postParametricTransform.SetSkew(m_skew.x, m_skew.y);
			}
			else if (isPerspective)
			{
				if (isTranslate)
				{
					bool isSameTransformExists = false;
					if (m_bakedTransform)
					{
						std::shared_ptr<UICombinedTransform> bakedCombinedTransform = std::dynamic_pointer_cast<UICombinedTransform>(m_bakedTransform);
						if (bakedCombinedTransform)
						{
							if (bakedCombinedTransform->GetPre() && bakedCombinedTransform->GetPost()
								&& std::dynamic_pointer_cast<UIPerspectiveTransform>(bakedCombinedTransform->GetPre())
								&& std::dynamic_pointer_cast<OffsetTransform>(bakedCombinedTransform->GetPost()))
								isSameTransformExists = true;
						}
					}
					if (!isSameTransformExists)
					{
						m_bakedTransform = std::make_shared<UICombinedTransform>(
							std::make_shared<UIPerspectiveTransform>(),
							std::make_shared<OffsetTransform>(m_translate.x, m_translate.y)
							);
					}

					std::shared_ptr<UICombinedTransform> tempBakedCombindTransformHolder = std::dynamic_pointer_cast<UICombinedTransform>(m_bakedTransform);
					UICombinedTransform &combinedTransform = *tempBakedCombindTransformHolder;
					std::shared_ptr<UIPerspectiveTransform> tempPrePerspectiveTransformHolder = std::dynamic_pointer_cast<UIPerspectiveTransform>(combinedTransform.GetPre());
					UIPerspectiveTransform &prePerspectiveTransform = *tempPrePerspectiveTransformHolder;
					prePerspectiveTransform.SetAnchor(m_anchor.x, m_anchor.y);
					prePerspectiveTransform.SetPerspective(m_perspective.x, m_perspective.y);
					std::shared_ptr<OffsetTransform> tempPostOffsetTransformHolder = std::dynamic_pointer_cast<OffsetTransform>(combinedTransform.GetPost());
					OffsetTransform &postOffsetTransform = *tempPostOffsetTransformHolder;
					postOffsetTransform.SetOffset(m_translate.x, m_translate.y);
				}
				else
				{
					bool isSameTransformExists = false;
					if (m_bakedTransform)
					{
						std::shared_ptr<UIPerspectiveTransform> bakedPerspectiveTransform = std::dynamic_pointer_cast<UIPerspectiveTransform>(m_bakedTransform);
						if (bakedPerspectiveTransform)
						{
							isSameTransformExists = true;
						}
					}
					if (!isSameTransformExists)
					{
						m_bakedTransform = std::make_shared<UIPerspectiveTransform>();
					}

					std::shared_ptr<UIPerspectiveTransform> tempPerspectiveTransformHolder = std::dynamic_pointer_cast<UIPerspectiveTransform>(m_bakedTransform);
					UIPerspectiveTransform &perspectiveTransform = *tempPerspectiveTransformHolder;
					perspectiveTransform.SetAnchor(m_anchor.x, m_anchor.y);
					perspectiveTransform.SetPerspective(m_perspective.x, m_perspective.y);
				}
			}
			else if (isMatrixTransform)
			{
				bool isSameTransformExists = false;
				if (m_bakedTransform)
				{
					std::shared_ptr<UIParameterTransform> bakedParamterTransform = std::dynamic_pointer_cast<UIParameterTransform>(m_bakedTransform);
					if (bakedParamterTransform)
					{
						isSameTransformExists = true;
					}
				}
				if (!isSameTransformExists)
				{
					m_bakedTransform = std::make_shared<UIParameterTransform>();
				}

				std::shared_ptr<UIParameterTransform> tempParamterTransformHolder = std::dynamic_pointer_cast<UIParameterTransform>(m_bakedTransform);
				UIParameterTransform &paramterTransform = *tempParamterTransformHolder;
				paramterTransform.SetTranslate(m_translate.x, m_translate.y);
				paramterTransform.SetAnchor(m_anchor.x, m_anchor.y);
				paramterTransform.SetRotate(m_rotate);
				paramterTransform.SetScale(m_scale.x, m_scale.y);
				paramterTransform.SetSkew(m_skew.x, m_skew.y);
			}
			else if (isTranslate)
			{
				bool isSameTransformExists = false;
				if (m_bakedTransform)
				{
					std::shared_ptr<OffsetTransform> bakedOffsetTransform = std::dynamic_pointer_cast<OffsetTransform>(m_bakedTransform);
					if (bakedOffsetTransform)
					{
						bakedOffsetTransform->SetOffset(m_translate.x, m_translate.y);
						isSameTransformExists = true;
					}
				}
				if (!isSameTransformExists)
				{
					m_bakedTransform = std::make_shared<OffsetTransform>(m_translate.x, m_translate.y);
				}
			}
			else
			{
				if (!m_bakedTransform || !std::dynamic_pointer_cast<IdentityTransform>(m_bakedTransform))
				{
					m_bakedTransform = std::make_shared<IdentityTransform>();
				}
			}

			m_isTransformDirty = false;
		}

		//------------------------------------------------------------------------------
		UIVector2D UIParametericTransformComponent::GetPerspectiveFocus() const
		{
			if (fabs(m_perspective.x) < kFloatEpsilon && fabs(m_perspective.y) < kFloatEpsilon)
			{
				return UIVector2D::kZero;
			}
			UICoordinateType focalLength = static_cast<UICoordinateType>(1.0f / sqrt(m_perspective.x * m_perspective.x + m_perspective.y * m_perspective.y));
			UIVector2D focalDirection(static_cast<UICoordinateType>(m_perspective.x), static_cast<UICoordinateType>(m_perspective.y)); focalDirection.DoNormalize();
			return focalLength * focalDirection;
		}

		//------------------------------------------------------------------------------
		UICoordinateType UIParametericTransformComponent::GetPerspectiveFocusX() const
		{
			return GetPerspectiveFocus().x;
		}

		//------------------------------------------------------------------------------
		UICoordinateType UIParametericTransformComponent::GetPerspectiveFocusY() const
		{
			return GetPerspectiveFocus().y;
		}

		//------------------------------------------------------------------------------
		void UIParametericTransformComponent::SetPerspectiveFocus(UICoordinateType x, UICoordinateType y)
		{
			if (fabs(x) < kFloatEpsilon && fabs(y) < kFloatEpsilon)
			{
				m_perspective.x = 0.0f;
				m_perspective.y = 0.0f;
				_Invalidate();
				return;
			}
			UIVector2D focalPoint(x, y);
			UICoordinateType focalLength = static_cast<UICoordinateType>(focalPoint.Length());
			UICoordinateType paramA = static_cast<UICoordinateType>(1.0f / (focalLength * focalLength));
			if (fabs(x) < kFloatEpsilon)
			{
				m_perspective.x = 0.0f;
				m_perspective.y = (y > 0.0f) ? (float)sqrt(paramA) : -(float)sqrt(paramA);
				_Invalidate();
				return;
			}
			else if (fabs(y) < kFloatEpsilon)
			{
				m_perspective.x = (x > 0.0f) ? (float)sqrt(paramA) : -(float)sqrt(paramA);
				m_perspective.y = 0.0f;
				_Invalidate();
				return;
			}
			UICoordinateType paramB = x / y;
			m_perspective.y = (y > 0.0f) ? (float)sqrt(paramA / (paramB * paramB + 1)) : -(float)sqrt(paramA / (paramB * paramB + 1));
			m_perspective.x = paramB * m_perspective.y;
			_Invalidate();
		}
	}
}
