////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionUnit.cpp (Leggiero/Modules - LegacyUI)
//
// Description Unit Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionUnit.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionReader.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// DescriptionUnit

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionUnit::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
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
			std::shared_ptr<DescriptionUnit> DescriptionUnit::LoadFromXML(std::string xmlData, DescriptionManager &loadingManager)
			{
				tinyxml2::XMLDocument dataDoc;
				if (dataDoc.Parse(xmlData.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
				{
					return nullptr;
				}
				tinyxml2::XMLElement *rootElem = dataDoc.RootElement();
				if (rootElem == NULL)
				{
					return nullptr;
				}

				try
				{
					std::shared_ptr<DescriptionUnit> createdUnit(std::make_shared<DescriptionUnit>());

					for (tinyxml2::XMLElement *currentNode = rootElem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
					{
						std::shared_ptr<IDescriptionObject> nodeObject(Reader::XML::ReadObjectFromElement(currentNode, loadingManager));
						if (!nodeObject)
						{
							continue;
						}

						VariableNameType objectName = Reader::XML::ReadObjectNameFromElement(currentNode);
						if (objectName == TypeUtility::kInvalidName)
						{
							// No Named Object
							continue;
						}

						createdUnit->m_descriptionTable[objectName] = nodeObject;
					}

					return createdUnit;
				}
				catch (...)
				{
					// Invalid Data
				}

				return nullptr;
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionUnit::SelectDescriptionByName(DescriptionManager &manager, const VariableNameType &name)
			{
				DescriptionProcessingContext contextOfUnit(manager, shared_from_this());
				return Select(name, contextOfUnit);
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionUnit::SelectDescriptionByPath(DescriptionManager &manager, const std::string &path)
			{
				return SelectDescriptionByPath(manager, TypeUtility::ParseVariablePath(path));
			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair DescriptionUnit::SelectDescriptionByPath(DescriptionManager &manager, const VariablePathType &path)
			{
				DescriptionProcessingContext contextOfUnit(manager, shared_from_this());
				return IDescriptionNameSpaceContext::Select(path, contextOfUnit);
			}

			//------------------------------------------------------------------------------
			void DescriptionUnit::AddObject(const VariableNameType &name, std::shared_ptr<IDescriptionObject> descriptionObject)
			{
				if (!descriptionObject)
				{
					m_descriptionTable.erase(name);
					return;
				}
				m_descriptionTable[name] = descriptionObject;
			}
		}
	}
}
