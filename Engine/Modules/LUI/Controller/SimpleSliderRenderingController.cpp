////////////////////////////////////////////////////////////////////////////////
// Controller/SimpleSliderRenderingController.cpp (Leggiero/Modules - LegacyUI)
//
// Simple slider basic rendering controller implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SimpleSliderRenderingController.h"

// Leggiero.LegacyUI
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Controller
		{
			//////////////////////////////////////////////////////////////////////////////// SimpleSliderBasicRenderingController

			//------------------------------------------------------------------------------
			SimpleSliderBasicRenderingController::SimpleSliderBasicRenderingController(std::shared_ptr<UIObject> marker, std::shared_ptr<UIObject> bar,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingMarkerObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingMarkerObjectId,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingMarkerContentsObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingMarkerContentsObjectId,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingBarObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingBarObjectId,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingBarContentsObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingBarContentsObjectId)
			{
				if (marker)
				{
					std::shared_ptr<UIFabricatedComponent> markerFabricatedComponent(marker->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));
					if (markerFabricatedComponent)
					{
						normalBGRenderingMarker = markerFabricatedComponent->FindObject(normalRenderingMarkerObjectId);
						pushedBGRenderingMarker = markerFabricatedComponent->FindObject(pushedRenderingMarkerObjectId);
						normalContentsRenderingMarker = markerFabricatedComponent->FindObject(normalRenderingMarkerContentsObjectId);
						pushedContentsRenderingMarker = markerFabricatedComponent->FindObject(pushedRenderingMarkerContentsObjectId);
					}
				}
				if (bar)
				{
					std::shared_ptr<UIFabricatedComponent> barFabricatedComponent(bar->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));
					if (barFabricatedComponent)
					{
						normalBGRenderingBar = barFabricatedComponent->FindObject(normalRenderingBarObjectId);
						pushedBGRenderingBar = barFabricatedComponent->FindObject(pushedRenderingBarObjectId);
						normalContentsRenderingBar = barFabricatedComponent->FindObject(normalRenderingBarContentsObjectId);
						pushedContentsRenderingBar = barFabricatedComponent->FindObject(pushedRenderingBarContentsObjectId);
					}
				}
			}

			//------------------------------------------------------------------------------
			void SimpleSliderBasicRenderingController::UpdateFrameSelf(GameTimeClockType::duration frameInterval, bool isUsing)
			{
				if (isUsing)
				{
					if (normalBGRenderingMarker) normalBGRenderingMarker->SetIsVisible(false);
					if (pushedBGRenderingMarker) pushedBGRenderingMarker->SetIsVisible(true);
					if (normalContentsRenderingMarker) normalContentsRenderingMarker->SetIsVisible(false);
					if (pushedContentsRenderingMarker) pushedContentsRenderingMarker->SetIsVisible(true);

					if (normalBGRenderingBar) normalBGRenderingBar->SetIsVisible(false);
					if (pushedBGRenderingBar) pushedBGRenderingBar->SetIsVisible(true);
					if (normalContentsRenderingBar) normalContentsRenderingBar->SetIsVisible(false);
					if (pushedContentsRenderingBar) pushedContentsRenderingBar->SetIsVisible(true);
				}
				else
				{
					if (normalBGRenderingMarker) normalBGRenderingMarker->SetIsVisible(true);
					if (pushedBGRenderingMarker) pushedBGRenderingMarker->SetIsVisible(false);
					if (normalContentsRenderingMarker) normalContentsRenderingMarker->SetIsVisible(true);
					if (pushedContentsRenderingMarker) pushedContentsRenderingMarker->SetIsVisible(false);

					if (normalBGRenderingBar) normalBGRenderingBar->SetIsVisible(true);
					if (pushedBGRenderingBar) pushedBGRenderingBar->SetIsVisible(false);
					if (normalContentsRenderingBar) normalContentsRenderingBar->SetIsVisible(true);
					if (pushedContentsRenderingBar) pushedContentsRenderingBar->SetIsVisible(false);
				}
			}
		}
	}
}
