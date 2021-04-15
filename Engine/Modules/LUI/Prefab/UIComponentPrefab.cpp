////////////////////////////////////////////////////////////////////////////////
// Prefab/UIComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// UI Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIComponentPrefab.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UISizeSettable.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Description/UIDescriptionExpression.h"
#include "UIPrefabPlaceProcessor.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// IUIComponentPrefab

				//------------------------------------------------------------------------------
				bool IUIComponentPrefab::_IsSizeFittingNeeded(DescriptionProcessingContext &processingContext)
				{
					if (!m_isFitSizeExpression)
					{
						return false;
					}
					return m_isFitSizeExpression->Evaluate(processingContext);
				}


				//////////////////////////////////////////////////////////////////////////////// ComponentPlaceHolder

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> ComponentPlaceHolder::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					if (placeProcessor == nullptr)
					{
						return nullptr;
					}

					// Create
					std::shared_ptr<IUIComponent> createdComponent(placeProcessor->ProcessComponentPlaceholderFabrication(creatingManager, ownerObject, m_creatingKey, m_parameters, postProcessingContext));
					if (!createdComponent)
					{
						return nullptr;
					}

					return createdComponent;
				}


				//////////////////////////////////////////////////////////////////////////////// ComponentPostProcessCreatingContext

				//------------------------------------------------------------------------------
				void ComponentPostProcessCreatingContext::DoPostProcessAfterFabrication(std::shared_ptr<UIObject> createdObject, DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &parentPostProcessContext)
				{
					_DoFitSizingPostProess(createdObject, parentPostProcessContext);
				}

				//------------------------------------------------------------------------------
				void ComponentPostProcessCreatingContext::_DoFitSizingPostProess(std::shared_ptr<UIObject> createdObject, ComponentPostProcessCreatingContext &parentPostProcessContext)
				{
					if (!needToSizeSets.empty())
					{
						std::shared_ptr<UILayoutComponent> objectLayout(createdObject->GetLayoutComponent());
						if (objectLayout)
						{
							std::shared_ptr<IUISizeSubComponent> objectSizeSubComponent(objectLayout->GetSizeSubComponent());
							if (objectSizeSubComponent)
							{
								if (objectSizeSubComponent->HasParentDependency())
								{
									std::vector<std::shared_ptr<IRectSizeSettable> > &sizeSetTargetsCopy = needToSizeSets;
									parentPostProcessContext.delayedSizeSetsForParent.push_back([objectSizeSubComponent, sizeSetTargetsCopy]() {
										if (objectSizeSubComponent->IsParentDependencySolved())
										{
											UIElementSize objectSize = objectSizeSubComponent->CalculateSize();
											for (std::shared_ptr<IRectSizeSettable> currentWidgetToSetSize : sizeSetTargetsCopy)
											{
												currentWidgetToSetSize->SetUIWidgetRectSize(objectSize.width, objectSize.height);
											}
											return true;
										}
										return false;
										});
								}
								else
								{
									UIElementSize objectSize = objectSizeSubComponent->CalculateSize();
									for (std::shared_ptr<IRectSizeSettable> currentWidgetToSetSize : needToSizeSets)
									{
										currentWidgetToSetSize->SetUIWidgetRectSize(objectSize.width, objectSize.height);
									}
								}
							}
						}
					}

					// Delayed Sizing
					for (std::function<bool()> &currentDelayedSizingFun : delayedSizeSetsForParent)
					{
						bool resultValue = currentDelayedSizingFun();
						if (!resultValue)
						{
							parentPostProcessContext.delayedSizeSetsForParent.push_back(currentDelayedSizingFun);
						}
					}
				}
			}
		}
	}
}
