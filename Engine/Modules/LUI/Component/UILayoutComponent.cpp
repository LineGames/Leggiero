////////////////////////////////////////////////////////////////////////////////
// Component/UILayoutComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Layouting Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UILayoutComponent.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "UISizeSubComponent.h"
#include "../Common/UITransform.h"
#include "UITransformComponent.h"
#include "../Rendering/UIClipping.h"
#include "UIClippingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// CalculatedUILayoutInformation

		//------------------------------------------------------------------------------
		void CalculatedUILayoutInformation::ResetInformation()
		{
			objectId = kInvalidObjectId;
			objectReference.reset();

			isSizeCalculated = false;
			isOffsetCalculated = false;

			applyingTransform.reset();
			applyingClipping.reset();
		}


		//////////////////////////////////////////////////////////////////////////////// UILayoutComponent

		//------------------------------------------------------------------------------
		UILayoutComponent::UILayoutComponent(std::shared_ptr<IUISizeSubComponent> sizeSubComponent)
			: m_sizeSubComponent(sizeSubComponent)
			, parentReferenceRatioX(0.0f), parentReferenceRatioY(0.0f), parentOffsetX(0.0f), parentOffsetY(0.0f)
			, anchorReferenceRatioX(0.0f), anchorReferenceRatioY(0.0f), anchorOffsetX(0.0f), anchorOffsetY(0.0f)
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UILayoutComponent::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<IUISizeSubComponent> sizeSubComponentCopy;
			if (m_sizeSubComponent)
			{
				sizeSubComponentCopy = m_sizeSubComponent->Clone();

				std::shared_ptr<IOwnerTargetingSizeSubComponent> targetingSubComponent(std::dynamic_pointer_cast<IOwnerTargetingSizeSubComponent>(sizeSubComponentCopy));
				if (targetingSubComponent)
				{
					targetingSubComponent->ChangeTargetObject(std::const_pointer_cast<UIObject>(ownerObject.shared_from_this()));
				}
			}

			std::shared_ptr<UILayoutComponent> createdComponent(std::make_shared<UILayoutComponent>(sizeSubComponentCopy));

			createdComponent->parentReferenceRatioX = parentReferenceRatioX;
			createdComponent->parentReferenceRatioY = parentReferenceRatioY;
			createdComponent->parentOffsetX = parentOffsetX;
			createdComponent->parentOffsetY = parentOffsetY;
			createdComponent->anchorReferenceRatioX = anchorReferenceRatioX;
			createdComponent->anchorReferenceRatioY = anchorReferenceRatioY;
			createdComponent->anchorOffsetX = anchorOffsetX;
			createdComponent->anchorOffsetY = anchorOffsetY;

			return createdComponent;
		}

		//------------------------------------------------------------------------------
		void UILayoutComponent::SetParentOfOwnerObject(const UIObject &parentObject)
		{
			if (m_sizeSubComponent)
			{
				m_sizeSubComponent->SetParentOfOwnerObject(parentObject);
			}
		}

		//------------------------------------------------------------------------------
		void UILayoutComponent::SetNoParentOfOwnerObject()
		{
			if (m_sizeSubComponent)
			{
				m_sizeSubComponent->SetNoParentOfOwnerObject();
			}
		}

		//------------------------------------------------------------------------------
		void UILayoutComponent::UpdateLayout(UIObject &targetObject, UIObject &parentObject, std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> > &layoutTable)
		{
			if (layoutTable.find(targetObject.GetId()) == layoutTable.end())
			{
				return;
			}
			if (layoutTable.find(parentObject.GetId()) == layoutTable.end())
			{
				return;
			}

			CalculatedUILayoutInformation &parentObjectLayoutInfo = *(layoutTable[parentObject.GetId()]);
			CalculatedUILayoutInformation &currentObjectLayoutInfo = *(layoutTable[targetObject.GetId()]);

			// Make Transform
			std::shared_ptr<IUITransformComponent> transformComponent = targetObject.GetComponentObject<IUITransformComponent>(UIComponentType::kTransform);
			std::shared_ptr<IUITransform> objectTransform;
			if (transformComponent)
			{
				objectTransform = transformComponent->GetEffectiveTransform();
			}

			// Only Floating Objects for NOW
			UIElementSize calculatedSize = m_sizeSubComponent->CalculateSize();
			currentObjectLayoutInfo.width = calculatedSize.width;
			currentObjectLayoutInfo.height = calculatedSize.height;
			currentObjectLayoutInfo.marginedWidth = calculatedSize.width;
			currentObjectLayoutInfo.marginedHeight = calculatedSize.height;
			currentObjectLayoutInfo.isSizeCalculated = true;

			if (objectTransform)
			{
				UIVector2D anchorOffset = objectTransform->ApplyTransform(static_cast<UICoordinateType>(anchorReferenceRatioX * calculatedSize.width) + anchorOffsetX, static_cast<UICoordinateType>(anchorReferenceRatioY * calculatedSize.height) + anchorOffsetY) - objectTransform->ApplyTransform(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f));
				currentObjectLayoutInfo.parentalOffsetX = (static_cast<UICoordinateType>(parentReferenceRatioX * parentObjectLayoutInfo.width) + parentOffsetX) - anchorOffset.x;
				currentObjectLayoutInfo.parentalOffsetY = (static_cast<UICoordinateType>(parentReferenceRatioY * parentObjectLayoutInfo.height) + parentOffsetY) - anchorOffset.y;
				currentObjectLayoutInfo.isOffsetCalculated = true;

				if (fabs(currentObjectLayoutInfo.parentalOffsetX) < kFloatEpsilon && fabs(currentObjectLayoutInfo.parentalOffsetY) < kFloatEpsilon)
				{
					currentObjectLayoutInfo.applyingTransform = parentObjectLayoutInfo.applyingTransform->Combine(objectTransform);
				}
				else
				{
					currentObjectLayoutInfo.applyingTransform = parentObjectLayoutInfo.applyingTransform->Combine(OffsetTransform::AllocateTransform(currentObjectLayoutInfo.parentalOffsetX, currentObjectLayoutInfo.parentalOffsetY)->Combine(objectTransform));
				}
			}
			else
			{
				currentObjectLayoutInfo.parentalOffsetX = (static_cast<UICoordinateType>(parentReferenceRatioX * parentObjectLayoutInfo.width) + parentOffsetX) - (static_cast<UICoordinateType>(anchorReferenceRatioX * calculatedSize.width) + anchorOffsetX);
				currentObjectLayoutInfo.parentalOffsetY = (static_cast<UICoordinateType>(parentReferenceRatioY * parentObjectLayoutInfo.height) + parentOffsetY) - (static_cast<UICoordinateType>(anchorReferenceRatioY * calculatedSize.height) + anchorOffsetY);
				currentObjectLayoutInfo.isOffsetCalculated = true;

				if (fabs(currentObjectLayoutInfo.parentalOffsetX) < kFloatEpsilon && fabs(currentObjectLayoutInfo.parentalOffsetY) < kFloatEpsilon)
				{
					currentObjectLayoutInfo.applyingTransform = parentObjectLayoutInfo.applyingTransform;
				}
				else
				{
					currentObjectLayoutInfo.applyingTransform = parentObjectLayoutInfo.applyingTransform->Combine(OffsetTransform::AllocateTransform(currentObjectLayoutInfo.parentalOffsetX, currentObjectLayoutInfo.parentalOffsetY));
				}
			}

			// Make Clipping
			std::shared_ptr<IUIClippingComponent> clippingComponent = targetObject.GetComponentObject<IUIClippingComponent>(UIComponentType::kClipping);
			if (clippingComponent && clippingComponent->IsClipping())
			{
				currentObjectLayoutInfo.applyingClipping = clippingComponent->GetEffectiveClipping(parentObjectLayoutInfo.applyingClipping, currentObjectLayoutInfo.applyingTransform);
			}
			else
			{
				currentObjectLayoutInfo.applyingClipping = parentObjectLayoutInfo.applyingClipping;
			}
		}
	}
}
