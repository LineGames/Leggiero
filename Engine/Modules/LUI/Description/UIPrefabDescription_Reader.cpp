////////////////////////////////////////////////////////////////////////////////
// Description/UIPrefabDescription_Reader.cpp (Leggiero/Modules - LegacyUI)
//
// UI Prefab Description Reader Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIPrefabDescription_Reader.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "UIDescriptionExpression.h"

// Leggiero.LegacyUI Prefabs
#include "../Prefab/UIPrefabCommonObjects.h"
#include "../Prefab/UIObjectPrefab.h"
#include "../Prefab/UITextObjectPrefab.h"
#include "../Prefab/UITextInputBoxPrefab.h"
#include "../Prefab/UIObjectButtonPrefab.h"
#include "../Prefab/UIObjectSliderPrefab.h"
#include "../Prefab/UIObjectScrollPanelPrefab.h"
#include "../Prefab/UIComponentPrefab.h"
#include "../Prefab/UILayoutComponentPrefab.h"
#include "../Prefab/UITransformComponentPrefab.h"
#include "../Prefab/UIShapeRenderingComponentPrefab.h"
#include "../Prefab/UITextureRenderingComponentPrefab.h"
#include "../Prefab/UIPatchedTextureRenderingComponentPrefab.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				namespace Reader
				{
					//////////////////////////////////////////////////////////////////////////////// Reader from XML Data

					namespace XML
					{
						namespace _Internal
						{
							namespace ObjectPrefabTagNames
							{
								const char *kPlaceholder = "UIObjectPlaceHolder";
								const char *kImportPrefab = "ImportPrefab";

								const char *kBaseObject = "UIObject";

								const char *kTextLabelObject = "TextLabel";
								const char *kTextPrintObject = "TextPrint";
								const char *kCachedTextPrintObject = "CachedTextPrint";
								const char *kTextInputBoxObject = "TextInputBox";

								const char *kSimpleSingleButton = "SimpleSingleButton";
								const char *kSimpleSlider = "SimpleSlider";
								const char *kSimpleVerticalScrollPanel = "SimpleScrollPanelV";
								const char *kSimpleHorizontalScrollPanel = "SimpleScrollPanelH";
							}

							namespace ComponentPrefabTagNames
							{
								const char *kPlaceholder = "UIComponentPlaceHolder";

								const char *kLayout = "Layout";
								const char *kTransform = "Transform";

								const char *kRenderingShapeSet = "ShapeSet";
								const char *kRenderingTexture = "Texture";
								const char *kRenderingPatch = "Patch";
							}


							//////////////////////////////////////////////////////////////////////////////// Object

							//------------------------------------------------------------------------------
							void FillObjectPrefabCommonData(tinyxml2::XMLElement *elem, std::shared_ptr<IUIObjectPrefab> &createdPrefab)
							{
								createdPrefab->SetId(elem->Attribute("id"));

								const char *showConditionAttribute = elem->Attribute("showOn");
								if (showConditionAttribute != nullptr)
								{
									createdPrefab->SetShowCondition(Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(showConditionAttribute, UIDescriptionValueType::kBoolean)));
								}

								const char *hideConditionAttribute = elem->Attribute("hideOn");
								if (hideConditionAttribute != nullptr)
								{
									createdPrefab->SetHideCondition(Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(hideConditionAttribute, UIDescriptionValueType::kBoolean)));
								}
							}


							//////////////////////////////////////////////////////////////////////////////// Component

							//------------------------------------------------------------------------------
							std::shared_ptr<ComponentPlaceHolder> ReadComponentPlaceholderFromElement(tinyxml2::XMLElement *elem)
							{
								std::shared_ptr<ComponentPlaceHolder> createdComponentPrefab(std::make_shared<ComponentPlaceHolder>());

								for (const tinyxml2::XMLAttribute *currentAttribute = elem->FirstAttribute(); currentAttribute != nullptr; currentAttribute = currentAttribute->Next())
								{
									const char *attributeName = currentAttribute->Name();

									// Special Member
									if (strcmp(attributeName, "creatingKey") == 0)
									{
										createdComponentPrefab->SetCreatingKey(TypeUtility::ToVariableName(currentAttribute->Value()));
										continue;
									}

									// Parameters
									if (currentAttribute->Value() == nullptr)
									{
										createdComponentPrefab->AddParameter(attributeName, "");
									}
									else
									{
										createdComponentPrefab->AddParameter(attributeName, currentAttribute->Value());
									}
								}

								return createdComponentPrefab;
							}
						}

						//------------------------------------------------------------------------------
						// Read an Object Prefab from given XML Element
						std::shared_ptr<IUIObjectPrefab> ReadObjectPrefabFromElement(ParsingXMLElementType *elem)
						{
							const char *elementName = elem->Value();

							std::shared_ptr<IUIObjectPrefab> createdPrefab;
							if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kPlaceholder) == 0)
							{
								createdPrefab = ObjectPlaceHolder::ReadFromFromXMLElement(elem);
							}
							else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kImportPrefab) == 0)
							{
								createdPrefab = PrefabImportedObject::ReadFromFromXMLElement(elem);
							}

							// Object Prefabs
							if (!createdPrefab)
							{
								if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kBaseObject) == 0)
								{
									createdPrefab = UIObjectPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kTextLabelObject) == 0)
								{
									createdPrefab = TextLabelPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kTextPrintObject) == 0)
								{
									createdPrefab = TextPrintPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kCachedTextPrintObject) == 0)
								{
									createdPrefab = CachedTextPrintPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kTextInputBoxObject) == 0)
								{
									createdPrefab = TextInputBoxPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kSimpleSingleButton) == 0)
								{
									createdPrefab = SimpleSingleButtonPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kSimpleSlider) == 0)
								{
									createdPrefab = SimpleSliderPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kSimpleVerticalScrollPanel) == 0)
								{
									createdPrefab = SimpleVerticalScrollPanelPrefab::ReadFromFromXMLElement(elem);
								}
								else if (strcmp(elementName, _Internal::ObjectPrefabTagNames::kSimpleHorizontalScrollPanel) == 0)
								{
									createdPrefab = SimpleHorizontalScrollPanelPrefab::ReadFromFromXMLElement(elem);
								}
							}

							if (createdPrefab)
							{
								_Internal::FillObjectPrefabCommonData(elem, createdPrefab);
							}

							return createdPrefab;
						}

						//------------------------------------------------------------------------------
						// Read a Component Prefab from given XML Element
						std::shared_ptr<IUIComponentPrefab> ReadComponentPrefabFromElement(ParsingXMLElementType *elem)
						{
							const char *elementName = elem->Value();

							if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kPlaceholder) == 0)
							{
								return _Internal::ReadComponentPlaceholderFromElement(elem);
							}
							else if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kLayout) == 0)
							{
								return LayoutComponentPrefab::ReadFromFromXMLElement(elem);
							}
							else if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kTransform) == 0)
							{
								return TransformComponentPrefab::ReadFromFromXMLElement(elem);
							}
							else if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kRenderingShapeSet) == 0)
							{
								return ShapeSetRenderingComponentPrefab::ReadFromFromXMLElement(elem);
							}
							else if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kRenderingTexture) == 0)
							{
								return TextureRenderingComponentPrefab::ReadFromFromXMLElement(elem);
							}
							else if (strcmp(elementName, _Internal::ComponentPrefabTagNames::kRenderingPatch) == 0)
							{
								return PatchedTextureRenderingComponentPrefab::ReadFromFromXMLElement(elem);
							}

							return nullptr;
						}
					}
				}
			}
		}
	}
}
