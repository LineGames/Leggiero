////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionProcessingContext.h (Leggiero/Modules - LegacyUI)
//
// Context of UI Description Processing
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_PROCESSING_CONTEXT_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_PROCESSING_CONTEXT_H


// Standard Library
#include <memory>
#include <tuple>
#include <vector>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Forward Declarations
			class DescriptionManager;
			class DescriptionUnit;
			class IDescriptionObject;
			class IDescriptionNameSpaceContext;


			// Forward Declaration for Dependent Type Definition
			class DescriptionProcessingContext;


			// Environment and Object Pair
			using ObjectEnvironmentPair = std::tuple<std::shared_ptr<IDescriptionObject>, DescriptionProcessingContext>;


			// Context for processing description
			class DescriptionProcessingContext
			{
			public:
				DescriptionManager &manager;
				std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > nameSpaceStack;

			public:
				DescriptionProcessingContext(DescriptionManager &manager, std::shared_ptr<IDescriptionNameSpaceContext> baseNameSpace = nullptr);
				DescriptionProcessingContext(const DescriptionProcessingContext &original);
				DescriptionProcessingContext(DescriptionProcessingContext &&rhs);

			public:
				DescriptionProcessingContext GetStacked(std::shared_ptr<IDescriptionNameSpaceContext> stackingNameSpace) const;

			public:
				ObjectEnvironmentPair Find(const VariableNameType &name);
				ObjectEnvironmentPair Find(const VariablePathType &path);
			};
		}
	}
}

#endif
