////////////////////////////////////////////////////////////////////////////////
// Touch/UITouchNegotiator.cpp (Leggiero/Modules - LegacyUI)
//
// UI touch processing logic over UI things
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITouchNegotiator.h"

// Standard Library
#include <set>

// Leggiero.LegacyUI
#include "../UIManager.h"
#include "../UIObject.h"
#include "../Component/UIComponentType.h"
#include "../Component/IUIComponent.h"
#include "UITouchComponent.h"
#include "../Common/IUIArea.h"
#include "UITouch.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UITouchNegotiator

		//------------------------------------------------------------------------------
		UITouchNegotiator::UITouchNegotiator()
		{

		}

		//------------------------------------------------------------------------------
		UITouchNegotiator::~UITouchNegotiator()
		{
			ClearAllTouches();
		}

		//------------------------------------------------------------------------------
		// Do touch event processing
		void UITouchNegotiator::ProcessTouchEvents(const std::vector<Input::Touch::TouchEvent> &touchEvents, GameTimeClockType::time_point frameTime, UIManager &processingManager)
		{
			// Update Touch State by Other Changes
			std::unordered_set<UIObjectIdType> objectInTree;
			std::shared_ptr<UITreeCopyEntry> treeSnapshot;
			for (std::unordered_map<Input::TouchIdType, std::shared_ptr<UITouch> >::iterator it = m_activeTouches.begin(); it != m_activeTouches.end(); ++it)
			{
				if (!treeSnapshot)
				{
					treeSnapshot = _CopyUITree(processingManager.GetRootUIObject(), &objectInTree);
				}
				StructuralVersionType uiVersionBeforeProcessing = processingManager.GetRootUIObject()->GetStructuralVersion();

				// Extract All State
				std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > allTouchState;
				_ExtractTouchState(treeSnapshot, it->second, false, allTouchState);

				// Process By Touch(Subscribed)
				std::unordered_set<UIObjectIdType> processedObjects;
				it->second->UpdateVirtualTouchChangeTime(frameTime);
				it->second->_UpdateTouchState(allTouchState, processedObjects, objectInTree);

				// Process Touch In
				_DoTouchUpdateProcess(treeSnapshot, it->second, allTouchState, processedObjects);

				// Update All Touch-In Objects
				std::unordered_set<UIObjectIdType> touchInObjects;
				for (std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator touchInCheckIt = allTouchState.begin(); touchInCheckIt != allTouchState.end(); ++touchInCheckIt)
				{
					if (!std::get<1>(touchInCheckIt->second) || std::get<2>(touchInCheckIt->second))
					{
						touchInObjects.insert(touchInCheckIt->first);
					}
				}
				it->second->m_onceInObjects.insert(touchInObjects.cbegin(), touchInObjects.cend());
				it->second->m_lastTouchInObjects.swap(touchInObjects);

				if (processingManager.GetRootUIObject()->GetStructuralVersion() != uiVersionBeforeProcessing)
				{
					_ReleaseUITreeCopy(treeSnapshot);
					treeSnapshot = nullptr;
					objectInTree.clear();
				}
			}

			// Update Touch State by Events
			for (const Input::Touch::TouchEvent &currentEvent : touchEvents)
			{
				std::shared_ptr<UITouch> eventTouchObject;
				std::unordered_map<Input::TouchIdType, std::shared_ptr<UITouch> >::iterator findIt = m_activeTouches.find(currentEvent.touchId);
				if (currentEvent.eventType == Input::Touch::TouchEventType::kDown)
				{
					if (findIt != m_activeTouches.end())
					{
						findIt->second->_ProcessTouchCancelEvent();
						m_activeTouches.erase(currentEvent.touchId);
					}
					eventTouchObject = std::make_shared<UITouch>(currentEvent.touchId, static_cast<UICoordinateType>(currentEvent.x), static_cast<UICoordinateType>(currentEvent.y), currentEvent.eventTime);
					m_activeTouches[currentEvent.touchId] = eventTouchObject;
				}
				else
				{
					if (findIt != m_activeTouches.end())
					{
						eventTouchObject = findIt->second;
						eventTouchObject->UpdateTouchDataUsingEvent(currentEvent);
					}
				}
				if (!eventTouchObject)
				{
					continue;
				}

				switch (currentEvent.eventType)
				{
					case Input::Touch::TouchEventType::kDown:
						{
							if (!treeSnapshot)
							{
								treeSnapshot = _CopyUITree(processingManager.GetRootUIObject(), nullptr);
							}
							StructuralVersionType uiVersionBeforeProcessing = processingManager.GetRootUIObject()->GetStructuralVersion();

							_DoDownTouchProcess(treeSnapshot, eventTouchObject);

							if (processingManager.GetRootUIObject()->GetStructuralVersion() != uiVersionBeforeProcessing)
							{
								_ReleaseUITreeCopy(treeSnapshot);
								treeSnapshot = nullptr;
							}
						}
						break;

					case Input::Touch::TouchEventType::kMove:
						{
							if (!treeSnapshot)
							{
								treeSnapshot = _CopyUITree(processingManager.GetRootUIObject(), nullptr);
							}
							StructuralVersionType uiVersionBeforeProcessing = processingManager.GetRootUIObject()->GetStructuralVersion();

							// Extract All State
							std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > allTouchState;
							_ExtractTouchState(treeSnapshot, eventTouchObject, false, allTouchState);

							// Process By Touch(Subscribed)
							std::unordered_set<UIObjectIdType> processedObjects;
							eventTouchObject->_ProcessTouchMoveEvent(allTouchState, processedObjects);

							// Process Touch In
							_DoTouchMoveProcess(treeSnapshot, eventTouchObject, allTouchState, processedObjects);

							// Update All Touch-In Objects
							std::unordered_set<UIObjectIdType> touchInObjects;
							for (std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator it = allTouchState.begin(); it != allTouchState.end(); ++it)
							{
								if (!std::get<1>(it->second) || std::get<2>(it->second))
								{
									touchInObjects.insert(it->first);
								}
							}
							eventTouchObject->m_onceInObjects.insert(touchInObjects.cbegin(), touchInObjects.cend());
							eventTouchObject->m_lastTouchInObjects.swap(touchInObjects);

							if (processingManager.GetRootUIObject()->GetStructuralVersion() != uiVersionBeforeProcessing)
							{
								_ReleaseUITreeCopy(treeSnapshot);
								treeSnapshot = nullptr;
							}
						}
						break;

					case Input::Touch::TouchEventType::kUp:
						{
							eventTouchObject->_ProcessTouchUpEvent();
						}
						break;

					case Input::Touch::TouchEventType::kCancel:
						{
							eventTouchObject->_ProcessTouchCancelEvent();
						}
						break;
				}

				if (currentEvent.eventType == Input::Touch::TouchEventType::kUp || currentEvent.eventType == Input::Touch::TouchEventType::kCancel)
				{
					m_activeTouches.erase(currentEvent.touchId);
				}
			}
		}

		//------------------------------------------------------------------------------
		// Cancel all touches and clear touch processing state
		void UITouchNegotiator::ClearAllTouches()
		{
			for (std::unordered_map<Input::TouchIdType, std::shared_ptr<UITouch> >::iterator it = m_activeTouches.begin(); it != m_activeTouches.end(); ++it)
			{
				it->second->_ProcessTouchCancelEvent();
			}
			m_activeTouches.clear();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> UITouchNegotiator::_GetTreeCopyEntryObject()
		{
			std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> resultNode;
			if (m_treeCopyEntryPool.try_dequeue(resultNode))
			{
				return resultNode;
			}

			return std::make_shared<UITouchNegotiator::UITreeCopyEntry>();
		}

		//------------------------------------------------------------------------------
		void UITouchNegotiator::_ReleaseTreeCopyEntryObject(std::shared_ptr<UITreeCopyEntry> releasingObject)
		{
			if (!releasingObject)
			{
				return;
			}
			releasingObject->Clear();

			m_treeCopyEntryPool.enqueue(releasingObject);
		}

		//------------------------------------------------------------------------------
		// Build a tree represents current structure of UI objects with pointer
		std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> UITouchNegotiator::_CopyUITree(std::shared_ptr<UIObject> treeRoot, std::unordered_set<UIObjectIdType> *inTreeObjects)
		{
			if (!treeRoot->IsVisible() || !treeRoot->IsEnable())
			{
				return nullptr;
			}

			std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> treeNode = _GetTreeCopyEntryObject();
			treeNode->treeNode = treeRoot;
			if (inTreeObjects != nullptr)
			{
				inTreeObjects->insert(treeRoot->GetId());
			}

			treeNode->m_preChildren.reserve(treeRoot->m_preChildren.size());
			for (std::shared_ptr<UIObject> &currentChildEntry : treeRoot->m_preChildren)
			{
				std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> childTree(_CopyUITree(currentChildEntry, inTreeObjects));
				if (childTree)
				{
					treeNode->m_preChildren.push_back(childTree);
				}
			}
			treeNode->m_postChildren.reserve(treeRoot->m_postChildren.size());
			for (std::shared_ptr<UIObject> &currentChildEntry : treeRoot->m_postChildren)
			{
				std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> childTree(_CopyUITree(currentChildEntry, inTreeObjects));
				if (childTree)
				{
					treeNode->m_postChildren.push_back(childTree);
				}
			}

			return treeNode;
		}

		//------------------------------------------------------------------------------
		void UITouchNegotiator::_ReleaseUITreeCopy(std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> releasingTree)
		{
			if (!releasingTree)
			{
				return;
			}
			for (std::shared_ptr<UITreeCopyEntry> &currentCopiedTreeEntry : releasingTree->m_preChildren)
			{
				_ReleaseUITreeCopy(currentCopiedTreeEntry);
			}
			for (std::shared_ptr<UITreeCopyEntry> &currentCopiedTreeEntry : releasingTree->m_postChildren)
			{
				_ReleaseUITreeCopy(currentCopiedTreeEntry);
			}

			_ReleaseTreeCopyEntryObject(releasingTree);
		}

		//------------------------------------------------------------------------------
		// Process touch down for given UI tree and return whether it covered by
		bool UITouchNegotiator::_DoDownTouchProcess(std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject)
		{
			if (!treeRoot->treeNode->IsVisible() || !treeRoot->treeNode->IsEnable())
			{
				// Not Enabled Object (Tree)
				return false;
			}

			std::shared_ptr<UITouchComponent> touchComponent = treeRoot->treeNode->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);

			bool isCoverdByPostNode = false;
			int postChildrenCount = (int)treeRoot->m_postChildren.size();
			for (int i = postChildrenCount - 1; i >= 0; --i)
			{
				if (_DoDownTouchProcess(treeRoot->m_postChildren[i], touchObject))
				{
					isCoverdByPostNode = true;
					break;
				}
			}

			bool isCoveredByNode = false;
			if (touchComponent)
			{
				std::shared_ptr<IUIArea> coverArea = touchComponent->GetCoverArea();
				if (coverArea)
				{
					if (coverArea->IsInArea(touchObject->GetCurrentTouchPosition()))
					{
						isCoveredByNode = true;
					}
				}
			}

			if (!isCoverdByPostNode && !isCoveredByNode)
			{
				int preChildrenCount = (int)treeRoot->m_preChildren.size();
				for (int i = preChildrenCount - 1; i >= 0; --i)
				{
					if (_DoDownTouchProcess(treeRoot->m_preChildren[i], touchObject))
					{
						isCoveredByNode = true;
						break;
					}
				}
			}

			if (touchComponent)
			{
				std::shared_ptr<IUIArea> interactionArea = touchComponent->GetInteractionArea();
				if (interactionArea)
				{
					if (interactionArea->IsInArea(touchObject->GetCurrentTouchPosition()))
					{
						touchComponent->OnTouchDown(touchObject, touchObject->GetCurrentTouchX(), touchObject->GetCurrentTouchY(), touchObject->GetCurrentTouchChangeTime(), isCoverdByPostNode);
					}
				}
			}

			return (isCoveredByNode || isCoverdByPostNode);
		}

		//------------------------------------------------------------------------------
		// Calculate touch state for all UI objects
		bool UITouchNegotiator::_ExtractTouchState(std::shared_ptr<UITouchNegotiator::UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, bool isCovered, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &outTouchState)
		{
			if (!treeRoot->treeNode->IsVisible() || !treeRoot->treeNode->IsEnable())
			{
				// Not Enabled Object (Tree)
				return false;
			}

			std::shared_ptr<UITouchComponent> touchComponent = treeRoot->treeNode->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);

			bool isCoverdByPostNode = false;
			int postChildrenCount = (int)treeRoot->m_postChildren.size();
			for (int i = postChildrenCount - 1; i >= 0; --i)
			{
				isCoverdByPostNode |= _ExtractTouchState(treeRoot->m_postChildren[i], touchObject, (isCoverdByPostNode || isCovered), outTouchState);
			}

			bool isCoveredByNode = false;
			if (touchComponent)
			{
				std::shared_ptr<IUIArea> coverArea = touchComponent->GetCoverArea();
				if (coverArea)
				{
					if (coverArea->IsInArea(touchObject->GetCurrentTouchPosition()))
					{
						isCoveredByNode = true;
					}
				}
			}

			bool isCoverdByPreNode = false;
			int preChildrenCount = (int)treeRoot->m_preChildren.size();
			for (int i = preChildrenCount - 1; i >= 0; --i)
			{
				isCoverdByPreNode |= _ExtractTouchState(treeRoot->m_preChildren[i], touchObject, (isCoverdByPostNode || isCoveredByNode || isCoverdByPreNode || isCovered), outTouchState);
			}

			if (touchComponent)
			{
				std::shared_ptr<IUIArea> interactionArea = touchComponent->GetInteractionArea();
				if (interactionArea)
				{
					if (interactionArea->IsInArea(touchObject->GetCurrentTouchPosition()))
					{
						outTouchState.insert(std::pair<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >(treeRoot->treeNode->GetId(), std::make_tuple(treeRoot->treeNode, (isCovered || isCoverdByPostNode), (isCoverdByPostNode && !isCovered))));
					}
				}
			}

			return (isCoverdByPostNode || isCoveredByNode || isCoverdByPreNode);
		}

		//------------------------------------------------------------------------------
		// Process touch move for given UI tree
		void UITouchNegotiator::_DoTouchMoveProcess(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &processedObjects)
		{
			if (!treeRoot->treeNode->IsVisible() || !treeRoot->treeNode->IsEnable())
			{
				// Not Enabled Object (Tree)
				return;
			}

			int postChildrenCount = (int)treeRoot->m_postChildren.size();
			for (int i = postChildrenCount - 1; i >= 0; --i)
			{
				_DoTouchMoveProcess(treeRoot->m_postChildren[i], touchObject, allTouchState, processedObjects);
			}

			int preChildrenCount = (int)treeRoot->m_preChildren.size();
			for (int i = preChildrenCount - 1; i >= 0; --i)
			{
				_DoTouchMoveProcess(treeRoot->m_preChildren[i], touchObject, allTouchState, processedObjects);
			}

			UIObjectIdType currentObjectId = treeRoot->treeNode->GetId();
			if (processedObjects.find(currentObjectId) != processedObjects.end())
			{
				// Already Process by Touch
				return;
			}

			std::shared_ptr<UITouchComponent> touchComponent = treeRoot->treeNode->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
			if (touchComponent)
			{
				std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator stateIt = allTouchState.find(currentObjectId);
				if (stateIt != allTouchState.end())
				{
					if (!std::get<1>(stateIt->second) || std::get<2>(stateIt->second))
					{
						if (touchObject->m_lastTouchInObjects.find(currentObjectId) == touchObject->m_lastTouchInObjects.end())
						{
							// Out -> In by current event
							touchComponent->OnTouchIn(touchObject, touchObject->GetCurrentTouchX(), touchObject->GetCurrentTouchY(), touchObject->GetCurrentTouchChangeTime(),
								(touchObject->m_onceInObjects.find(currentObjectId) == touchObject->m_onceInObjects.end()),
								std::get<2>(stateIt->second));
						}
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		// Update touch state for tree
		void UITouchNegotiator::_DoTouchUpdateProcess(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &processedObjects)
		{
			if (!treeRoot->treeNode->IsVisible() || !treeRoot->treeNode->IsEnable())
			{
				// Not Enabled Object (Tree)
				return;
			}

			int postChildrenCount = (int)treeRoot->m_postChildren.size();
			for (int i = postChildrenCount - 1; i >= 0; --i)
			{
				_DoTouchUpdateProcess(treeRoot->m_postChildren[i], touchObject, allTouchState, processedObjects);
			}

			int preChildrenCount = (int)treeRoot->m_preChildren.size();
			for (int i = preChildrenCount - 1; i >= 0; --i)
			{
				_DoTouchUpdateProcess(treeRoot->m_preChildren[i], touchObject, allTouchState, processedObjects);
			}

			UIObjectIdType currentObjectId = treeRoot->treeNode->GetId();
			if (processedObjects.find(currentObjectId) != processedObjects.end())
			{
				// Already Process by Touch
				return;
			}

			std::shared_ptr<UITouchComponent> touchComponent = treeRoot->treeNode->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
			if (touchComponent)
			{
				std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator stateIt = allTouchState.find(currentObjectId);
				if (stateIt != allTouchState.end())
				{
					if (!std::get<1>(stateIt->second) || std::get<2>(stateIt->second))
					{
						if (touchObject->m_lastTouchInObjects.find(currentObjectId) == touchObject->m_lastTouchInObjects.end())
						{
							// Out -> In by current event
							touchComponent->OnTouchIn(touchObject, touchObject->GetCurrentTouchX(), touchObject->GetCurrentTouchY(), touchObject->GetCurrentTouchChangeTime(),
								(touchObject->m_onceInObjects.find(currentObjectId) == touchObject->m_onceInObjects.end()),
								std::get<2>(stateIt->second));
						}
					}
				}
			}
		}
	}
}