////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionUnit.h (Leggiero/Modules - LegacyUI)
//
// Definition of an UI Description Unit which is a unit of UI Description
// Processing
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_UNIT_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_UNIT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Forward Declaration
			class DescriptionManager;


			// An UI Description Unit
			// Also is a namespace. When load UI Description data from a file, it considered as an Description Unit.
			class DescriptionUnit
				: public IDescriptionNameSpaceContext
				, public std::enable_shared_from_this<DescriptionUnit>
			{
			public:
				DescriptionUnit() { }
				virtual ~DescriptionUnit() { }

			public:
				static std::shared_ptr<DescriptionUnit> LoadFromXML(std::string xmlData, DescriptionManager &loadingManager);

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;
				virtual bool IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const override { return true; }

			public:
				ObjectEnvironmentPair SelectDescriptionByName(DescriptionManager &manager, const VariableNameType &name);

				ObjectEnvironmentPair SelectDescriptionByPath(DescriptionManager &manager, const std::string &path);
				ObjectEnvironmentPair SelectDescriptionByPath(DescriptionManager &manager, const VariablePathType &path);

			public:
				void AddObject(const VariableNameType &name, std::shared_ptr<IDescriptionObject> descriptionObject);

			protected:
				std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > m_descriptionTable;
			};
		}
	}
}

#endif
