////////////////////////////////////////////////////////////////////////////////
// Prefab/UIFabricatedComponent.h (Leggiero/Modules - LegacyUI)
//
// Component that represent a object fabricated from a prefab
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_FABRICATED_COMPONENT_H
#define __LM_LUI__PREFAB__UI_FABRICATED_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>
#include <unordered_map>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Component/IUIComponent.h"
#include "../Description/UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class UIObject;


		// UI Object component for an object fabricated from a prefab
		class UIFabricatedComponent
			: public IUIComponent
		{
		public:
			using ObjectDescriptionIdType = Description::VariableNameType;

		public:
			UIFabricatedComponent() { }
			UIFabricatedComponent(const std::unordered_map<ObjectDescriptionIdType, std::weak_ptr<UIObject> > &subObjectTable) : m_idObjectTable(subObjectTable) { }
			UIFabricatedComponent(std::unordered_map<ObjectDescriptionIdType, std::weak_ptr<UIObject> > &&subObjectTable) : m_idObjectTable(subObjectTable) { }
			virtual ~UIFabricatedComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kFabricated; }
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

		public:
			std::shared_ptr<UIObject> FindObject(const ObjectDescriptionIdType &findingId);
			std::shared_ptr<UIObject> FindObjectByPath(const std::string &findingPathString);

		protected:
			std::shared_ptr<UIObject> _FindObjectByPath(const Description::VariablePathType &findingPath, size_t offset);

		protected:
			std::unordered_map<ObjectDescriptionIdType, std::weak_ptr<UIObject> > m_idObjectTable;
		};
	}
}

#endif
