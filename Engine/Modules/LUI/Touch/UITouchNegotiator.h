////////////////////////////////////////////////////////////////////////////////
// Touch/UITouchNegotiator.h (Leggiero/Modules - LegacyUI)
//
// Processing touches for UI context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__TOUCH__UI_TOUCH_NEGOTIATOR_H
#define __LM_LUI__TOUCH__UI_TOUCH_NEGOTIATOR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Input
#include <Input/Touch/TouchEvent.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;
		class UITouch;


		// UI Touch Negotiator
		class UITouchNegotiator
		{
		public:
			UITouchNegotiator();
			virtual ~UITouchNegotiator();

		public:
			void ProcessTouchEvents(const std::vector<Input::Touch::TouchEvent> &touchEvents, GameTimeClockType::time_point frameTime, UIManager &processingManager);
			void ClearAllTouches();

		protected:
			std::unordered_map<Input::TouchIdType, std::shared_ptr<UITouch> > m_activeTouches;

		protected:
			// Copied UI tree node entry to deal with tree changing during processing
			struct UITreeCopyEntry
			{
				std::shared_ptr<UIObject> treeNode;
				std::vector<std::shared_ptr<UITreeCopyEntry> > m_preChildren;
				std::vector<std::shared_ptr<UITreeCopyEntry> > m_postChildren;

				void Clear()
				{
					treeNode.reset();
					m_preChildren.clear();
					m_postChildren.clear();
				}
			};

			// Use object pool
			moodycamel::ConcurrentQueue<std::shared_ptr<UITreeCopyEntry> > m_treeCopyEntryPool;

			std::shared_ptr<UITreeCopyEntry> _GetTreeCopyEntryObject();
			void _ReleaseTreeCopyEntryObject(std::shared_ptr<UITreeCopyEntry> releasingObject);

			std::shared_ptr<UITreeCopyEntry> _CopyUITree(std::shared_ptr<UIObject> treeRoot, std::unordered_set<UIObjectIdType> *inTreeObjects);
			void _ReleaseUITreeCopy(std::shared_ptr<UITreeCopyEntry> releasingTree);

		protected:
			bool _DoDownTouchProcess(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject);

			bool _ExtractTouchState(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, bool isCovered, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &outTouchState);
			void _DoTouchMoveProcess(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &processedObjects);
			void _DoTouchUpdateProcess(std::shared_ptr<UITreeCopyEntry> treeRoot, std::shared_ptr<UITouch> &touchObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &processedObjects);
		};
	}
}

#endif
