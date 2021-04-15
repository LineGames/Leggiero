////////////////////////////////////////////////////////////////////////////////
// UIManager.h (Leggiero/Modules - LegacyUI)
//
// UI Manager for a single context of UI processing
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__UI_MANAGER_H
#define __LM_LUI__UI_MANAGER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Input
#include <Input/Touch/TouchEvent.h>

// Leggiero.LegacyUI
#include "Common/UICommonTypes.h"


namespace Leggiero
{
	// Forward Declarations
	namespace Application
	{
		class ApplicationComponent;
	}

	namespace Graphics
	{
		class GraphicResourceManagerComponent;
	}


	namespace LUI
	{
		// Forward Declarations
		struct CalculatedUILayoutInformation;
		class UICommonResources;
		class UIRenderer;
		class UITouchNegotiator;
		class UIObject;
		

		// UI Manager
		class UIManager
			: public std::enable_shared_from_this<UIManager>
			, private Utility::SyntacticSugar::NonCopyable
		{
			friend class UIRenderer;
			friend class UITouchNegotiator;

		public:
			UIManager();
			virtual ~UIManager();

		private:
			UIManager(const UIManager &rhs) = delete;
			UIManager &operator=(const UIManager &rhs) = delete;

		public:
			// Need deferred initialization because of shared_ptr
			void Initialize(Application::ApplicationComponent *applicationComponent, UICommonResources *commonResources);
			void Initialize(Application::ApplicationComponent *applicationComponent, Graphics::GraphicResourceManagerComponent *resourceManager);

		public:	// Update Interface
			// Process Touch Events
			void ProcessTouchEvents(const std::vector<Input::Touch::TouchEvent> &touchEvents, GameTimeClockType::time_point frameTime);

			// Update UI Frame Logic
			void UpdateFrame(GameTimeClockType::duration frameInterval);

			// Update UI Layout
			void UpdateLayout();

		public:	// Control Interface
			// Prepare Graphic Resources
			void GraphicPrepare();

			// Pause and Resume UI System
			void Pause();
			void Resume();

			// Flush current states
			void Flush();

			void HintScreenDimensionChanged();

		public:
			UIRenderer &Renderer() { return *m_renderer; }

		public:	// UI Object System
			std::shared_ptr<UIObject> GetRootUIObject() { return m_root; }

			void AddTopUIObject(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf = nullptr);
			void PopupTopUIObject(std::shared_ptr<UIObject> object);
			void AddBackTopUIObject(std::shared_ptr<UIObject> object);
			void RemoveTopUIObject(std::shared_ptr<UIObject> object);

			void RemoveUIObject(std::shared_ptr<UIObject> object);

		public:	// UI Action
			// Type for an action that should be runned in UI thread
			using DelayedUIAction = std::function<void()>;

			void RequestDelayedUIAction(const DelayedUIAction &action);
			void RequestDelayedUIAction(DelayedUIAction &&action);
			void ClearDelayedUIAction();

		protected:
			void _ProcessDelayedUIActions();

		//------------------------------------------------------------------------------
		// Below is for UI system
		public:
			UIObjectIdType IssueObjectId() { return m_objectIdIssuer.fetch_add(1); }

		public:	// Input Dialog Processing
			using InputDialogCallbackType = std::function<void(bool, GameDataString)>;

			bool RequestOpenInputDialog(InputDialogCallbackType closeCallbackFunc,
				const GameDataString &title, const GameDataString &message, const GameDataString &initialText, const GameDataString &acceptButton, const GameDataString &cancelButton);

		public:
			// Get cached layout information of that frame
			std::shared_ptr<CalculatedUILayoutInformation> FindFrameCachedLayoutInformation(UIObjectIdType objectId);

		protected:
			std::shared_ptr<CalculatedUILayoutInformation> _AllocateLayoutInformationObject();
			void _ReleaseLayoutInformationObject(std::shared_ptr<CalculatedUILayoutInformation> toRelease);

			void _BuildLayoutTable();

		protected:
			Utility::Threading::SafePthreadLock m_uiUpdateLock;

			Application::ApplicationComponent *m_pApplicationComponent;

			std::atomic_bool m_isInitialized;
			std::atomic_bool m_isPaused;

			std::shared_ptr<UIObject> m_root;

			std::atomic<UIObjectIdType> m_objectIdIssuer;

			std::shared_ptr<UIRenderer>			m_renderer;
			std::shared_ptr<UITouchNegotiator>	m_touchNegotiator;

			std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> >	m_frameLayoutInformationTable;
			std::list<std::shared_ptr<CalculatedUILayoutInformation> >							m_layoutInformationObjectPool;

			moodycamel::ConcurrentQueue<DelayedUIAction> m_delayedActionQueue;

		protected:
			static InputDialogCallbackType		ms_dummyInputDialogCallback;
			InputDialogCallbackType				m_inputDialogCallback;

			std::atomic_bool					m_isInputDialogWaiting;
		};
	}
}

#endif
