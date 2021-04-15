////////////////////////////////////////////////////////////////////////////////
// Component/IUIComponent.h (Leggiero/Modules - LegacyUI)
//
// Base Interface for UI Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__I_UI_COMPONENT_H
#define __LM_LUI__COMPONENT__I_UI_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "UIComponentType.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class UIObject;


		// UI Component Interface
		class IUIComponent
		{
		public:
			IUIComponent() { }
			virtual ~IUIComponent() { }

		private:
			IUIComponent(const IUIComponent &rhs) = delete;
			IUIComponent &operator=(const IUIComponent &rhs) = delete;

		public:
			virtual UIComponentType GetComponentType() const = 0;

			virtual const std::vector<UIComponentType> &GetDependentComponentTypes() const { static std::vector<UIComponentType> nullTypeList; return nullTypeList; }
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const = 0;

			virtual void SetParentOfOwnerObject(const UIObject &parentObject) { }
			virtual void SetNoParentOfOwnerObject() { }

			virtual void UpdateFrame(GameTimeClockType::duration frameInterval) { }

			virtual void HintScreenDimensionChanged() { }
		};
	}
}

#endif
