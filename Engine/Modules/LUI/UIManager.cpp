////////////////////////////////////////////////////////////////////////////////
// UIManager.cpp (Leggiero/Modules - LegacyUI)
//
// UI Manager implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIManager.h"

// Standard Library
#include <functional>
#include <unordered_set>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppTextInputController.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsReferenceState.h>
#include <Graphics/GraphicResourceManagerComponent.h>

// Leggiero.LegacyUI
#include "Common/UICommonResources.h"
#include "Common/UITransform.h"
#include "Component/UILayoutComponent.h"
#include "Rendering/UIRenderer.h"
#include "Rendering/UIClipping.h"
#include "Rendering/UIShaders.h"
#include "Touch/UITouchNegotiator.h"
#include "UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIManager

		//------------------------------------------------------------------------------
		UIManager::InputDialogCallbackType UIManager::ms_dummyInputDialogCallback([](bool _0, GameDataString _1) { });

		//------------------------------------------------------------------------------
		UIManager::UIManager()
			: m_pApplicationComponent(nullptr)
			, m_objectIdIssuer(1)
			, m_isInputDialogWaiting(false)
			, m_isInitialized(false), m_isPaused(false)
		{
		}

		//------------------------------------------------------------------------------
		UIManager::~UIManager()
		{
		}

		//------------------------------------------------------------------------------
		// Initialize using common resource
		void UIManager::Initialize(Application::ApplicationComponent *applicationComponent, UICommonResources *commonResources)
		{
			if (m_isInitialized.load())
			{
				return;
			}

			m_pApplicationComponent = applicationComponent;

			m_renderer = std::make_shared<UIRenderer>(shared_from_this(),
				commonResources->m_uiColorShader, commonResources->m_uiTextureShader, commonResources->m_uiBlurShader);
			m_touchNegotiator = std::make_shared<UITouchNegotiator>();

			m_root = std::make_shared<UIObject>(shared_from_this());

			m_isPaused.store(false);
			m_inputDialogCallback = ms_dummyInputDialogCallback;

			m_isInitialized.store(true);
		}

		//------------------------------------------------------------------------------
		// Initialize with own resource creation
		void UIManager::Initialize(Application::ApplicationComponent *applicationComponent, Graphics::GraphicResourceManagerComponent *resourceManager)
		{
			if (m_isInitialized.load())
			{
				return;
			}

			m_pApplicationComponent = applicationComponent;

			m_renderer = std::make_shared<UIRenderer>(shared_from_this(),
				Shaders::UIColorPrimitiveShader::Create(resourceManager), 
				Shaders::UITextureColorShader::Create(resourceManager),
				Shaders::UITextureBlurShader::Create(resourceManager));

			m_touchNegotiator = std::make_shared<UITouchNegotiator>();

			m_root = std::make_shared<UIObject>(shared_from_this());

			m_isPaused.store(false);
			m_inputDialogCallback = ms_dummyInputDialogCallback;

			m_isInitialized.store(true);
		}

		//------------------------------------------------------------------------------
		// Process Touch Events
		void UIManager::ProcessTouchEvents(const std::vector<Input::Touch::TouchEvent> &touchEvents, GameTimeClockType::time_point frameTime)
		{
			if (!m_isInitialized.load() || m_isPaused.load())
			{
				return;
			}

			if (m_isInputDialogWaiting.load())
			{
				return;
			}

			m_touchNegotiator->ProcessTouchEvents(touchEvents, frameTime, *this);
		}

		//------------------------------------------------------------------------------
		// Update UI Frame Logic
		void UIManager::UpdateFrame(GameTimeClockType::duration frameInterval)
		{
			_ProcessDelayedUIActions();

			// Update Input Dialog
			if (m_isInputDialogWaiting.load())
			{
				if (!m_pApplicationComponent->TextInputController().IsTextInputDialogOpen())
				{					
					m_inputDialogCallback(m_pApplicationComponent->TextInputController().IsDialogClosedOK(), m_pApplicationComponent->TextInputController().GetTextInputDialogText());
					m_inputDialogCallback = ms_dummyInputDialogCallback;
					m_isInputDialogWaiting.store(false);
				}
			}

			if (!m_isInitialized.load() || m_isPaused.load())
			{
				return;
			}

			// Do update in global ui lock context
			pthread_mutex_t *pLock = &m_uiUpdateLock.GetLock();
			int lockResult = pthread_mutex_lock(pLock);
			if (lockResult == 0)
			{
				auto releaseLockFunc = [pLock]() mutable { pthread_mutex_unlock(pLock); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_root->IterateUIJob(
					[frameInterval](std::shared_ptr<UIObject> target) { target->UpdateFrame(frameInterval); return false; },
					[](std::shared_ptr<UIObject> target) { return target->IsUpdating(); }
				);
			}
			else
			{
				// Lock Failed, but anyway go
				m_root->IterateUIJob(
					[frameInterval](std::shared_ptr<UIObject> target) { target->UpdateFrame(frameInterval); return false; },
					[](std::shared_ptr<UIObject> target) { return target->IsUpdating(); }
				);
			}
		}

		//------------------------------------------------------------------------------
		// Update UI Layout
		void UIManager::UpdateLayout()
		{
			if (!m_isInitialized.load())
			{
				return;
			}

			// Do update in global ui lock context
			pthread_mutex_t *pLock = &m_uiUpdateLock.GetLock();
			int lockResult = pthread_mutex_lock(pLock);
			if (lockResult == 0)
			{
				auto releaseLockFunc = [pLock]() mutable { pthread_mutex_unlock(pLock); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				_BuildLayoutTable();
			}
			else
			{
				// Lock Failed, but anyway go
				_BuildLayoutTable();
			}
		}

		//------------------------------------------------------------------------------
		// Prepare Graphic Resources
		// Should be called in Graphic Thread
		void UIManager::GraphicPrepare()
		{
			if (!m_isInitialized.load())
			{
				return;
			}

			// Do update in global ui lock context
			pthread_mutex_t *pLock = &m_uiUpdateLock.GetLock();
			int lockResult = pthread_mutex_lock(pLock);
			if (lockResult == 0)
			{
				auto releaseLockFunc = [pLock]() mutable { pthread_mutex_unlock(pLock); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_root->IterateUIJob(
					[](std::shared_ptr<UIObject> target) { target->GraphicPrepare(); return false; }
				);
			}
			else
			{
				// Lock Failed. Don't do prepare job
			}
		}

		//------------------------------------------------------------------------------
		// Pause UI System
		void UIManager::Pause()
		{
			if (!m_isInitialized.load())
			{
				return;
			}

			m_isPaused.store(true);
			m_touchNegotiator->ClearAllTouches();
		}

		//------------------------------------------------------------------------------
		// Resume UI System
		void UIManager::Resume()
		{
			m_isPaused.store(false);
		}

		//------------------------------------------------------------------------------
		// Flush current states
		void UIManager::Flush()
		{
			if (!m_isInitialized.load())
			{
				return;
			}

			if (m_isInputDialogWaiting.load())
			{
				m_inputDialogCallback(false, "");
				m_inputDialogCallback = ms_dummyInputDialogCallback;
				m_isInputDialogWaiting.store(false);
			}

			m_touchNegotiator->ClearAllTouches();

			ClearDelayedUIAction();
		}

		//------------------------------------------------------------------------------
		void UIManager::HintScreenDimensionChanged()
		{
			// Do update in global ui lock context
			pthread_mutex_t *pLock = &m_uiUpdateLock.GetLock();
			int lockResult = pthread_mutex_lock(pLock);
			if (lockResult == 0)
			{
				auto releaseLockFunc = [pLock]() mutable { pthread_mutex_unlock(pLock); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_root->IterateUIJob(
					[](std::shared_ptr<UIObject> target) { target->HintScreenDimensionChanged(); return false; }
				);
			}
		}

		//------------------------------------------------------------------------------
		// Add Top-Level UI Object into the specified poition
		// if beforeOf is nullptr, the position will be top
		void UIManager::AddTopUIObject(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf)
		{
			if (!object)
			{
				return;
			}
			RemoveTopUIObject(object);

			if (!beforeOf)
			{
				m_root->m_preChildren.push_back(object);
				object->UpdateParent(*m_root);
				return;
			}
			UIObjectIdType findId = beforeOf->GetId();
			std::vector<std::shared_ptr<UIObject> >::iterator findIter = m_root->m_preChildren.begin();
			while (findIter != m_root->m_preChildren.end())
			{
				if ((*findIter)->GetId() == findId)
				{
					break;
				}
				++findIter;
			}
			m_root->m_preChildren.insert(findIter, object);
			object->UpdateParent(*m_root);
		}

		//------------------------------------------------------------------------------
		// Add Top-Level UI Object into the top of current UI
		void UIManager::PopupTopUIObject(std::shared_ptr<UIObject> object)
		{
			if (!object)
			{
				return;
			}
			RemoveTopUIObject(object);

			m_root->m_preChildren.push_back(object);
			object->UpdateParent(*m_root);
		}

		//------------------------------------------------------------------------------
		// Add Top-Level UI Object into the bottom of current UI
		void UIManager::AddBackTopUIObject(std::shared_ptr<UIObject> object)
		{
			if (!object)
			{
				return;
			}
			RemoveTopUIObject(object);

			m_root->m_preChildren.insert(m_root->m_preChildren.begin(), object);
			object->UpdateParent(*m_root);
		}

		//------------------------------------------------------------------------------
		// Remove Top-Level UI Object
		void UIManager::RemoveTopUIObject(std::shared_ptr<UIObject> object)
		{
			if (!object)
			{
				return;
			}

			UIObjectIdType findId = object->GetId();
			std::vector<std::shared_ptr<UIObject> >::iterator removeIter = m_root->m_preChildren.begin();
			while (removeIter != m_root->m_preChildren.end())
			{
				if ((*removeIter)->GetId() == findId)
				{
					break;
				}
				++removeIter;
			}
			if (removeIter == m_root->m_preChildren.end())
			{
				// Not Exists
				return;
			}

			(*removeIter)->UpdateNoParent();
			m_root->m_preChildren.erase(removeIter);
		}

		//------------------------------------------------------------------------------
		// Remove the UI object from manager's managing context
		void UIManager::RemoveUIObject(std::shared_ptr<UIObject> object)
		{
			m_root->RemoveUIObject(object, true);
		}

		//------------------------------------------------------------------------------
		void UIManager::RequestDelayedUIAction(const UIManager::DelayedUIAction &action)
		{
			m_delayedActionQueue.enqueue(action);
		}

		//------------------------------------------------------------------------------
		void UIManager::RequestDelayedUIAction(UIManager::DelayedUIAction &&action)
		{
			m_delayedActionQueue.enqueue(action);
		}

		//------------------------------------------------------------------------------
		void UIManager::ClearDelayedUIAction()
		{
			DelayedUIAction buffer;
			while (m_delayedActionQueue.try_dequeue(buffer))
			{
			}
		}

		//------------------------------------------------------------------------------
		void UIManager::_ProcessDelayedUIActions()
		{
			DelayedUIAction buffer;
			while (m_delayedActionQueue.try_dequeue(buffer))
			{
				if (buffer)
				{
					buffer();
				}
			}
		}

		//------------------------------------------------------------------------------
		// Get cached layout information of that frame
		// Can be not represent exact current state. please use this with enough understanding of Leggiero Legacy UI system
		// This function does not copy or extend life cycle of information objects: the result should use immedeatly or be copied by user
		std::shared_ptr<CalculatedUILayoutInformation> UIManager::FindFrameCachedLayoutInformation(UIObjectIdType objectId)
		{
			std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> >::iterator findIt = m_frameLayoutInformationTable.find(objectId);
			if (findIt == m_frameLayoutInformationTable.end())
			{
				return nullptr;
			}
			return findIt->second;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<CalculatedUILayoutInformation> UIManager::_AllocateLayoutInformationObject()
		{
			if (m_layoutInformationObjectPool.empty())
			{
				return std::make_shared<CalculatedUILayoutInformation>();
			}
			else
			{
				std::shared_ptr<CalculatedUILayoutInformation> pooledObject(m_layoutInformationObjectPool.front());
				pooledObject->ResetInformation();
				m_layoutInformationObjectPool.pop_front();
				return pooledObject;
			}
		}

		//------------------------------------------------------------------------------
		void UIManager::_ReleaseLayoutInformationObject(std::shared_ptr<CalculatedUILayoutInformation> toRelease)
		{
			m_layoutInformationObjectPool.push_front(toRelease);
		}

		//------------------------------------------------------------------------------
		// * Should be called in update lock context
		void UIManager::_BuildLayoutTable()
		{
			std::unordered_set<UIObjectIdType> inTreeLayoutingObjects;
			m_root->IterateUIJob(
				[this, &inTreeLayoutingObjects](std::shared_ptr<UIObject> target) {
					if (target->IsVisible())
					{
						inTreeLayoutingObjects.insert(target->GetId());
						std::shared_ptr<CalculatedUILayoutInformation> informationObject(target->m_cachedLayoutInfo.lock());
						if (informationObject)
						{
							informationObject->ResetInformation();
							informationObject->objectId = target->GetId();
							informationObject->objectReference = target;
						}
						else
						{
							informationObject = _AllocateLayoutInformationObject();
							informationObject->objectId = target->GetId();
							informationObject->objectReference = target;
							m_frameLayoutInformationTable[target->GetId()] = informationObject;
							target->m_cachedLayoutInfo = informationObject;
						}
					}
					return false;
				},
				[](std::shared_ptr<UIObject> target) { return target->IsVisible(); }
				);
			if (inTreeLayoutingObjects.size() != m_frameLayoutInformationTable.size())
			{
				std::vector<UIObjectIdType> notInTreeObjects;
				notInTreeObjects.reserve(m_frameLayoutInformationTable.size());
				for (std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> >::iterator it = m_frameLayoutInformationTable.begin(); it != m_frameLayoutInformationTable.end(); ++it)
				{
					if (inTreeLayoutingObjects.find(it->first) == inTreeLayoutingObjects.end())
					{
						std::shared_ptr<UIObject> ownerObject(it->second->objectReference.lock());
						if (ownerObject)
						{
							ownerObject->m_cachedLayoutInfo.reset();
						}
						_ReleaseLayoutInformationObject(it->second);
						notInTreeObjects.push_back(it->first);
					}
				}
				for (const UIObjectIdType &idToRemove : notInTreeObjects)
				{
					m_frameLayoutInformationTable.erase(idToRemove);
				}
			}

			CalculatedUILayoutInformation &rootLayout(*(m_frameLayoutInformationTable[m_root->GetId()]));
			rootLayout.width = static_cast<UICoordinateType>(Graphics::ReferenceState::GetReferenceViewportWidth());
			rootLayout.height = static_cast<UICoordinateType>(Graphics::ReferenceState::GetReferenceViewportHeight());
			rootLayout.marginedWidth = rootLayout.width;
			rootLayout.marginedHeight = rootLayout.height;
			rootLayout.isSizeCalculated = true;
			rootLayout.parentalOffsetX = static_cast<UICoordinateType>(0.0f);
			rootLayout.parentalOffsetY = static_cast<UICoordinateType>(0.0f);
			rootLayout.isOffsetCalculated = true;
			rootLayout.applyingTransform = std::make_shared<IdentityTransform>();
			rootLayout.applyingClipping = IUIClipping::kAlwaysPass;

			m_root->UpdateLayout(nullptr, m_frameLayoutInformationTable);
		}

		//------------------------------------------------------------------------------
		bool UIManager::RequestOpenInputDialog(UIManager::InputDialogCallbackType closeCallbackFunc,
			const GameDataString &title, const GameDataString &message, const GameDataString &initialText, const GameDataString &acceptButton, const GameDataString &cancelButton)
		{
			// Prevent Double-Start
			bool expectedStartedValue = false;
			if (!m_isInputDialogWaiting.compare_exchange_strong(expectedStartedValue, true))
			{
				return false;
			}

			if (m_touchNegotiator)
			{
				m_touchNegotiator->ClearAllTouches();
			}

			m_inputDialogCallback = closeCallbackFunc;
			m_pApplicationComponent->TextInputController().OpenTextInputDialog(title.c_str(), message.c_str(), initialText, acceptButton.c_str(), cancelButton.c_str());

			return true;
		}
	}
}
