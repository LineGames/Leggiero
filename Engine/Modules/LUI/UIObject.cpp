////////////////////////////////////////////////////////////////////////////////
// UIObject.cpp (Leggiero/Modules - LegacyUI)
//
// UI Object Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIObject.h"

// Standard Library
#include <unordered_set>
#include <utility>

// Leggiero.LegacyUI
#include "UIManager.h"
#include "Component/UILayoutComponent.h"
#include "Component/UISizeSubComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIObject

		//------------------------------------------------------------------------------
		UIObject::UIJobValidityCheckerType UIObject::kAlwayTrueValidityCheker = [](std::shared_ptr<UIObject> _) { return true; };

		//------------------------------------------------------------------------------
		UIObject::UIObject(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<UILayoutComponent> layoutComponent, bool isLazyLayout)
			: m_ownerManager(ownerManager), m_id(ownerManager->IssueObjectId()), m_structuralVersion(static_cast<StructuralVersionType>(1)), m_parentReference(nullptr)
			, m_isVisible(true), m_isUpdating(true), m_isEnable(true), m_isParentEnable(true), m_isVirtualLogicObject(false), m_alpha(1.0f)
			, m_layoutComponent(layoutComponent)
		{
			if (!m_layoutComponent && !isLazyLayout)
			{
				m_layoutComponent = std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(0.0f, 0.0f));
			}
		}

		//------------------------------------------------------------------------------
		UIObject::~UIObject()
		{
			for (std::shared_ptr<UIObject> &currentObject : m_preChildren)
			{
				currentObject->UpdateNoParent();
			}
			for (std::shared_ptr<UIObject> &currentObject : m_postChildren)
			{
				currentObject->UpdateNoParent();
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIObject> UIObject::Clone() const
		{
			std::shared_ptr<UIObject> clonedObject(_AllocateCloningObject());

			// Clone Own Data
			clonedObject->m_isVisible = m_isVisible;
			clonedObject->m_isUpdating = m_isUpdating;
			clonedObject->m_isEnable = m_isEnable;
			clonedObject->m_isParentEnable = m_isParentEnable;

			if (m_layoutComponent)
			{
				clonedObject->m_layoutComponent = std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject));
			}

			std::unordered_set<UIComponentType, Utility::SyntacticSugar::EnumClassHash> copiedComponentSet;
			while (copiedComponentSet.size() < m_components.size())
			{
				bool isCopiedLoop = false;
				for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::const_iterator it = m_components.begin(); it != m_components.end(); ++it)
				{
					if (copiedComponentSet.find(it->first) != copiedComponentSet.end())
					{
						continue;
					}

					const std::vector<UIComponentType> &dependencyList = it->second->GetDependentComponentTypes();
					bool isUnResolvedDependencyExists = false;
					for (const UIComponentType &dependingComponentType : dependencyList)
					{
						if (copiedComponentSet.find(dependingComponentType) == copiedComponentSet.end())
						{
							isUnResolvedDependencyExists = true;
							break;
						}
					}
					if (isUnResolvedDependencyExists)
					{
						continue;
					}

					isCopiedLoop = true;
					copiedComponentSet.insert(it->first);
					std::shared_ptr<IUIComponent> clonedComponent(it->second->Clone(*clonedObject));
					if (clonedComponent)
					{
						clonedObject->m_components[it->first] = clonedComponent;
					}
				}
				if (!isCopiedLoop)
				{
					// Cyclic Component Dependency Exists
					break;
				}
			}

			clonedObject->m_preChildren.reserve(m_preChildren.size());
			for (const std::shared_ptr<UIObject> &currentChild : m_preChildren)
			{
				std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
				if (clonedChild)
				{
					clonedChild->UpdateParent(*clonedObject);
					clonedObject->m_preChildren.push_back((clonedChild));
				}
			}
			clonedObject->m_postChildren.reserve(m_postChildren.size());
			for (const std::shared_ptr<UIObject> &currentChild : m_postChildren)
			{
				std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
				if (clonedChild)
				{
					clonedChild->UpdateParent(*clonedObject);
					clonedObject->m_postChildren.push_back((clonedChild));
				}
			}

			return clonedObject;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIObject> UIObject::_AllocateCloningObject() const
		{
			std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
			if (!managerCopy)
			{
				return nullptr;
			}
			return std::make_shared<UIObject>(managerCopy);
		}

		//------------------------------------------------------------------------------
		std::vector<std::shared_ptr<UIObject> > UIObject::GetPreChildren() const
		{
			return std::vector<std::shared_ptr<UIObject> >(m_preChildren);
		}

		//------------------------------------------------------------------------------
		std::vector<std::shared_ptr<UIObject> > UIObject::GetPostChildren() const
		{
			return std::vector<std::shared_ptr<UIObject> >(m_postChildren);
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPreUIObject(std::shared_ptr<UIObject> object)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			m_preChildren.push_back(object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPreUIObjectBeforeOf(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			if (!beforeOf)
			{
				m_preChildren.push_back(object);
				object->UpdateParent(*this);
				_IncreaseStructuralVersion();
				return;
			}
			UIObjectIdType findId = beforeOf->GetId();
			std::vector<std::shared_ptr<UIObject> >::iterator findIter = m_preChildren.begin();
			while (findIter != m_preChildren.end())
			{
				if ((*findIter)->GetId() == findId)
				{
					break;
				}
				++findIter;
			}
			m_preChildren.insert(findIter, object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPreUIObjectAtIndex(std::shared_ptr<UIObject> object, size_t index)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			if (index > m_preChildren.size())
			{
				index = m_preChildren.size();
			}
			std::vector<std::shared_ptr<UIObject> >::iterator insertIter = m_preChildren.begin() + index;
			m_preChildren.insert(insertIter, object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPostUIObject(std::shared_ptr<UIObject> object)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			m_postChildren.push_back(object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPostUIObjectBeforeOf(std::shared_ptr<UIObject> object, std::shared_ptr<UIObject> beforeOf)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			if (!beforeOf)
			{
				m_postChildren.push_back(object);
				object->UpdateParent(*this);
				_IncreaseStructuralVersion();
				return;
			}
			UIObjectIdType findId = beforeOf->GetId();
			std::vector<std::shared_ptr<UIObject> >::iterator findIter = m_postChildren.begin();
			while (findIter != m_postChildren.end())
			{
				if ((*findIter)->GetId() == findId)
				{
					break;
				}
				++findIter;
			}
			m_postChildren.insert(findIter, object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::AddPostUIObjectAtIndex(std::shared_ptr<UIObject> object, size_t index)
		{
			if (!object)
			{
				return;
			}
			RemoveUIObject(object, false);

			if (index > m_postChildren.size())
			{
				index = m_postChildren.size();
			}
			std::vector<std::shared_ptr<UIObject> >::iterator insertIter = m_postChildren.begin() + index;
			m_postChildren.insert(insertIter, object);
			object->UpdateParent(*this);
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		// if isFindInDescendant is true, then do removing recursively
		bool UIObject::RemoveUIObject(std::shared_ptr<UIObject> object, bool isFindInDescendant)
		{
			if (!object)
			{
				return false;
			}

			UIObjectIdType findId = object->GetId();

			std::vector<std::shared_ptr<UIObject> >::iterator removeIter = m_preChildren.begin();
			while (removeIter != m_preChildren.end())
			{
				if ((*removeIter)->GetId() == findId)
				{
					break;
				}

				if (isFindInDescendant)
				{
					bool isChildRemoveSuccess = (*removeIter)->RemoveUIObject(object, true);
					if (isChildRemoveSuccess)
					{
						return true;
					}
				}

				++removeIter;
			}
			if (removeIter != m_preChildren.end())
			{
				(*removeIter)->UpdateNoParent();
				m_preChildren.erase(removeIter);
				_IncreaseStructuralVersion();
				return true;
			}

			removeIter = m_postChildren.begin();
			while (removeIter != m_postChildren.end())
			{
				if ((*removeIter)->GetId() == findId)
				{
					break;
				}

				if (isFindInDescendant)
				{
					bool isChildRemoveSuccess = (*removeIter)->RemoveUIObject(object, true);
					if (isChildRemoveSuccess)
					{
						return true;
					}
				}

				++removeIter;
			}
			if (removeIter != m_postChildren.end())
			{
				(*removeIter)->UpdateNoParent();
				m_postChildren.erase(removeIter);
				_IncreaseStructuralVersion();
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------------
		void UIObject::ClearPreChildren()
		{
			for (std::shared_ptr<UIObject> &currentChild : m_preChildren)
			{
				currentChild->UpdateNoParent();
			}
			m_preChildren.clear();
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		void UIObject::ClearPostChildren()
		{
			for (std::shared_ptr<UIObject> &currentChild : m_postChildren)
			{
				currentChild->UpdateNoParent();
			}
			m_postChildren.clear();
			_IncreaseStructuralVersion();
		}

		//------------------------------------------------------------------------------
		// Iterate UI Tree and do a given job
		bool UIObject::IterateUIJob(const UIObject::UIJobType &jobToDo, const UIObject::UIJobValidityCheckerType &validityChecker)
		{
			for (std::shared_ptr<UIObject> &currentObject : m_preChildren)
			{
				if (validityChecker(currentObject))
				{
					if (currentObject->IterateUIJob(jobToDo, validityChecker))
					{
						return true;
					}
				}
			}

			if (jobToDo(shared_from_this()))
			{
				return true;
			}

			for (std::shared_ptr<UIObject> &currentObject : m_postChildren)
			{
				if (validityChecker(currentObject))
				{
					if (currentObject->IterateUIJob(jobToDo, validityChecker))
					{
						return true;
					}
				}
			}

			return false;
		}

		//------------------------------------------------------------------------------
		bool UIObject::IsAncestorOf(std::shared_ptr<const UIObject> targetObject) const
		{
			for (const std::shared_ptr<UIObject> &currentObject : m_preChildren)
			{
				if (currentObject == targetObject)
				{
					return true;
				}
				if (currentObject->IsAncestorOf(targetObject))
				{
					return true;
				}
			}

			for (const std::shared_ptr<UIObject> &currentObject : m_postChildren)
			{
				if (currentObject == targetObject)
				{
					return true;
				}
				if (currentObject->IsAncestorOf(targetObject))
				{
					return true;
				}
			}

			return false;
		}

		//------------------------------------------------------------------------------
		bool UIObject::IsParentOf(std::shared_ptr<const UIObject> targetObject) const
		{
			for (const std::shared_ptr<UIObject> &currentObject : m_preChildren)
			{
				if (currentObject == targetObject)
				{
					return true;
				}
			}

			for (const std::shared_ptr<UIObject> &currentObject : m_postChildren)
			{
				if (currentObject == targetObject)
				{
					return true;
				}
			}

			return false;
		}

		//------------------------------------------------------------------------------
		bool UIObject::IsChildOf(std::shared_ptr<const UIObject> targetObject) const
		{
			return targetObject->IsParentOf(shared_from_this());
		}

		//------------------------------------------------------------------------------
		bool UIObject::IsDescendantOf(std::shared_ptr<const UIObject> targetObject) const
		{
			return targetObject->IsAncestorOf(shared_from_this());
		}

		//------------------------------------------------------------------------------
		// Update parent change
		void UIObject::UpdateParent(const UIObject &parent)
		{
			m_parentReference = const_cast<UIObject *>(&parent);
			if (m_layoutComponent)
			{
				m_layoutComponent->SetParentOfOwnerObject(parent);
			}
			for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator it = m_components.begin(); it != m_components.end(); ++it)
			{
				it->second->SetParentOfOwnerObject(parent);
			}
			bool parentEnableValue = parent.IsEnable();
			if (m_isParentEnable != parentEnableValue)
			{
				_SetIsParentEnable(parentEnableValue);
			}
		}

		//------------------------------------------------------------------------------
		// Update to no parent
		void UIObject::UpdateNoParent()
		{
			m_parentReference = nullptr;
			if (m_layoutComponent)
			{
				m_layoutComponent->SetNoParentOfOwnerObject();
			}
			for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator it = m_components.begin(); it != m_components.end(); ++it)
			{
				it->second->SetNoParentOfOwnerObject();
			}
		}

		//------------------------------------------------------------------------------
		void UIObject::UpdateFrame(GameTimeClockType::duration frameInterval)
		{
			if (m_layoutComponent)
			{
				m_layoutComponent->UpdateFrame(frameInterval);
			}
			for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator it = m_components.begin(); it != m_components.end(); ++it)
			{
				it->second->UpdateFrame(frameInterval);
			}
			_UpdateFrameSelf(frameInterval);
		}

		//------------------------------------------------------------------------------
		void UIObject::HintScreenDimensionChanged()
		{
			if (m_layoutComponent)
			{
				m_layoutComponent->HintScreenDimensionChanged();
			}
			for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator it = m_components.begin(); it != m_components.end(); ++it)
			{
				it->second->HintScreenDimensionChanged();
			}
			_HintScreenDimensionChangedSelf();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UIObject::GetComponent(UIComponentType componentType) const
		{
			std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::const_iterator findIt = m_components.find(componentType);
			if (findIt == m_components.cend())
			{
				if (componentType == UIComponentType::kLayout && m_layoutComponent)
				{
					return m_layoutComponent;
				}

				return nullptr;
			}

			return findIt->second;
		}

		//------------------------------------------------------------------------------
		// Register the given component. Fails if a component of same type existing.
		// This function is not thread safe.
		bool UIObject::RegisterComponent(std::shared_ptr<IUIComponent> component)
		{
			if (!component)
			{
				return false;
			}

			UIComponentType registerType = component->GetComponentType();
			if (registerType == UIComponentType::kLayout)
			{
				ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(component));
				return true;
			}

			std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator findIt = m_components.find(registerType);
			if (findIt != m_components.end())
			{
				return false;
			}

			m_components[registerType] = component;
			return true;
		}

		//------------------------------------------------------------------------------
		// Remove a component of given type, and return it.
		// This function is not thread safe.
		std::shared_ptr<IUIComponent> UIObject::UnRegisterComponent(UIComponentType componentType)
		{
			std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::iterator findIt = m_components.find(componentType);
			if (findIt == m_components.end())
			{
				std::shared_ptr<UILayoutComponent> layoutComponentCopy(m_layoutComponent);
				if (componentType == UIComponentType::kLayout && layoutComponentCopy)
				{
					ChangeLayoutComponent(nullptr);
					return layoutComponentCopy;
				}
				return nullptr;
			}

			std::shared_ptr<IUIComponent> removedComponent(findIt->second);
			m_components.erase(findIt);
			return removedComponent;
		}

		//------------------------------------------------------------------------------
		// Set whether to active the UI object
		void UIObject::SetIsEnable(bool isEnable)
		{
			bool lastEnableValue = IsEnable();
			m_isEnable = isEnable;

			bool afterEnableValue = IsEnable();
			if (lastEnableValue != afterEnableValue)
			{
				_IncreaseStructuralVersion();

				OnEnableChanged(afterEnableValue);
				for (std::shared_ptr<UIObject> &currentObject : m_preChildren)
				{
					currentObject->_SetIsParentEnable(afterEnableValue);
				}
				for (std::shared_ptr<UIObject> &currentObject : m_postChildren)
				{
					currentObject->_SetIsParentEnable(afterEnableValue);
				}
			}
		}

		//------------------------------------------------------------------------------
		// Notify parent's activation changed
		void UIObject::_SetIsParentEnable(bool isEnable)
		{
			bool lastEnableValue = IsEnable();
			m_isParentEnable = isEnable;

			bool afterEnableValue = IsEnable();
			if (lastEnableValue != afterEnableValue)
			{
				OnEnableChanged(afterEnableValue);
				for (std::shared_ptr<UIObject> &currentObject : m_preChildren)
				{
					currentObject->_SetIsParentEnable(afterEnableValue);
				}
				for (std::shared_ptr<UIObject> &currentObject : m_postChildren)
				{
					currentObject->_SetIsParentEnable(afterEnableValue);
				}
			}
		}

		//------------------------------------------------------------------------------
		// Update its layout
		void UIObject::UpdateLayout(std::shared_ptr<UIObject> updatingParent, std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> > &layoutTable)
		{
			if (!IsVisible())
			{
				return;
			}

			// Ignore Root
			if (updatingParent)
			{
				m_layoutComponent->UpdateLayout(*this, *updatingParent, layoutTable);
			}

			// Only Floating Objects
			for (std::shared_ptr<UIObject> &currentObject : m_preChildren)
			{
				currentObject->UpdateLayout(shared_from_this(), layoutTable);
			}
			for (std::shared_ptr<UIObject> &currentObject : m_postChildren)
			{
				currentObject->UpdateLayout(shared_from_this(), layoutTable);
			}
		}

		//------------------------------------------------------------------------------
		// Return object size in layout
		UIElementSize UIObject::CalculateSize()
		{
			if (!m_layoutComponent)
			{
				return UIElementSize();
			}

			std::shared_ptr<IUISizeSubComponent> sizeSubComponent(m_layoutComponent->GetSizeSubComponent());
			if (!sizeSubComponent)
			{
				return UIElementSize();
			}
			return sizeSubComponent->CalculateSize();
		}

		//------------------------------------------------------------------------------
		// Return object with in layout
		UICoordinateType UIObject::CalculateWidth()
		{
			if (!m_layoutComponent)
			{
				return static_cast<UICoordinateType>(0.0f);
			}

			std::shared_ptr<IUISizeSubComponent> sizeSubComponent(m_layoutComponent->GetSizeSubComponent());
			if (!sizeSubComponent)
			{
				return static_cast<UICoordinateType>(0.0f);
			}
			return sizeSubComponent->CalculateSize().width;
		}

		//------------------------------------------------------------------------------
		// Return object height in layout
		UICoordinateType UIObject::CalculateHeight()
		{
			if (!m_layoutComponent)
			{
				return static_cast<UICoordinateType>(0.0f);
			}

			std::shared_ptr<IUISizeSubComponent> sizeSubComponent(m_layoutComponent->GetSizeSubComponent());
			if (!sizeSubComponent)
			{
				return static_cast<UICoordinateType>(0.0f);
			}
			return sizeSubComponent->CalculateSize().height;
		}

		//------------------------------------------------------------------------------
		// Update Structural Version
		void UIObject::_IncreaseStructuralVersion()
		{
			++m_structuralVersion;
			if (m_parentReference != nullptr)
			{
				m_parentReference->_IncreaseStructuralVersion();
			}
		}
	}
}
