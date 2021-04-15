////////////////////////////////////////////////////////////////////////////////
// Controller/CommonButtonRenderingController.cpp (Leggiero/Modules - LegacyUI)
//
// Basic controllers implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CommonButtonRenderingController.h"

// Leggiero.LegacyUI
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Controller
		{
			//////////////////////////////////////////////////////////////////////////////// SimpleButtonRenderingController

			//------------------------------------------------------------------------------
			SimpleButtonRenderingController::SimpleButtonRenderingController(UIObject &button,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingObjectId, 
				const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingContentsObjectId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingContentsObjectId)
			{
				std::shared_ptr<UIFabricatedComponent> fabricatedComponent(button.GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));

				normalRendering = fabricatedComponent->FindObject(normalRenderingObjectId);
				pushedRendering = fabricatedComponent->FindObject(pushedRenderingObjectId);

				normalRenderingContents = fabricatedComponent->FindObject(normalRenderingContentsObjectId);
				pushedRenderingContents = fabricatedComponent->FindObject(pushedRenderingContentsObjectId);
			}

			//------------------------------------------------------------------------------
			void SimpleButtonRenderingController::UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState)
			{
				switch (currentState)
				{
					case Element::SingleSimpleButton::StateType::kNormal:
						{
							if (normalRendering) normalRendering->SetIsVisible(true);
							if (pushedRendering) pushedRendering->SetIsVisible(false);

							if (normalRenderingContents) normalRenderingContents->SetIsVisible(true);
							if (pushedRenderingContents) pushedRenderingContents->SetIsVisible(false);
						}
						break;

					case Element::SingleSimpleButton::StateType::kPushed:
						{
							if (normalRendering) normalRendering->SetIsVisible(false);
							if (pushedRendering) pushedRendering->SetIsVisible(true);

							if (normalRenderingContents) normalRenderingContents->SetIsVisible(false);
							if (pushedRenderingContents) pushedRenderingContents->SetIsVisible(true);
						}
						break;

					default:
						{
							if (normalRendering) normalRendering->SetIsVisible(true);
							if (pushedRendering) pushedRendering->SetIsVisible(false);

							if (normalRenderingContents) normalRenderingContents->SetIsVisible(true);
							if (pushedRenderingContents) pushedRenderingContents->SetIsVisible(false);
						}
						break;
				}
			}


			/////////////////////////////////////////////////////////////////////////////// ActivatableButtonRenderingController

			//------------------------------------------------------------------------------
			ActivatableButtonRenderingController::ActivatableButtonRenderingController(UIObject &button,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalBGId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedBGId, const UIFabricatedComponent::ObjectDescriptionIdType &activeBGId,
				const UIFabricatedComponent::ObjectDescriptionIdType &normalContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &pushedContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &activeContentsId)
			{
				std::shared_ptr<UIFabricatedComponent> fabricatedComponent(button.GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));

				m_bgNormal = fabricatedComponent->FindObject(normalBGId);
				m_bgPush = fabricatedComponent->FindObject(pushedBGId);
				m_bgActive = fabricatedComponent->FindObject(activeBGId);

				m_contentsNormal = fabricatedComponent->FindObject(normalContentsId);
				m_contentsPush = fabricatedComponent->FindObject(pushedContentsId);
				m_contentsNonActive = fabricatedComponent->FindObject(inactiveContentsId);
				m_contentsActive = fabricatedComponent->FindObject(activeContentsId);
			}

			//------------------------------------------------------------------------------
			void ActivatableButtonRenderingController::UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState)
			{
				if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kActive))
				{
					if (m_bgNormal) m_bgNormal->SetIsVisible(false);
					if (m_bgPush) m_bgPush->SetIsVisible(false);
					if (m_bgActive) m_bgActive->SetIsVisible(true);

					if (m_contentsNormal) m_contentsNormal->SetIsVisible(false);
					if (m_contentsPush) m_contentsPush->SetIsVisible(false);
					if (m_contentsNonActive) m_contentsNonActive->SetIsVisible(false);
					if (m_contentsActive) m_contentsActive->SetIsVisible(true);
				}
				else
				{
					if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kPushed))
					{
						if (m_bgNormal) m_bgNormal->SetIsVisible(false);
						if (m_bgPush) m_bgPush->SetIsVisible(true);
						if (m_bgActive) m_bgActive->SetIsVisible(false);

						if (m_contentsNormal) m_contentsNormal->SetIsVisible(false);
						if (m_contentsPush) m_contentsPush->SetIsVisible(true);
						if (m_contentsNonActive) m_contentsNonActive->SetIsVisible(true);
						if (m_contentsActive) m_contentsActive->SetIsVisible(false);
					}
					else
					{
						if (m_bgNormal) m_bgNormal->SetIsVisible(true);
						if (m_bgPush) m_bgPush->SetIsVisible(false);
						if (m_bgActive) m_bgActive->SetIsVisible(false);

						if (m_contentsNormal) m_contentsNormal->SetIsVisible(true);
						if (m_contentsPush) m_contentsPush->SetIsVisible(false);
						if (m_contentsNonActive) m_contentsNonActive->SetIsVisible(true);
						if (m_contentsActive) m_contentsActive->SetIsVisible(false);
					}
				}
			}


			//////////////////////////////////////////////////////////////////////////////// ActiveDisableButtonRenderingController

			//------------------------------------------------------------------------------
			ActiveDisableButtonRenderingController::ActiveDisableButtonRenderingController(UIObject &button,
				const UIFabricatedComponent::ObjectDescriptionIdType &inactiveCommonBGId, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveNormalBGId, const UIFabricatedComponent::ObjectDescriptionIdType &inactivePushedBGId,
				const UIFabricatedComponent::ObjectDescriptionIdType &activeCommonBGId, const UIFabricatedComponent::ObjectDescriptionIdType &activeNormalBGId, const UIFabricatedComponent::ObjectDescriptionIdType &activePushedBGId,
				const UIFabricatedComponent::ObjectDescriptionIdType &disabledBGId,
				const UIFabricatedComponent::ObjectDescriptionIdType &inactiveCommonContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveNormalContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &inactivePushedContentsId,
				const UIFabricatedComponent::ObjectDescriptionIdType &activeCommonContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &activeNormalContentsId, const UIFabricatedComponent::ObjectDescriptionIdType &activePushedContentsId,
				const UIFabricatedComponent::ObjectDescriptionIdType &disabledContentsId)
			{
				std::shared_ptr<UIFabricatedComponent> fabricatedComponent(button.GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));

				inactiveCommonBG = fabricatedComponent->FindObject(inactiveCommonBGId);
				normalBG = fabricatedComponent->FindObject(inactiveNormalBGId);
				pushedBG = fabricatedComponent->FindObject(inactivePushedBGId);
				activeCommonBG = fabricatedComponent->FindObject(activeCommonBGId);
				activeNormalBG = fabricatedComponent->FindObject(activeNormalBGId);
				activePushedBG = fabricatedComponent->FindObject(activePushedBGId);
				disabledBG = fabricatedComponent->FindObject(disabledBGId);

				inactiveCommonContents = fabricatedComponent->FindObject(inactiveCommonContentsId);
				normalContents = fabricatedComponent->FindObject(inactiveNormalContentsId);
				pushedContents = fabricatedComponent->FindObject(inactivePushedContentsId);
				activeCommonContents = fabricatedComponent->FindObject(activeCommonContentsId);
				activeNormalContents = fabricatedComponent->FindObject(activeNormalContentsId);
				activePushedContents = fabricatedComponent->FindObject(activePushedContentsId);
				disabledContents = fabricatedComponent->FindObject(disabledContentsId);
			}

			//------------------------------------------------------------------------------
			void ActiveDisableButtonRenderingController::UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState)
			{
				if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kDisabled))
				{
					if (inactiveCommonBG) inactiveCommonBG->SetIsVisible(false);
					if (normalBG) normalBG->SetIsVisible(false);
					if (pushedBG) pushedBG->SetIsVisible(false);
					if (activeCommonBG) activeCommonBG->SetIsVisible(false);
					if (activeNormalBG) activeNormalBG->SetIsVisible(false);
					if (activePushedBG) activePushedBG->SetIsVisible(false);
					if (disabledBG) disabledBG->SetIsVisible(true);

					if (inactiveCommonContents) inactiveCommonBG->SetIsVisible(false);
					if (normalContents) normalContents->SetIsVisible(false);
					if (pushedContents) pushedContents->SetIsVisible(false);
					if (activeCommonContents) activeCommonContents->SetIsVisible(false);
					if (activeNormalContents) activeNormalContents->SetIsVisible(false);
					if (activePushedContents) activePushedContents->SetIsVisible(false);
					if (disabledContents) disabledContents->SetIsVisible(true);
				}
				else if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kActive))
				{
					if (inactiveCommonBG) inactiveCommonBG->SetIsVisible(false);
					if (normalBG) normalBG->SetIsVisible(false);
					if (pushedBG) pushedBG->SetIsVisible(false);
					if (activeCommonBG) activeCommonBG->SetIsVisible(true);
					if (disabledBG) disabledBG->SetIsVisible(false);

					if (inactiveCommonContents) inactiveCommonBG->SetIsVisible(false);
					if (normalContents) normalContents->SetIsVisible(false);
					if (pushedContents) pushedContents->SetIsVisible(false);
					if (activeCommonContents) activeCommonContents->SetIsVisible(true);
					if (disabledContents) disabledContents->SetIsVisible(false);

					if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kPushed))
					{
						if (activeNormalBG) activeNormalBG->SetIsVisible(false);
						if (activePushedBG) activePushedBG->SetIsVisible(true);

						if (activeNormalContents) activeNormalContents->SetIsVisible(false);
						if (activePushedContents) activePushedContents->SetIsVisible(true);
					}
					else
					{
						if (activeNormalBG) activeNormalBG->SetIsVisible(true);
						if (activePushedBG) activePushedBG->SetIsVisible(false);

						if (activeNormalContents) activeNormalContents->SetIsVisible(true);
						if (activePushedContents) activePushedContents->SetIsVisible(false);
					}
				}
				else
				{
					if (inactiveCommonBG) inactiveCommonBG->SetIsVisible(true);
					if (activeCommonBG) activeCommonBG->SetIsVisible(false);
					if (activeNormalBG) activeNormalBG->SetIsVisible(false);
					if (activePushedBG) activePushedBG->SetIsVisible(false);
					if (disabledBG) disabledBG->SetIsVisible(false);

					if (inactiveCommonContents) inactiveCommonBG->SetIsVisible(true);
					if (activeCommonContents) activeCommonContents->SetIsVisible(false);
					if (activeNormalContents) activeNormalContents->SetIsVisible(false);
					if (activePushedContents) activePushedContents->SetIsVisible(false);
					if (disabledContents) disabledContents->SetIsVisible(false);

					if (Utility::SyntacticSugar::HasFlag(currentState, Element::SingleSimpleButton::StateType::kPushed))
					{
						if (normalBG) normalBG->SetIsVisible(false);
						if (pushedBG) pushedBG->SetIsVisible(true);

						if (normalContents) normalContents->SetIsVisible(false);
						if (pushedContents) pushedContents->SetIsVisible(true);
					}
					else
					{
						if (normalBG) normalBG->SetIsVisible(true);
						if (pushedBG) pushedBG->SetIsVisible(false);

						if (normalContents) normalContents->SetIsVisible(true);
						if (pushedContents) pushedContents->SetIsVisible(false);
					}
				}
			}
		}
	}
}
