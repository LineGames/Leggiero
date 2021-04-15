////////////////////////////////////////////////////////////////////////////////
// UIObject.h (Leggiero/Modules - LegacyUI)
//
// UI Object Definition
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__UI_OBJECT_H
#define __LM_LUI__UI_OBJECT_H


// Standard Library
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

// Leggiero.LegacyUI
#include "Common/UICommonTypes.h"
#include "Component/UIComponentType.h"
#include "Component/IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIRenderer;
		class UITouchNegotiator;
		class UILayoutComponent;
		class IUIClipping;
		struct CalculatedUILayoutInformation;


		// UI Object
		class UIObject
			: public std::enable_shared_from_this<UIObject>
		{
			friend class UIManager;
			friend class UIRenderer;
			friend class UITouchNegotiator;
			friend class UILayoutComponent;

		public:
			using UIJobType = std::function<bool(std::shared_ptr<UIObject>)>;
			using UIJobValidityCheckerType = std::function<bool(std::shared_ptr<UIObject>)>;

			static UIJobValidityCheckerType kAlwayTrueValidityCheker;

		public:
			UIObject(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<UILayoutComponent> layoutComponent = nullptr, bool isLazyLayout = false);
			virtual ~UIObject();

			virtual std::shared_ptr<UIObject> Clone() const;

		protected:
			virtual std::shared_ptr<UIObject> _AllocateCloningObject() const;

		private:
			UIObject(const UIObject &rhs) = delete;
			UIObject &operator=(const UIObject &rhs) = delete;

			UIObject(UIObject &&rhs) = delete;
			UIObject &operator=(UIObject &&rhs) = delete;

			UIObject() = delete;

		public:
			UIObjectIdType GetId() const { return m_id; }

		public:	// UI Tree
			size_t GetPreChildrenCount() const { return m_preChildren.size(); }
			std::vector<std::shared_ptr<UIObject> > GetPreChildren() const;

			size_t GetPostChildrenCount() const { return m_postChildren.size(); }
			std::vector<std::shared_ptr<UIObject> > GetPostChildren() const;

			void AddPreUIObject(std::shared_ptr<UIObject> object);
			void AddPreUIObjectBeforeOf(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf = nullptr);
			void AddPreUIObjectAtIndex(std::shared_ptr<UIObject> object, size_t index = 0);

			void AddPostUIObject(std::shared_ptr<UIObject> object);
			void AddPostUIObjectBeforeOf(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf = nullptr);
			void AddPostUIObjectAtIndex(std::shared_ptr<UIObject> object, size_t index = 0);

			bool RemoveUIObject(std::shared_ptr<UIObject> object, bool isFindInDescendant = false);

			void ClearPreChildren();
			void ClearPostChildren();
			void ClearChildren() { ClearPreChildren(); ClearPostChildren(); }

			bool IterateUIJob(const UIJobType &jobToDo, const UIJobValidityCheckerType &validityChecker = kAlwayTrueValidityCheker);

			bool IsAncestorOf(std::shared_ptr<const UIObject> targetObject) const;
			bool IsParentOf(std::shared_ptr<const UIObject> targetObject) const;
			bool IsChildOf(std::shared_ptr<const UIObject> targetObject) const;
			bool IsDescendantOf(std::shared_ptr<const UIObject> targetObject) const;

		public:	// Common Utility
			virtual void UpdateParent(const UIObject &parent);
			virtual void UpdateNoParent();

			virtual void UpdateFrame(GameTimeClockType::duration frameInterval);

			virtual void HintScreenDimensionChanged();

		public:	// UI Component Holder
			std::shared_ptr<IUIComponent> GetComponent(UIComponentType componentType) const;

			template <class ComponentT>
			std::shared_ptr<ComponentT> GetComponentObject(UIComponentType componentType) const
			{
				return std::dynamic_pointer_cast<ComponentT>(GetComponent(componentType));
			}

			bool RegisterComponent(std::shared_ptr<IUIComponent> component);
			std::shared_ptr<IUIComponent> UnRegisterComponent(UIComponentType componentType);

		public:	// Object State
			bool IsVisible() const { return m_isVisible; }
			void SetIsVisible(bool isVisible)
			{
				if (isVisible == m_isVisible) return;
				m_isVisible = isVisible;
				_IncreaseStructuralVersion();
			}

			bool IsUpdating() const { return m_isUpdating; }
			void SetIsUpdating(bool isUpdating) { m_isUpdating = isUpdating; }

			bool IsEnable() const { return m_isEnable && m_isParentEnable; }
			void SetIsEnable(bool isEnable);

			float GetAlpha() const { return m_alpha; }
			void SetAlpha(float alpha) {
				if (alpha < 0.0f) { m_alpha = 0.0f; }
				else if (alpha > 1.0f) { m_alpha = 1.0f; }
				else { m_alpha = alpha; }
			}

			bool IsVirtualLogicObject() const { return m_isVirtualLogicObject; }
			void SetIsVirtualLogicObject(bool isVirtualLogicObject) { m_isVirtualLogicObject = isVirtualLogicObject; }

			StructuralVersionType GetStructuralVersion() const { return m_structuralVersion; }

		public:	// Layout
			virtual void UpdateLayout(std::shared_ptr<UIObject> updatingParent, std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> > &layoutTable);

			virtual UIElementSize CalculateSize();
			virtual UICoordinateType CalculateWidth();
			virtual UICoordinateType CalculateHeight();

			std::shared_ptr<UILayoutComponent> GetLayoutComponent() const { return m_layoutComponent; }
			void ChangeLayoutComponent(std::shared_ptr<UILayoutComponent> layoutComponent) { m_layoutComponent = layoutComponent; }

			std::shared_ptr<CalculatedUILayoutInformation> GetCachedLayoutInformation() { return m_cachedLayoutInfo.lock(); }

		public:	// Thread-Aware Process
			virtual void GraphicPrepare() { }
			virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) { }

		protected:
			void _SetIsParentEnable(bool isEnable);
			virtual void OnEnableChanged(bool isEnable) { }

			void _IncreaseStructuralVersion();

		protected:
			virtual void _UpdateFrameSelf(GameTimeClockType::duration frameInterval) { }
			virtual void _HintScreenDimensionChangedSelf() { }

		protected:
			std::weak_ptr<UIManager> m_ownerManager;
			UIObjectIdType m_id;

			UIObject *m_parentReference;
			StructuralVersionType	m_structuralVersion;

			std::vector<std::shared_ptr<UIObject> > m_preChildren;
			std::vector<std::shared_ptr<UIObject> > m_postChildren;

			std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash> m_components;

			bool m_isVisible;
			bool m_isUpdating;
			bool m_isEnable;
			bool m_isParentEnable;

			bool m_isVirtualLogicObject;

			float m_alpha;

		protected:
			std::shared_ptr<UILayoutComponent> m_layoutComponent;
			std::weak_ptr<CalculatedUILayoutInformation> m_cachedLayoutInfo;
		};
	}
}

#endif
