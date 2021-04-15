////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionReader.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description Reader Logic
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionReader.h"

// Standard Library
#include <cstring>
#include <limits>

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionExpression.h"
#include "UIPrefabDescription.h"
#include "UIDescriptionColor.h"
#include "UIDescriptionTexture.h"
#include "UIDescriptionFont.h"
#include "UIDescriptionCommonExpression.h"
#include "UIPrefabDescription_Reader.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Reader
			{
				//////////////////////////////////////////////////////////////////////////////// Reader from XML Data

				namespace XML
				{
					namespace _Internal
					{
						namespace TagNames
						{
							const char *kImportUnitTag = "Import";
							const char *kNameSpaceGroupTag = "Namespace";
							const char *kOverridingNameSpaceTag = "NamespaceStack";

							const char *kAliasTag = "Alias";

							const char *kUIPrefabTag = "Prefab";

							const char *kSimpleVariableTag = "Define";
							const char *kExpressionVariableTag = "DefineExpression";
							const char *kColorVariableTag = "DefineColor";
							const char *kScaleValueVariableTag = "DefineScaledValue";

							const char *kImportTextureAtlasTag = "ImportTexture";
							const char *kSubTextureSegmentTag = "SubTexture";
							const char *kTextureSliceTag = "TextureSlice";

							const char *kFontClassTag = "FontClass";

							const char *kThreePatchClassTag = "ThreePatchClass";
							const char *kThreePatchHClassTag = "ThreePatchHClass";
							const char *kThreePatchVClassTag = "ThreePatchVClass";
							const char *kNinePatchClassTag = "NinePatchClass";
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<ImportedUnit> ReadUnitImportFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							const char *importingUnit = elem->Attribute("unit");
							if (importingUnit == nullptr)
							{
								return nullptr;
							}

							loadingManager.PreLoadUnit(importingUnit);

							return std::make_shared<ImportedUnit>(TypeUtility::ToSavedUnitName(importingUnit));
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<NameSpaceGroup> ReadNameSpaceGroupFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							std::shared_ptr<NameSpaceGroup> createdGroup(std::make_shared<NameSpaceGroup>());

							for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
							{
								std::shared_ptr<IDescriptionObject> nodeObject(ReadObjectFromElement(currentNode, loadingManager));
								if (!nodeObject)
								{
									continue;
								}

								VariableNameType objectName = ReadObjectNameFromElement(currentNode);
								if (objectName == TypeUtility::kInvalidName)
								{
									// No Named Object
									continue;
								}

								createdGroup->Register(objectName, nodeObject);
							}

							return createdGroup;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<OverridingNameSpace> ReadOverridingNameSpaceFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							const char *kIncludingNameSpaceTag = "IncludeNamespace";

							std::shared_ptr<OverridingNameSpace> createdNameSpace(std::make_shared<OverridingNameSpace>());

							for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
							{
								std::shared_ptr<IDescriptionNameSpaceContext> subNameSpace;
								const char *elementName = currentNode->Value();
								if (strcmp(elementName, TagNames::kImportUnitTag) == 0)
								{
									subNameSpace = ReadUnitImportFromElement(currentNode, loadingManager);
								}
								else if (strcmp(elementName, TagNames::kNameSpaceGroupTag) == 0)
								{
									subNameSpace = ReadNameSpaceGroupFromElement(currentNode, loadingManager);
								}
								else if (strcmp(elementName, kIncludingNameSpaceTag) == 0)
								{
									const char *includingTarget = currentNode->Attribute("for");
									if (includingTarget != nullptr)
									{
										VariablePathType includingTargetPath = TypeUtility::ParseVariablePath(includingTarget);
										if (includingTargetPath.size() == 1)
										{
											subNameSpace = std::make_shared<OverridingNameSpace::NameSpaceAliasByName>(includingTargetPath[0]);
										}
										else if (includingTargetPath.size() > 0)
										{
											subNameSpace = std::make_shared<OverridingNameSpace::NameSpaceAliasByPath>(includingTargetPath);
										}
									}
								}
								if (!subNameSpace)
								{
									continue;
								}

								createdNameSpace->Append(subNameSpace);
							}

							return createdNameSpace;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IDescriptionObject> ReadAliasFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							const char *aliasTarget = elem->Attribute("for");
							if (aliasTarget == nullptr)
							{
								return nullptr;
							}

							VariablePathType aliasTargetPath = TypeUtility::ParseVariablePath(aliasTarget);
							if (aliasTargetPath.size() == 1)
							{
								return std::make_shared<AliasName>(aliasTargetPath[0]);
							}
							else if (aliasTargetPath.size() > 0)
							{
								return std::make_shared<AliasPath>(aliasTargetPath);
							}

							return nullptr;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<UIPrefabDescription> ReadUIPrefabFromElement(tinyxml2::XMLElement *elem)
						{
							std::shared_ptr<UIPrefabDescription> createdPrefabDescription(std::make_shared<UIPrefabDescription>());

							for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
							{
								createdPrefabDescription->AddObjectToCreate(Prefab::Reader::XML::ReadObjectPrefabFromElement(currentNode));
							}

							return createdPrefabDescription;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IDescriptionObject> ReadConstantVariableFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							const char *valueAttribute = elem->Attribute("value");

							UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;

							const char *typeAttribute = elem->Attribute("type");
							if (typeAttribute != nullptr)
							{
								valueType = Expression::ParseTypeFromString(typeAttribute);
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Type
									return nullptr;
								}
							}

							if (valueAttribute == nullptr)
							{
								// Default Values
								switch (valueType)
								{
									case UIDescriptionValueType::kBoolean: return std::make_shared<BoolVariable>(false);
									case UIDescriptionValueType::kInteger: return std::make_shared<IntVariable>(0);
									case UIDescriptionValueType::kFloatingPoint: return std::make_shared<FloatVariable>(std::numeric_limits<FloatingPointValueType>::quiet_NaN());
									case UIDescriptionValueType::kColor: return std::make_shared<ColorVariable>(ColorARGBValueType::kBlack);
									case UIDescriptionValueType::kVector2D: return std::make_shared<Vector2DVariable>(Vector2DValueType::kInvalid);
									case UIDescriptionValueType::kVector3D: return std::make_shared<Vector3DVariable>(Vector3DValueType::kInvalid);
									case UIDescriptionValueType::kString: return std::make_shared<StringVariable>("");
								}

								// No Value
								return nullptr;
							}

							if (valueType == UIDescriptionValueType::kINVALID)
							{
								valueType = Expression::DetectTypeFromString(valueAttribute);
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Value
									return nullptr;
								}
							}

							switch (valueType)
							{
								case UIDescriptionValueType::kBoolean:
									return std::make_shared<BoolVariable>(Expression::ParseBooleanValueFromString(valueAttribute));

								case UIDescriptionValueType::kInteger:
									return std::make_shared<IntVariable>(Expression::ParseIntegerValueFromString(valueAttribute));

								case UIDescriptionValueType::kFloatingPoint:
									return std::make_shared<FloatVariable>(Expression::ParseFloatingPointValueFromString(valueAttribute));

								case UIDescriptionValueType::kString:
									return std::make_shared<StringVariable>(Expression::ParseStringValueFromString(valueAttribute));
							}

							return nullptr;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IDescriptionObject> ReadScaleVariableFromElement(tinyxml2::XMLElement *elem)
						{
							UIDescriptionValueType valueType = UIDescriptionValueType::kINVALID;
							const char *typeAttribute = elem->Attribute("type");
							if (typeAttribute != nullptr)
							{
								valueType = Expression::ParseTypeFromString(typeAttribute);
								if (valueType == UIDescriptionValueType::kINVALID)
								{
									// Invalid Type
									return nullptr;
								}
							}
							else
							{
								// Default Float
								valueType = UIDescriptionValueType::kFloatingPoint;
							}

							switch (valueType)
							{
								case UIDescriptionValueType::kInteger:
									return ScaledVariable<IntegerValueType>::ReadFromFromXMLElement(elem);

								case UIDescriptionValueType::kFloatingPoint:
									return ScaledVariable<FloatingPointValueType>::ReadFromFromXMLElement(elem);
							}

							return nullptr;
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IDescriptionObject> ReadColorVariableFromElement(tinyxml2::XMLElement *elem)
						{
							return std::make_shared<ColorVariable>(ReadColorFromElement(elem));
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IDescriptionObject> ReadImportTextureAtlasFromElement(tinyxml2::XMLElement *elem, DescriptionManager &loadingManager)
						{
							const char *textureSource = elem->Attribute("src");
							if (textureSource == nullptr)
							{
								return nullptr;
							}

							loadingManager.PreLoadTexture(textureSource);

							return std::make_shared<ImportedTexture>(textureSource);
						}
					}

					//------------------------------------------------------------------------------
					// Read object name from given XML Element
					VariableNameType ReadObjectNameFromElement(ParsingXMLElementType *elem)
					{
						const char *nameAttribute = elem->Attribute("name");
						if (nameAttribute != nullptr)
						{
							return TypeUtility::ToVariableName(nameAttribute);
						}

						// NO Name
						return TypeUtility::kInvalidName;
					}

					//------------------------------------------------------------------------------
					// Read a description object from given XML Element
					std::shared_ptr<IDescriptionObject> ReadObjectFromElement(ParsingXMLElementType *elem, DescriptionManager &loadingManager)
					{
						const char *elementName = elem->Value();

						if (strcmp(elementName, _Internal::TagNames::kImportUnitTag) == 0)
						{
							return _Internal::ReadUnitImportFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kNameSpaceGroupTag) == 0)
						{
							return _Internal::ReadNameSpaceGroupFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kOverridingNameSpaceTag) == 0)
						{
							return _Internal::ReadOverridingNameSpaceFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kAliasTag) == 0)
						{
							return _Internal::ReadAliasFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kUIPrefabTag) == 0)
						{
							return _Internal::ReadUIPrefabFromElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kSimpleVariableTag) == 0)
						{
							return _Internal::ReadConstantVariableFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kExpressionVariableTag) == 0)
						{
							return Expression::Reader::XML::ReadExpressionVariableFromElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kColorVariableTag) == 0)
						{
							return _Internal::ReadColorVariableFromElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kScaleValueVariableTag) == 0)
						{
							return _Internal::ReadScaleVariableFromElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kImportTextureAtlasTag) == 0)
						{
							return _Internal::ReadImportTextureAtlasFromElement(elem, loadingManager);
						}
						else if (strcmp(elementName, _Internal::TagNames::kSubTextureSegmentTag) == 0)
						{
							return SubTextureSegment::ReadFromFromXMLElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kTextureSliceTag) == 0)
						{
							return SlicedTexture::ReadFromFromXMLElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kFontClassTag) == 0)
						{
							return FontClass::ReadFromFromXMLElement(elem);
						}
						else if (strcmp(elementName, _Internal::TagNames::kThreePatchClassTag) == 0)
						{
							return ThreePatchTextureClass::ReadFromFromXMLElement(elem, UIDescriptionObjectType::kINVALID);
						}
						else if (strcmp(elementName, _Internal::TagNames::kThreePatchHClassTag) == 0)
						{
							return ThreePatchTextureClass::ReadFromFromXMLElement(elem, UIDescriptionObjectType::kThreePatchHClass);
						}
						else if (strcmp(elementName, _Internal::TagNames::kThreePatchVClassTag) == 0)
						{
							return ThreePatchTextureClass::ReadFromFromXMLElement(elem, UIDescriptionObjectType::kThreePatchVClass);
						}
						else if (strcmp(elementName, _Internal::TagNames::kNinePatchClassTag) == 0)
						{
							return NinePatchTextureClass::ReadFromFromXMLElement(elem);
						}

						return nullptr;
					}
				}
			}
		}
	}
}
