////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionColor.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description Color Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionColor.h"

// Standard Library
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <tuple>
#include <unordered_map>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>
#include <Utility/Sugar/SingletonPattern.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// External Library
#include <tinyxml2/tinyxml2.h>


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
						//------------------------------------------------------------------------------
						// Named color table
						class NamedColorTable
							: public Utility::DesignPattern::Singleton<NamedColorTable>
						{
							LEGGIERO_MAKE_SINGLETON_UNIQUE(NamedColorTable);

						protected:
							virtual void Initialize() override
							{
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("TRANSPARENT"), Graphics::GLByteARGB::kTransparent);
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("BLACK"), Graphics::GLByteARGB::kBlack);
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("WHITE"), Graphics::GLByteARGB::kWhite);
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("RED"), Graphics::GLByteARGB::kRed);
								//RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("GREEN"), Graphics::GLByteARGB::kGreen);	// Use web color name
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("BLUE"), Graphics::GLByteARGB::kBlue);

								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("SILVER"), Graphics::GLByteARGB::FromColorCodeARGB(0xFFC0C0C0U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("GRAY"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF808080U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("MAROON"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF800000U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("YELLOW"), Graphics::GLByteARGB::FromColorCodeARGB(0xFFFFFF00U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("OLIVE"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF808000U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("LIME"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF00FF00U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("GREEN"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF008000U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("AQUA"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF00FFFFU));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("CYAN"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF00FFFFU));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("TEAL"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF008080U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("NAVY"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF000080U));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("FUCHSIA"), Graphics::GLByteARGB::FromColorCodeARGB(0xFFFF00FFU));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("MAGENTA"), Graphics::GLByteARGB::FromColorCodeARGB(0xFFFF00FFU));
								RegisterColor(Utility::String::ASCIIStringUtility::ToUpper("PURPLE"), Graphics::GLByteARGB::FromColorCodeARGB(0xFF800080U));
							}

						public:
							void RegisterColor(const std::string &colorName, Graphics::GLByteARGB color)
							{
								m_colorTable[colorName] = color;
							}

							Graphics::GLByteARGB GetColor(const std::string &colorName) const
							{
								std::unordered_map<std::string, Graphics::GLByteARGB>::const_iterator findIt = m_colorTable.find(Utility::String::ASCIIStringUtility::ToUpper(colorName));
								return findIt->second;
							}

							bool HasColor(const std::string &colorName) const
							{
								std::unordered_map<std::string, Graphics::GLByteARGB>::const_iterator findIt = m_colorTable.find(Utility::String::ASCIIStringUtility::ToUpper(colorName));
								return (findIt != m_colorTable.cend());
							}

						protected:
							std::unordered_map<std::string, Graphics::GLByteARGB> m_colorTable;
						};

						//------------------------------------------------------------------------------
						std::tuple<GLubyte, GLubyte, GLubyte> ReadRGBColorComponentsFromElement(tinyxml2::XMLElement *elem)
						{
							if (elem->Attribute("color") != nullptr && NamedColorTable::GetInstance().HasColor(elem->Attribute("color")))
							{
								Graphics::GLByteARGB colorValue = NamedColorTable::GetInstance().GetColor(elem->Attribute("color"));
								return std::make_tuple(colorValue.red, colorValue.green, colorValue.blue);
							}

							const char *currentAttribute = nullptr;

							if (elem->Attribute("rgb") != nullptr)
							{
								currentAttribute = elem->Attribute("rgb");
								if (currentAttribute[0] == '#')
								{
									++currentAttribute;
								}
								uint32_t rgbValue = (uint32_t)std::strtoul(currentAttribute, nullptr, 16);
								Graphics::GLByteARGB colorValue = Graphics::GLByteARGB::FromColorCodeARGB(rgbValue);
								return std::make_tuple(colorValue.red, colorValue.green, colorValue.blue);
							}

							if ((elem->Attribute("h") != nullptr || elem->Attribute("dh") != nullptr)
								&& elem->Attribute("s") != nullptr
								&& elem->Attribute("l") != nullptr)
							{
								float hValue = 0.0f;
								if (elem->Attribute("h") != nullptr)
								{
									hValue = (float)atof(elem->Attribute("h"));
								}
								else if (elem->Attribute("dh") != nullptr)
								{
									hValue = (float)atof(elem->Attribute("dh")) / 360.0f;
								}

								float sValue = (float)atof(elem->Attribute("s"));
								float lValue = (float)atof(elem->Attribute("l"));

								Graphics::GLByteRGB colorValue = Graphics::HSLColor(hValue, sValue, lValue).ToRGB();
								return std::make_tuple(colorValue.red, colorValue.green, colorValue.blue);
							}

							GLubyte redValue = 0;
							if (elem->Attribute("r") != nullptr)
							{
								redValue = (GLubyte)atoi(elem->Attribute("r"));
							}
							else if (elem->Attribute("fr") != nullptr)
							{
								redValue = (GLubyte)((float)atof(elem->Attribute("fr")) * 255.0f);
							}

							GLubyte greenValue = 0;
							if (elem->Attribute("g") != nullptr)
							{
								greenValue = (GLubyte)atoi(elem->Attribute("g"));
							}
							else if (elem->Attribute("fg") != nullptr)
							{
								greenValue = (GLubyte)((float)atof(elem->Attribute("fg")) * 255.0f);
							}

							GLubyte blueValue = 0;
							if (elem->Attribute("b") != nullptr)
							{
								blueValue = (GLubyte)atoi(elem->Attribute("b"));
							}
							else if (elem->Attribute("fb") != nullptr)
							{
								blueValue = (GLubyte)((float)atof(elem->Attribute("fb")) * 255.0f);
							}

							return std::make_tuple(redValue, greenValue, blueValue);
						}
					}

					//------------------------------------------------------------------------------
					// Read Color Value from an XML Element
					ColorARGBValueType ReadColorFromElement(ParsingXMLElementType *elem)
					{
						const char *currentAttribute = nullptr;

						currentAttribute = elem->Attribute("argb");
						if (currentAttribute != nullptr)
						{
							if (currentAttribute[0] == '#')
							{
								++currentAttribute;
							}
							uint32_t argbValue = (uint32_t)std::strtoul(currentAttribute, nullptr, 16);
							return Graphics::GLByteARGB::FromColorCodeARGB(argbValue);
						}

						currentAttribute = elem->Attribute("rgba");
						if (currentAttribute != nullptr)
						{
							if (currentAttribute[0] == '#')
							{
								++currentAttribute;
							}
							uint32_t rgbaValue = (uint32_t)std::strtoul(currentAttribute, nullptr, 16);
							return Graphics::GLByteARGB::FromColorCodeRGBA(rgbaValue);
						}

						std::tuple<GLubyte, GLubyte, GLubyte> color = _Internal::ReadRGBColorComponentsFromElement(elem);

						GLubyte alpha = 255;
						currentAttribute = elem->Attribute("a");
						if (currentAttribute != nullptr)
						{
							alpha = (GLubyte)atoi(currentAttribute);
						}
						else
						{
							currentAttribute = elem->Attribute("fa");
							if (currentAttribute != nullptr)
							{
								float alphaFloat = (float)atof(currentAttribute) * 255.0f;
								if (alphaFloat < 0.0f)
								{
									alpha = 0;
								}
								else if (alphaFloat > 255.0f)
								{
									alpha = 255;
								}
								else
								{
									alpha = (GLubyte)alphaFloat;
								}
							}
						}

						return Graphics::GLByteARGB(alpha, std::get<0>(color), std::get<1>(color), std::get<2>(color));
					}
				}


				//////////////////////////////////////////////////////////////////////////////// Reader Utility

				ColorARGBValueType ReadColorValueFromCString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return ColorARGBValueType::kTransparent;
					}

					if (XML::_Internal::NamedColorTable::GetInstance().HasColor(stringValue))
					{
						return XML::_Internal::NamedColorTable::GetInstance().GetColor(stringValue);
					}

					return ColorARGBValueType::kTransparent;
				}
			}
		}
	}
}

LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::LUI::Description::Reader::XML::_Internal::NamedColorTable)
