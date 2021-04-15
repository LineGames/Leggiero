////////////////////////////////////////////////////////////////////////////////
// Component/UILayoutComponent.h (Leggiero/Modules - LegacyUI)
//
// Layouting Component for an UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__UI_LAYOUT_COMPONENT_H
#define __LM_LUI__COMPONENT__UI_LAYOUT_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <unordered_map>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UITouch;
		class IUIArea;
		class UIObject;
		class IUISizeSubComponent;
		class IUITransform;
		class IUIClipping;


		// Data structure to store calculated UI layout
		struct CalculatedUILayoutInformation
		{
		public:
			UIObjectIdType objectId;
			std::weak_ptr<UIObject> objectReference;

			bool isSizeCalculated;
			UICoordinateType width;
			UICoordinateType height;

			UICoordinateType marginedWidth;
			UICoordinateType marginedHeight;

			bool isOffsetCalculated;
			UICoordinateType parentalOffsetX;
			UICoordinateType parentalOffsetY;

			std::shared_ptr<IUITransform> applyingTransform;
			std::shared_ptr<const IUIClipping> applyingClipping;

		public:
			CalculatedUILayoutInformation()
			{
				ResetInformation();
			}

			void ResetInformation();
		};


		// UI Layout Component
		class UILayoutComponent
			: public IUIComponent
		{
		public:
			UILayoutComponent(std::shared_ptr<IUISizeSubComponent> sizeSubComponent);
			virtual ~UILayoutComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kLayout; }
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			virtual void SetParentOfOwnerObject(const UIObject &parentObject) override;
			virtual void SetNoParentOfOwnerObject() override;

		public:
			virtual void UpdateLayout(UIObject &targetObject, UIObject &parentObject, std::unordered_map<UIObjectIdType, std::shared_ptr<CalculatedUILayoutInformation> > &layoutTable);

			std::shared_ptr<IUISizeSubComponent> GetSizeSubComponent() { return m_sizeSubComponent; }

		public:	// Floating Temp Impl.
			UICoordinateRatioType parentReferenceRatioX;
			UICoordinateRatioType parentReferenceRatioY;
			UICoordinateType parentOffsetX;
			UICoordinateType parentOffsetY;

			UICoordinateRatioType anchorReferenceRatioX;
			UICoordinateRatioType anchorReferenceRatioY;
			UICoordinateType anchorOffsetX;
			UICoordinateType anchorOffsetY;

		public:
			std::shared_ptr<IUISizeSubComponent> m_sizeSubComponent;
		};
	}
}

#endif
