////////////////////////////////////////////////////////////////////////////////
// Description/UIDescription.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description Common Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescription.h"

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionUnit.h"
#include "UIDescriptionProcessingContext.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// IDescriptionNameSpaceContext

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair IDescriptionNameSpaceContext::_Select(const VariablePathType &path, size_t offset, DescriptionProcessingContext &processingContext)
			{
				if (path.size() <= offset)
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				ObjectEnvironmentPair subObject = Select(path[offset], processingContext);
				if (!std::get<0>(subObject))
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				size_t nextOffset = offset + 1;
				if (nextOffset == path.size())
				{
					return subObject;
				}
				else
				{
					std::shared_ptr<IDescriptionNameSpaceContext> subObjectNameSpace(std::dynamic_pointer_cast<IDescriptionNameSpaceContext>(std::get<0>(subObject)));
					DescriptionProcessingContext nextContextCopy = subObjectNameSpace->IsNeedRecursiveSearch(std::get<1>(subObject)) ? std::get<1>(subObject).GetStacked(subObjectNameSpace) : std::get<1>(subObject);
					if (subObjectNameSpace)
					{
						return subObjectNameSpace->_Select(path, nextOffset, nextContextCopy);
					}
				}

				return ObjectEnvironmentPair(nullptr, processingContext);
			}


			//////////////////////////////////////////////////////////////////////////////// ImportedUnit

			//------------------------------------------------------------------------------
			ImportedUnit::ImportedUnit(SavedUnitNameType targetUnitName)
				: m_targetUnitName(targetUnitName)
			{
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair ImportedUnit::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				std::shared_ptr<DescriptionUnit> target(processingContext.manager.GetCachedUnit(m_targetUnitName));
				if (!target)
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}
				DescriptionProcessingContext unitContext(processingContext.manager, target);
				return target->Select(name, unitContext);
			}


			//////////////////////////////////////////////////////////////////////////////// NameSpaceGroup

			//------------------------------------------------------------------------------
			NameSpaceGroup::NameSpaceGroup(const std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > &original)
				: m_descriptionTable(original)
			{
			}

			//------------------------------------------------------------------------------
			NameSpaceGroup::NameSpaceGroup(std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> > &&rhs)
				: m_descriptionTable(rhs)
			{
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair NameSpaceGroup::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> >::iterator findIt = m_descriptionTable.find(name);
				if (findIt == m_descriptionTable.end())
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				std::shared_ptr<IDescriptionObject> foundObject(findIt->second);
				if (!foundObject)
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				if (foundObject->IsAlias())
				{
					switch (foundObject->GetType())
					{
						case UIDescriptionObjectType::kAliasName:
							{
								std::shared_ptr<AliasName> concreteObject(std::dynamic_pointer_cast<AliasName>(foundObject));
								return Select(concreteObject->GetAliasedName(), processingContext);
							}
							break;

						case UIDescriptionObjectType::kAliasPath:
							{
								std::shared_ptr<AliasPath> concreteObject(std::dynamic_pointer_cast<AliasPath>(foundObject));
								return _Select(concreteObject->GetAliasedPath(), 0, processingContext);
							}
							break;
					}
				}

				return ObjectEnvironmentPair(foundObject, processingContext);
			}

			//------------------------------------------------------------------------------
			void NameSpaceGroup::Register(const VariableNameType &name, std::shared_ptr<IDescriptionObject> value)
			{
				if (!value)
				{
					UnRegister(name);
					return;
				}
				m_descriptionTable[name] = value;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IDescriptionObject> NameSpaceGroup::UnRegister(const VariableNameType &name)
			{
				std::unordered_map<VariableNameType, std::shared_ptr<IDescriptionObject> >::iterator findIt = m_descriptionTable.find(name);
				if (findIt == m_descriptionTable.end())
				{
					return nullptr;
				}
				std::shared_ptr<IDescriptionObject> removedValue(findIt->second);
				m_descriptionTable.erase(findIt);
				return removedValue;
			}


			//////////////////////////////////////////////////////////////////////////////// OverridingNameSpace

			//------------------------------------------------------------------------------
			OverridingNameSpace::OverridingNameSpace(const std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > &original)
				: m_priorityTable(original)
			{ }

			//------------------------------------------------------------------------------
			OverridingNameSpace::OverridingNameSpace(std::vector<std::shared_ptr<IDescriptionNameSpaceContext> > &&rhs)
				: m_priorityTable(rhs)
			{ }

			//------------------------------------------------------------------------------
			OverridingNameSpace::OverridingNameSpace(std::initializer_list<std::shared_ptr<IDescriptionNameSpaceContext> > il)
				: m_priorityTable(il)
			{ }

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair OverridingNameSpace::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				for (const std::shared_ptr<IDescriptionNameSpaceContext> &currentNameSpace : m_priorityTable)
				{
					if (currentNameSpace)
					{
						ObjectEnvironmentPair searchResult(currentNameSpace->Select(name, processingContext));
						if (std::get<0>(searchResult))
						{
							return searchResult;
						}
					}
				}
				return ObjectEnvironmentPair(nullptr, processingContext);
			}

			//------------------------------------------------------------------------------
			void OverridingNameSpace::Append(std::shared_ptr<IDescriptionNameSpaceContext> appendingNameSpace)
			{
				if (!appendingNameSpace)
				{
					return;
				}
				m_priorityTable.push_back(appendingNameSpace);
			}

			//------------------------------------------------------------------------------
			void OverridingNameSpace::Override(std::shared_ptr<IDescriptionNameSpaceContext> overridingNameSpace)
			{
				if (!overridingNameSpace)
				{
					return;
				}
				m_priorityTable.insert(m_priorityTable.begin(), overridingNameSpace);
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair OverridingNameSpace::NameSpaceAliasByName::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				ObjectEnvironmentPair foundAliasContext(processingContext.Find(m_alias));

				std::shared_ptr<IDescriptionNameSpaceContext> aliasedNameSpace(std::dynamic_pointer_cast<IDescriptionNameSpaceContext>(std::get<0>(foundAliasContext)));
				if (aliasedNameSpace)
				{
					return aliasedNameSpace->Select(name, std::get<1>(foundAliasContext));
				}

				return ObjectEnvironmentPair(nullptr, processingContext);
			}

			//------------------------------------------------------------------------------
			bool OverridingNameSpace::NameSpaceAliasByName::IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const
			{
				ObjectEnvironmentPair foundAliasContext(processingContext.Find(m_alias));

				std::shared_ptr<IDescriptionNameSpaceContext> aliasedNameSpace(std::dynamic_pointer_cast<IDescriptionNameSpaceContext>(std::get<0>(foundAliasContext)));
				if (aliasedNameSpace)
				{
					return aliasedNameSpace->IsNeedRecursiveSearch(std::get<1>(foundAliasContext));
				}

				return false;
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair OverridingNameSpace::NameSpaceAliasByPath::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				ObjectEnvironmentPair foundAliasContext(processingContext.Find(m_alias));

				std::shared_ptr<IDescriptionNameSpaceContext> aliasedNameSpace(std::dynamic_pointer_cast<IDescriptionNameSpaceContext>(std::get<0>(foundAliasContext)));
				if (aliasedNameSpace)
				{
					return aliasedNameSpace->Select(name, std::get<1>(foundAliasContext));
				}

				return ObjectEnvironmentPair(nullptr, processingContext);
			}

			//------------------------------------------------------------------------------
			bool OverridingNameSpace::NameSpaceAliasByPath::IsNeedRecursiveSearch(DescriptionProcessingContext &processingContext) const
			{
				ObjectEnvironmentPair foundAliasContext(processingContext.Find(m_alias));

				std::shared_ptr<IDescriptionNameSpaceContext> aliasedNameSpace(std::dynamic_pointer_cast<IDescriptionNameSpaceContext>(std::get<0>(foundAliasContext)));
				if (aliasedNameSpace)
				{
					return aliasedNameSpace->IsNeedRecursiveSearch(std::get<1>(foundAliasContext));
				}

				return false;
			}


			//////////////////////////////////////////////////////////////////////////////// Variable

			//------------------------------------------------------------------------------
			template<>
			Variable<Vector2DValueType>::Variable()
				: m_isEvaluated(false), m_value(Vector2DValueType::kZero)
			{
			}

			template Variable<Vector2DValueType>::Variable();
        
			//------------------------------------------------------------------------------
			template<>
			Variable<Vector3DValueType>::Variable()
				: m_isEvaluated(false), m_value(Vector3DValueType::kZero)
			{
			}

			template Variable<Vector3DValueType>::Variable();
        
			//------------------------------------------------------------------------------
			template<>
			const UIDescriptionValueType Variable<BooleanValueType>::ms_valueType = UIDescriptionValueType::kBoolean;

			template<>
			const UIDescriptionValueType Variable<IntegerValueType>::ms_valueType = UIDescriptionValueType::kInteger;

			template<>
			const UIDescriptionValueType Variable<FloatingPointValueType>::ms_valueType = UIDescriptionValueType::kFloatingPoint;

			template<>
			const UIDescriptionValueType Variable<ColorARGBValueType>::ms_valueType = UIDescriptionValueType::kColor;

			template<>
			const UIDescriptionValueType Variable<Vector2DValueType>::ms_valueType = UIDescriptionValueType::kVector2D;

			template<>
			const UIDescriptionValueType Variable<Vector3DValueType>::ms_valueType = UIDescriptionValueType::kVector3D;

			template<>
			const UIDescriptionValueType Variable<StringValueType>::ms_valueType = UIDescriptionValueType::kString;
		}
	}
}
