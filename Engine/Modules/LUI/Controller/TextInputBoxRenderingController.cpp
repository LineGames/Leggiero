////////////////////////////////////////////////////////////////////////////////
// Controller/TextInputBoxRenderingController.cpp (Leggiero/Modules - LegacyUI)
//
// Common text input box rendering controller implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextInputBoxRenderingController.h"

// Leggiero.LegacyUI
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Controller
		{
			//////////////////////////////////////////////////////////////////////////////// SimpleFabricatedTextInputBoxRenderingController

			//------------------------------------------------------------------------------
			SimpleFabricatedTextInputBoxRenderingController::SimpleFabricatedTextInputBoxRenderingController(std::shared_ptr<UIObject> fabricatedBox, 
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingObjectId,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingContentsObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingContentsObjectId)
			{
				std::shared_ptr<UIFabricatedComponent> fabricatedComponent(fabricatedBox->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));
				if (!fabricatedComponent)
				{
					return;
				}

				normalBGRendering = fabricatedComponent->FindObject(normalRenderingObjectId);
				pushedBGRendering = fabricatedComponent->FindObject(pushedRenderingObjectId);
				normalContentsRendering = fabricatedComponent->FindObject(normalRenderingContentsObjectId);
				pushedContentsRendering = fabricatedComponent->FindObject(pushedRenderingContentsObjectId);

				if (pushedBGRendering)
				{
					pushedBGRendering->SetIsVisible(false);
				}
				if (pushedContentsRendering)
				{
					pushedContentsRendering->SetIsVisible(false);
				}
			}

			//------------------------------------------------------------------------------
			void SimpleFabricatedTextInputBoxRenderingController::UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::UIElementTextInputBox::StateType currentState, const GameDataString &currentTextValue)
			{
				if (currentState == Element::UIElementTextInputBox::StateType::kNormal)
				{
					if (normalBGRendering) normalBGRendering->SetIsVisible(true);
					if (pushedBGRendering) pushedBGRendering->SetIsVisible(false);
					if (normalContentsRendering) normalContentsRendering->SetIsVisible(true);
					if (pushedContentsRendering) pushedContentsRendering->SetIsVisible(false);
				}
				else if (Utility::SyntacticSugar::HasFlag(currentState, Element::UIElementTextInputBox::StateType::kPushed))
				{
					if (normalBGRendering) normalBGRendering->SetIsVisible(false);
					if (pushedBGRendering) pushedBGRendering->SetIsVisible(true);
					if (normalContentsRendering) normalContentsRendering->SetIsVisible(false);
					if (pushedContentsRendering) pushedContentsRendering->SetIsVisible(true);
				}
			}
		}
	}
}
