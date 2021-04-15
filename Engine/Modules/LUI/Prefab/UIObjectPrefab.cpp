////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// UI Object Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIObjectPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UISizeSettable.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Description/UIPrefabDescription.h"
#include "../Description/UIDescriptionExpression.h"
#include "../Description/UIPrefabDescription_Reader.h"
#include "UIComponentPrefab.h"
#include "UIPrefabPlaceProcessor.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// UIObjectPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObjectPrefab> UIObjectPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<UIObjectPrefab> createdObject(std::make_shared<UIObjectPrefab>());

					createdObject->_ReadObjectCommonDataFromXMLElement(elem);

					return createdObject;
				}

				//------------------------------------------------------------------------------
				void UIObjectPrefab::_ReadObjectCommonDataFromXMLElement(ParsingXMLElementType *elem)
				{
					for (tinyxml2::XMLElement *currentChildNode = elem->FirstChildElement(); currentChildNode != nullptr; currentChildNode = currentChildNode->NextSiblingElement())
					{
						if (_IsObjectSpecialXMLElement(currentChildNode))
						{
							// Process by its custom handler
							continue;
						}

						const char *childName = currentChildNode->Value();

						if (strcmp(childName, "Components") == 0)
						{
							// Read Components
							for (tinyxml2::XMLElement *currentComponentNode = currentChildNode->FirstChildElement(); currentComponentNode != nullptr; currentComponentNode = currentComponentNode->NextSiblingElement())
							{
								std::shared_ptr<IUIComponentPrefab> componentPrefab(Reader::XML::ReadComponentPrefabFromElement(currentComponentNode));
								if (componentPrefab)
								{
									m_componentsPrefab.push_back(componentPrefab);
								}
							}
						}
						else if (strcmp(childName, "PreChildren") == 0)
						{
							// Read Pre-Children
							for (tinyxml2::XMLElement *currentSubChildNode = currentChildNode->FirstChildElement(); currentSubChildNode != nullptr; currentSubChildNode = currentSubChildNode->NextSiblingElement())
							{
								std::shared_ptr<IUIObjectPrefab> objectPrefab(Reader::XML::ReadObjectPrefabFromElement(currentSubChildNode));
								if (objectPrefab)
								{
									m_preChildrenPrefab.push_back(objectPrefab);
								}
							}
						}
						else if (strcmp(childName, "PostChildren") == 0)
						{
							// Read Post-Children
							for (tinyxml2::XMLElement *currentSubChildNode = currentChildNode->FirstChildElement(); currentSubChildNode != nullptr; currentSubChildNode = currentSubChildNode->NextSiblingElement())
							{
								std::shared_ptr<IUIObjectPrefab> objectPrefab(Reader::XML::ReadObjectPrefabFromElement(currentSubChildNode));
								if (objectPrefab)
								{
									m_postChildrenPrefab.push_back(objectPrefab);
								}
							}
						}
						else
						{
							// Try to read child
							std::shared_ptr<IUIObjectPrefab> objectPrefab(Reader::XML::ReadObjectPrefabFromElement(currentChildNode));
							if (objectPrefab)
							{
								bool isPreChild = false;
								if (currentChildNode->Attribute("isPre"))
								{
									isPreChild = static_cast<bool>(Expression::ParseBooleanValueFromString(currentChildNode->Attribute("isPre")));
								}
								if (isPreChild)
								{
									m_preChildrenPrefab.push_back(objectPrefab);
								}
								else
								{
									m_postChildrenPrefab.push_back(objectPrefab);
								}
							}
						}
					}
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> UIObjectPrefab::FabricateObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &parentPostProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					if (!IsShowing(processingContext))
					{
						return nullptr;
					}

					ComponentPostProcessCreatingContext postProcessContext;

					// Create
					std::shared_ptr<UIObject> createdObject(_CreateUIObject(processingContext, creatingManager, idTable, postProcessContext, placeProcessor));
					if (!createdObject)
					{
						return nullptr;
					}

					// Override Components
					std::shared_ptr<UILayoutComponent> layoutComponent;
					for (std::shared_ptr<IUIComponentPrefab> &componentPrefab : m_componentsPrefab)
					{
						std::shared_ptr<IUIComponent> createdComponent(componentPrefab->Fabricate(processingContext, creatingManager, createdObject, placeProcessor, postProcessContext));
						if (!createdComponent)
						{
							continue;
						}
						if (createdComponent->GetComponentType() == UIComponentType::kLayout)
						{
							layoutComponent = std::dynamic_pointer_cast<UILayoutComponent>(createdComponent);
						}
						else
						{
							createdObject->RegisterComponent(createdComponent);
						}
					}

					// Set Default Layout Component
					if (!createdObject->GetLayoutComponent())
					{
						if (!layoutComponent)
						{
							layoutComponent = std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(0.0f, 0.0f));
						}
					}
					if (layoutComponent)
					{
						createdObject->ChangeLayoutComponent(layoutComponent);
					}

					// Post Process
					_RegisterToIdTable(idTable, createdObject);

					// Create Children
					for (std::shared_ptr<IUIObjectPrefab> &currentChildPrefab : m_preChildrenPrefab)
					{
						std::shared_ptr<UIObject> fabricatedObject(currentChildPrefab->FabricateObject(processingContext, creatingManager, idTable, postProcessContext, placeProcessor));
						if (!fabricatedObject)
						{
							continue;
						}
						createdObject->AddPreUIObject(fabricatedObject);
					}
					for (std::shared_ptr<IUIObjectPrefab> &currentChildPrefab : m_postChildrenPrefab)
					{
						std::shared_ptr<UIObject> fabricatedObject(currentChildPrefab->FabricateObject(processingContext, creatingManager, idTable, postProcessContext, placeProcessor));
						if (!fabricatedObject)
						{
							continue;
						}
						createdObject->AddPostUIObject(fabricatedObject);
					}

					// Post Process
					_PostProcessAfterFabrication(createdObject, processingContext, postProcessContext, parentPostProcessContext);

					return createdObject;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> UIObjectPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					return std::make_shared<UIObject>(creatingManager, nullptr, true);
				}

				//------------------------------------------------------------------------------
				void UIObjectPrefab::_PostProcessAfterFabrication(std::shared_ptr<UIObject> createdObject, DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &postProcessContext, ComponentPostProcessCreatingContext &parentPostProcessContext)
				{
					postProcessContext.DoPostProcessAfterFabrication(createdObject, processingContext, parentPostProcessContext);
				}


				//////////////////////////////////////////////////////////////////////////////// ObjectPlaceHolder

				//------------------------------------------------------------------------------
				std::shared_ptr<ObjectPlaceHolder> ObjectPlaceHolder::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<ObjectPlaceHolder> createdObject(std::make_shared<ObjectPlaceHolder>());

					for (const tinyxml2::XMLAttribute *currentAttribute = elem->FirstAttribute(); currentAttribute != nullptr; currentAttribute = currentAttribute->Next())
					{
						const char *attributeName = currentAttribute->Name();

						// Special Member
						if (strcmp(attributeName, "creatingKey") == 0)
						{
							createdObject->SetCreatingKey(TypeUtility::ToVariableName(currentAttribute->Value()));
							continue;
						}

						// Common Data - for skip
						if (strcmp(attributeName, "id") == 0
							|| strcmp(attributeName, "showOn") == 0
							|| strcmp(attributeName, "hideOn") == 0)
						{
							continue;
						}

						// Parameters
						if (currentAttribute->Value() == nullptr)
						{
							createdObject->AddParameter(attributeName, "");
						}
						else
						{
							createdObject->AddParameter(attributeName, currentAttribute->Value());
						}
					}

					createdObject->_ReadObjectCommonDataFromXMLElement(elem);

					return createdObject;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> ObjectPlaceHolder::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					if (placeProcessor == nullptr)
					{
						return nullptr;
					}

					return placeProcessor->ProcessObjectPlaceholderFabrication(creatingManager, m_creatingKey, m_parameters, postProcessContext);
				}


				//////////////////////////////////////////////////////////////////////////////// PrefabImportedObject

				//------------------------------------------------------------------------------
				std::shared_ptr<PrefabImportedObject> PrefabImportedObject::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *importingPrefabPathAttribute = elem->Attribute("prefab");
					if (importingPrefabPathAttribute == nullptr)
					{
						return nullptr;
					}

					std::shared_ptr<PrefabImportedObject> createdObject(std::make_shared<PrefabImportedObject>(TypeUtility::ParseVariablePath(importingPrefabPathAttribute)));

					createdObject->_ReadObjectCommonDataFromXMLElement(elem);

					return createdObject;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> PrefabImportedObject::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					// Find Prefab
					ObjectEnvironmentPair prefabFindResult = processingContext.Find(m_prefabPath);
					if (!std::get<0>(prefabFindResult))
					{
						return nullptr;
					}
					std::shared_ptr<UIPrefabDescription> prefabDescription(std::dynamic_pointer_cast<UIPrefabDescription>(std::get<0>(prefabFindResult)));
					if (!prefabDescription)
					{
						return nullptr;
					}

					// Create
					std::shared_ptr<UIObject> createdObject(prefabDescription->FabricateInParentContext(std::get<1>(prefabFindResult), creatingManager, postProcessContext, placeProcessor));
					if (!createdObject)
					{
						return nullptr;
					}

					return createdObject;
				}
			}
		}
	}
}
