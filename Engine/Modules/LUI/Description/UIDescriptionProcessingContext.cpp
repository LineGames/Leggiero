////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionProcessingContext.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description Processing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionProcessingContext.h"

// Leggiero.LegacyUI
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// DescriptionProcessingContext

			//------------------------------------------------------------------------------
			DescriptionProcessingContext::DescriptionProcessingContext(DescriptionManager &manager, std::shared_ptr<IDescriptionNameSpaceContext> baseNameSpace)
				: manager(manager)
			{
				if (baseNameSpace)
				{
					nameSpaceStack.push_back(baseNameSpace);
				}
			}

			//------------------------------------------------------------------------------
			DescriptionProcessingContext::DescriptionProcessingContext(const DescriptionProcessingContext &original)
				: manager(original.manager), nameSpaceStack(original.nameSpaceStack)
			{
			}

			//------------------------------------------------------------------------------
			DescriptionProcessingContext::DescriptionProcessingContext(DescriptionProcessingContext &&rhs)
				: manager(rhs.manager), nameSpaceStack(std::move(rhs.nameSpaceStack))
			{
			}

			//------------------------------------------------------------------------------
			DescriptionProcessingContext DescriptionProcessingContext::GetStacked(std::shared_ptr<IDescriptionNameSpaceContext> stackingNameSpace) const
			{
				DescriptionProcessingContext stackedContext(*this);
				if (stackingNameSpace)
				{
					stackedContext.nameSpaceStack.push_back(stackingNameSpace);
				}
				return stackedContext;
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionProcessingContext::Find(const VariableNameType &name)
			{
				DescriptionProcessingContext findingContext(*this);

				size_t stackDepth = nameSpaceStack.size();
				for (size_t i = 0; i < stackDepth; ++i)
				{
					ObjectEnvironmentPair searchResult(nameSpaceStack[stackDepth - i - 1]->Select(name, findingContext));
					if (std::get<0>(searchResult))
					{
						return searchResult;
					}
					findingContext.nameSpaceStack.pop_back();
				}
				return ObjectEnvironmentPair(nullptr, *this);
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionProcessingContext::Find(const VariablePathType &path)
			{
				DescriptionProcessingContext findingContext(*this);

				size_t stackDepth = nameSpaceStack.size();
				for (size_t i = 0; i < stackDepth; ++i)
				{
					ObjectEnvironmentPair searchResult(nameSpaceStack[stackDepth - i - 1]->Select(path, findingContext));
					if (std::get<0>(searchResult))
					{
						return searchResult;
					}
					findingContext.nameSpaceStack.pop_back();
				}
				return ObjectEnvironmentPair(nullptr, *this);
			}
		}
	}
}
