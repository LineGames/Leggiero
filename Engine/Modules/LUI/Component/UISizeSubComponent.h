////////////////////////////////////////////////////////////////////////////////
// Component/UISizeSubComponent.h (Leggiero/Modules - LegacyUI)
//
// Sub-Component to process size information in layouting
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__UI_SIZE_SUB_COMPONENT_H
#define __LM_LUI__COMPONENT__UI_SIZE_SUB_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIObject;
		class UILayoutComponent;


		// Base Interface for UI Size Sub-Component
		class IUISizeSubComponent
		{
		public:
			IUISizeSubComponent() { }
			virtual ~IUISizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() = 0;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const = 0;

			virtual void SetParentOfOwnerObject(const UIObject &parentObject) { }
			virtual void SetNoParentOfOwnerObject() { }

			virtual bool HasParentDependency() const { return false; }
			virtual bool HasChildrenDependency() const { return false; }

			virtual bool IsParentDependencySolved() const { return false; }
		};


		// Targeting specific owner object
		class IOwnerTargetingSizeSubComponent
		{
		public:
			IOwnerTargetingSizeSubComponent(const std::shared_ptr<UIObject> &targetObject) : m_targetObject(targetObject) { }
			virtual ~IOwnerTargetingSizeSubComponent() { }

		public:
			void ChangeTargetObject(std::shared_ptr<UIObject> targetObject) { m_targetObject = targetObject; }

		protected:
			std::weak_ptr<UIObject> m_targetObject;
		};


		// Value-setted size
		class UIValuedSizeSubComponent
			: public IUISizeSubComponent
		{
		public:
			UIValuedSizeSubComponent(UICoordinateType width, UICoordinateType height);
			virtual ~UIValuedSizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() override { return UIElementSize(m_width, m_height); }
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;

		public:
			UICoordinateType GetWidth() const { return m_width; }
			UICoordinateType GetHeight() const { return m_height; }
			void SetWidth(UICoordinateType width) { m_width = width; }
			void SetHeight(UICoordinateType height) { m_height = height; }

		protected:
			UICoordinateType m_width;
			UICoordinateType m_height;
		};


		// Size sub-component for an UI Object
		class UIObjectSizeSubComponent
			: public IUISizeSubComponent
			, public IOwnerTargetingSizeSubComponent
		{
		public:
			UIObjectSizeSubComponent(const std::shared_ptr<UIObject> &targetObject)
				: IOwnerTargetingSizeSubComponent(targetObject)
			{ }
			virtual ~UIObjectSizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() override;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;
		};


		// Size sub-component for renderable
		class UIVisualSizeSubComponent
			: public IUISizeSubComponent
			, public IOwnerTargetingSizeSubComponent
		{
		public:
			UIVisualSizeSubComponent(const std::shared_ptr<UIObject> &targetObject);
			virtual ~UIVisualSizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() override;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;
		};


		// Size of parent
		class UIParentSizeSubComponent
			: public IUISizeSubComponent
		{
		public:
			UIParentSizeSubComponent(std::shared_ptr<UILayoutComponent> parentLayoutComponent = nullptr);
			virtual ~UIParentSizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() override;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;

			virtual void SetParentOfOwnerObject(const UIObject &parentObject) override;
			virtual void SetNoParentOfOwnerObject() override;

			virtual bool HasParentDependency() const override { return true; }
			virtual bool IsParentDependencySolved() const override;

		protected:
			std::weak_ptr<UILayoutComponent> m_parentLayoutComponent;
		};


		// Summation size of stacked children
		class UIChildrenStackSizeSubComponent
			: public IUISizeSubComponent
			, public IOwnerTargetingSizeSubComponent
		{
		public:
			UIChildrenStackSizeSubComponent(const std::shared_ptr<UIObject> &targetObject, bool isHorizontal);
			virtual ~UIChildrenStackSizeSubComponent() { }

		public:
			virtual UIElementSize CalculateSize() override;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;

			virtual bool HasChildrenDependency() const override { return true; }

		protected:
			bool m_isHorizontal;
		};


		// Size modification decorator
		class ModifedSizeSubComponent
			: public IUISizeSubComponent
		{
		public:
			ModifedSizeSubComponent(std::shared_ptr<IUISizeSubComponent> original,
				UICoordinateType widthAdd = static_cast<UICoordinateType>(0.0f), UICoordinateType heightAdd = static_cast<UICoordinateType>(0.0f),
				UICoordinateRatioType widthMultiplier = static_cast<UICoordinateRatioType>(1.0f), UICoordinateRatioType heightMultiplier = static_cast<UICoordinateRatioType>(1.0f));
			virtual ~ModifedSizeSubComponent() { }

		public:	// IUISizeSubComponent
			virtual UIElementSize CalculateSize() override;
			virtual std::shared_ptr<IUISizeSubComponent> Clone() const override;

			virtual void SetParentOfOwnerObject(const UIObject &parentObject) override { m_original->SetParentOfOwnerObject(parentObject); }
			virtual void SetNoParentOfOwnerObject() override { m_original->SetNoParentOfOwnerObject(); }

			virtual bool HasParentDependency() const override { return m_original->HasParentDependency(); }
			virtual bool HasChildrenDependency() const override { return m_original->HasChildrenDependency(); }

		protected:
			std::shared_ptr<IUISizeSubComponent> m_original;

			UICoordinateType m_widthAdd;
			UICoordinateType m_heightAdd;

			UICoordinateRatioType m_widthMultiplier;
			UICoordinateRatioType m_heightMultiplier;
		};
	}
}

#endif
