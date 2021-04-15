////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of Expressions in UI Description
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionExpression.h"

// Additional Dependent Header
#include "UIDescriptionExpression_Nodes.h"

// Standard Library
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

// External Library
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>

// Leggiero.LegacyUI
#include "UIDescriptionProcessingContext.h"
#include "UIDescriptionTexture.h"
#include "UIDescriptionColor.h"
#include "UIDescriptionParserUtility.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Expression
			{
				//------------------------------------------------------------------------------
				template<>
				const UIDescriptionValueType IExpression<BooleanValueType>::ms_valueType = UIDescriptionValueType::kBoolean;

				template<>
				const UIDescriptionValueType IExpression<IntegerValueType>::ms_valueType = UIDescriptionValueType::kInteger;

				template<>
				const UIDescriptionValueType IExpression<FloatingPointValueType>::ms_valueType = UIDescriptionValueType::kFloatingPoint;

				template<>
				const UIDescriptionValueType IExpression<ColorARGBValueType>::ms_valueType = UIDescriptionValueType::kColor;

				template<>
				const UIDescriptionValueType IExpression<Vector2DValueType>::ms_valueType = UIDescriptionValueType::kVector2D;

				template<>
				const UIDescriptionValueType IExpression<Vector3DValueType>::ms_valueType = UIDescriptionValueType::kVector3D;

				template<>
				const UIDescriptionValueType IExpression<StringValueType>::ms_valueType = UIDescriptionValueType::kString;


				//////////////////////////////////////////////////////////////////////////////// Parsing

				namespace _Internal
				{
					namespace TypeStrings
					{
						const char *kBoolean = "bool";
						const char *kInteger = "int";
						const char *kFloatingPoint = "float";
						const char *kColorARGB = "color";
						const char *kVector2D = "vec2";
						const char *kvector3D = "vec3";
						const char *kString = "string";
					}

					//------------------------------------------------------------------------------
					std::tuple<BooleanValueType, bool> _ParseBooleanValueFromStringWithValidity(const char *stringValue)
					{
						if (stringValue == nullptr)
						{
							return std::make_tuple<BooleanValueType, bool>(false, false);
						}

						std::string trimmedString(Utility::String::ASCIIStringUtility::StringTrim(stringValue));
						if (trimmedString.length() < 1)
						{
							return std::make_tuple<BooleanValueType, bool>(false, false);
						}

						switch (trimmedString.length())
						{
							case 1:
								{
									switch (trimmedString[0])
									{
										case 'T':
										case 't':
										case '1':
										case 'O':
										case 'Y':
										case 'y':
											return std::make_tuple<BooleanValueType, bool>(true, true);

										case 'F':
										case 'f':
										case '0':
										case 'X':
										case 'N':
										case 'n':
											return std::make_tuple<BooleanValueType, bool>(false, true);
									}
								}
								break;

							case 2:
								{
									if ((trimmedString[0] == 'N' || trimmedString[0] == 'n')
										&& (trimmedString[1] == 'O' || trimmedString[1] == 'o'))
									{
										return std::make_tuple<BooleanValueType, bool>(false, true);
									}
									if ((trimmedString[0] == 'O' || trimmedString[0] == 'o')
										&& (trimmedString[1] == 'K' || trimmedString[1] == 'k'))
									{
										return std::make_tuple<BooleanValueType, bool>(true, true);
									}
								}
								break;

							case 3:
								{
									if ((trimmedString[0] == 'Y' || trimmedString[0] == 'y')
										&& (trimmedString[1] == 'E' || trimmedString[1] == 'e')
										&& (trimmedString[2] == 'S' || trimmedString[2] == 's'))
									{
										return std::make_tuple<BooleanValueType, bool>(true, true);
									}
								}
								break;

							case 4:
								{
									if ((trimmedString[0] == 'T' || trimmedString[0] == 't')
										&& (trimmedString[1] == 'R' || trimmedString[1] == 'r')
										&& (trimmedString[2] == 'U' || trimmedString[2] == 'u')
										&& (trimmedString[3] == 'E' || trimmedString[3] == 'e'))
									{
										return std::make_tuple<BooleanValueType, bool>(true, true);
									}
								}
								break;

							case 5:
								{
									if ((trimmedString[0] == 'F' || trimmedString[0] == 'f')
										&& (trimmedString[1] == 'A' || trimmedString[1] == 'a')
										&& (trimmedString[2] == 'L' || trimmedString[2] == 'l')
										&& (trimmedString[3] == 'S' || trimmedString[3] == 's')
										&& (trimmedString[4] == 'E' || trimmedString[4] == 'e'))
									{
										return std::make_tuple<BooleanValueType, bool>(false, true);
									}
								}
								break;
						}

						return std::make_tuple<BooleanValueType, bool>(false, false);
					}
				}

				//------------------------------------------------------------------------------
				UIDescriptionValueType ParseTypeFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return UIDescriptionValueType::kINVALID;
					}

					if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kBoolean))
					{
						return UIDescriptionValueType::kBoolean;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kInteger))
					{
						return UIDescriptionValueType::kInteger;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kFloatingPoint))
					{
						return UIDescriptionValueType::kFloatingPoint;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kColorARGB))
					{
						return UIDescriptionValueType::kColor;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kVector2D))
					{
						return UIDescriptionValueType::kVector2D;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kvector3D))
					{
						return UIDescriptionValueType::kVector3D;
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringValue, _Internal::TypeStrings::kString))
					{
						return UIDescriptionValueType::kString;
					}

					return UIDescriptionValueType::kINVALID;
				}

				//------------------------------------------------------------------------------
				UIDescriptionValueType DetectTypeFromString(const char *stringValue)
				{
					if (stringValue != nullptr)
					{
						std::string trimmedString(Utility::String::ASCIIStringUtility::StringTrim(stringValue));
						if (trimmedString.length() == 0)
						{
							return UIDescriptionValueType::kString;
						}

						if (trimmedString.length() == 1)
						{
							switch (trimmedString[0])
							{
								case 'T':
								case 't':
								case 'O':
								case 'Y':
								case 'y':
								case 'F':
								case 'f':
								case 'X':
								case 'N':
								case 'n':
									return UIDescriptionValueType::kBoolean;
							}
							if (isdigit(trimmedString[0]))
							{
								return UIDescriptionValueType::kInteger;
							}
							return UIDescriptionValueType::kString;
						}

						if (trimmedString.length() == 2)
						{
							if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "no")
								|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "ok"))
							{
								return UIDescriptionValueType::kBoolean;
							}
						}
						else if (trimmedString.length() == 3)
						{
							if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "yes"))
							{
								return UIDescriptionValueType::kBoolean;
							}
						}
						else if (trimmedString.length() == 4)
						{
							if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "true"))
							{
								return UIDescriptionValueType::kBoolean;
							}
						}
						else if (trimmedString.length() == 5)
						{
							if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "false"))
							{
								return UIDescriptionValueType::kBoolean;
							}
						}

						// Special Floating Values
						if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "INF")
							|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "INFINITY")
							|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(trimmedString.c_str(), "NAN"))
						{
							return UIDescriptionValueType::kFloatingPoint;
						}

						// Check constructors
						std::tuple<bool, std::string, std::string> callTuple = Parsing::ParsingUtility::ExtractFunctionStyleCall(trimmedString);
						if (std::get<0>(callTuple))
						{
							std::string constructorString(Utility::String::ASCIIStringUtility::ToLower(std::get<1>(callTuple)));

							static const std::unordered_set<std::string> _kColorConstructors({ "argb", "rgba", "rgb", "fargb", "frgba", "frgb", "hsl", "dhsl", "ahsl", "adhsl" });
							static const std::unordered_set<std::string> _kVector2DConstructors({ "vec2", "vec2d", "vector2", "vector2d" });
							static const std::unordered_set<std::string> _kVector3DConstructors({ "vec3", "vec3d", "vector3", "vector3d" });
							static const std::unordered_set<std::string> _kGenericVectorConstructors({ "vec", "vector" });

							if (_kColorConstructors.find(constructorString) != _kColorConstructors.cend())
							{
								return UIDescriptionValueType::kColor;
							}
							if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(constructorString, "vec"))
							{
								if (_kVector2DConstructors.find(constructorString) != _kVector2DConstructors.cend())
								{
									return UIDescriptionValueType::kVector2D;
								}
								if (_kVector3DConstructors.find(constructorString) != _kVector3DConstructors.cend())
								{
									return UIDescriptionValueType::kVector3D;
								}
								if (_kGenericVectorConstructors.find(constructorString) != _kGenericVectorConstructors.cend())
								{
									std::vector<std::string> vecArgumentTokens = Utility::String::ASCIIStringUtility::Tokenize(std::get<2>(callTuple), { ',' });
									if (vecArgumentTokens.size() == 2)
									{
										return UIDescriptionValueType::kVector2D;
									}
									else if (vecArgumentTokens.size() == 3)
									{
										return UIDescriptionValueType::kVector3D;
									}
								}
							}
						}

						// Check number
						size_t numberCheckOffset = 0;
						if (trimmedString[numberCheckOffset] == '+' || trimmedString[numberCheckOffset] == '-')
						{
							++numberCheckOffset;
						}
						bool isHexNumber = false;
						if (trimmedString.length() >= numberCheckOffset + 2)
						{
							if (trimmedString[numberCheckOffset] == '0'
								&& (trimmedString[numberCheckOffset + 1] == 'x' || trimmedString[numberCheckOffset + 1] == 'X'))
							{
								isHexNumber = true;
								numberCheckOffset += 2;
							}
						}
						if (trimmedString.length() <= numberCheckOffset)
						{
							return UIDescriptionValueType::kString;
						}

						bool isNumber = true;
						for (size_t i = numberCheckOffset; i <= trimmedString.length(); ++i)
						{
							if (isHexNumber)
							{
								if (!isxdigit(trimmedString[i]))
								{
									isNumber = false;
									break;
								}
							}
							else
							{
								if (!isdigit(trimmedString[i]))
								{
									isNumber = false;
									break;
								}
							}
						}
						if (isNumber)
						{
							return UIDescriptionValueType::kInteger;
						}

						const char *trimmedStringCString = trimmedString.c_str();
						char *trimmedStringCStringEnd;
						strtof(trimmedStringCString, &trimmedStringCStringEnd);
						if (trimmedStringCStringEnd == trimmedStringCString + trimmedString.length())
						{
							return UIDescriptionValueType::kFloatingPoint;
						}

						return UIDescriptionValueType::kString;
					}

					return UIDescriptionValueType::kINVALID;
				}

				//------------------------------------------------------------------------------
				BooleanValueType ParseBooleanValueFromString(const char *stringValue)
				{
					return std::get<0>(_Internal::_ParseBooleanValueFromStringWithValidity(stringValue));
				}

				//------------------------------------------------------------------------------
				IntegerValueType ParseIntegerValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return 0;
					}
					return static_cast<IntegerValueType>(strtol(stringValue, nullptr, 0));
				}

				//------------------------------------------------------------------------------
				FloatingPointValueType ParseFloatingPointValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return std::numeric_limits<FloatingPointValueType>::quiet_NaN();
					}
					return static_cast<FloatingPointValueType>(strtof(stringValue, nullptr));
				}

				//------------------------------------------------------------------------------
				ColorARGBValueType ParseColorValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return ColorARGBValueType::kTransparent;
					}

					ColorARGBValueType colorFromTable = Description::Reader::ReadColorValueFromCString(stringValue);
					if (colorFromTable != ColorARGBValueType::kTransparent)
					{
						return colorFromTable;
					}

					std::string stringStringValue(Utility::String::ASCIIStringUtility::StringTrim(stringValue));
					if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(stringStringValue, "transparent"))
					{
						// Real transparent
						return ColorARGBValueType::kTransparent;
					}

					// Hex code
					if (stringStringValue.length() > 1 && stringStringValue[0] == '#')
					{
						std::string hexColorValue = Utility::String::ASCIIStringUtility::FilterString(stringStringValue.substr(1));
						uint32_t colorCodeValue = (uint32_t)std::strtoul(hexColorValue.c_str() + 1, nullptr, 16);
						if (hexColorValue.length() <= 7)
						{
							colorCodeValue |= 0xff000000;
						}
						return ColorARGBValueType(Graphics::GLByteARGB::FromColorCodeARGB(colorCodeValue));
					}

					// Check call-style construct
					std::tuple<bool, std::string, std::string> callTuple = Parsing::ParsingUtility::ExtractFunctionStyleCall(stringStringValue);
					if (std::get<0>(callTuple))
					{
						if (std::get<1>(callTuple).empty())
						{
							// Considered as tuple-style construct
							stringStringValue = std::get<2>(callTuple);
						}
						else
						{
							std::string colorConstructor(std::get<1>(callTuple));
							std::vector<std::string> argumentTokens = Utility::String::ASCIIStringUtility::Tokenize(std::get<2>(callTuple), { ',' });
							for (size_t i = 0; i < argumentTokens.size(); ++i)
							{
								argumentTokens[i] = Utility::String::ASCIIStringUtility::StringTrim(argumentTokens[i]);
							}
							if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "argb"))
							{
								if (argumentTokens.size() < 4) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[0].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[1].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[2].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[3].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "rgba"))
							{
								if (argumentTokens.size() < 4) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[3].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[0].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[1].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[2].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "rgb"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(static_cast<GLubyte>(255), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[0].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[1].c_str())), static_cast<GLubyte>(ParseIntegerValueFromString(argumentTokens[2].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "fargb"))
							{
								if (argumentTokens.size() < 4) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[0].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[3].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "frgba"))
							{
								if (argumentTokens.size() < 4) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[3].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[0].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "frgb"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								return ColorARGBValueType(1.0f, static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[0].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str())));
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "hsl"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								Graphics::GLByteRGB colorValue = Graphics::HSLColor(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[0].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str()))).ToRGB();
								return ColorARGBValueType(static_cast<GLubyte>(255), colorValue.red, colorValue.green, colorValue.blue);
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "dhsl"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								Graphics::GLByteRGB colorValue = Graphics::HSLColor(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[0].c_str())) / 360.0f, static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str()))).ToRGB();
								return ColorARGBValueType(static_cast<GLubyte>(255), colorValue.red, colorValue.green, colorValue.blue);
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "ahsl"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								Graphics::GLByteRGB colorValue = Graphics::HSLColor(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[3].c_str()))).ToRGB();
								return ColorARGBValueType(static_cast<GLubyte>(ParseFloatingPointValueFromString(argumentTokens[0].c_str()) * 255.0f), colorValue.red, colorValue.green, colorValue.blue);
							}
							else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(colorConstructor, "adhsl"))
							{
								if (argumentTokens.size() < 3) { return ColorARGBValueType::kTransparent; } // Invalid Arguments
								Graphics::GLByteRGB colorValue = Graphics::HSLColor(static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[1].c_str())) / 360.0f, static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[2].c_str())), static_cast<float>(ParseFloatingPointValueFromString(argumentTokens[3].c_str()))).ToRGB();
								return ColorARGBValueType(static_cast<GLubyte>(ParseFloatingPointValueFromString(argumentTokens[0].c_str()) * 255.0f), colorValue.red, colorValue.green, colorValue.blue);
							}
						}
					}
					
					// Parse tuple-style construct
					std::vector<std::string> tupleTokens = Utility::String::ASCIIStringUtility::Tokenize(stringStringValue, { ',' });
					if (tupleTokens.empty() || (tupleTokens.size() == 1 && tupleTokens[0].empty()))
					{
						return ColorARGBValueType::kTransparent;
					}
					else if (tupleTokens.size() == 1)
					{
						// Can be hex code
						std::string hexColorValue = Utility::String::ASCIIStringUtility::FilterString(tupleTokens[0]);
						uint32_t colorCodeValue = (uint32_t)std::strtoul(hexColorValue.c_str(), nullptr, 16);
						if (hexColorValue.length() <= 6)
						{
							colorCodeValue |= 0xff000000;
						}
						return ColorARGBValueType(Graphics::GLByteARGB::FromColorCodeARGB(colorCodeValue));
					}
					else if (tupleTokens.size() == 2)
					{
						// Can be hex code + alpha
						std::string hexColorValue = Utility::String::ASCIIStringUtility::FilterString(tupleTokens[0]);
						uint32_t colorCodeValue = ((uint32_t)std::strtoul(hexColorValue.c_str(), nullptr, 16)) & 0x00ffffffu;
						uint32_t alphaValue = static_cast<uint32_t>(ParseIntegerValueFromString(tupleTokens[1].c_str())) << 24;
						return ColorARGBValueType(Graphics::GLByteARGB::FromColorCodeARGB(colorCodeValue | alphaValue));
					}

					float tokenValues[4];
					if (tupleTokens.size() > 4)
					{
						tupleTokens.resize(4);
					}

					bool isForceFloatValues = false;
					for (size_t i = 0; i < tupleTokens.size(); ++i)
					{
						if (!isForceFloatValues && (tupleTokens[i].find('.') != std::string::npos))
						{
							// Has point
							isForceFloatValues = true;
						}

						tokenValues[i] = static_cast<float>(ParseFloatingPointValueFromString(Utility::String::ASCIIStringUtility::StringTrim(tupleTokens[i]).c_str()));
						if (tokenValues[i] < 0.0f)
						{
							tokenValues[i] = 0.0f;
						}
					}
					if (tupleTokens.size() == 3)
					{
						for (size_t i = 0; i < 3; ++i)
						{
							tokenValues[3 - i] = tokenValues[2 - i];
						}
					}

					if (isForceFloatValues)
					{
						if (tupleTokens.size() == 3)
						{
							tokenValues[0] = 1.0f;
						}
						return ColorARGBValueType(tokenValues[0], tokenValues[1], tokenValues[2], tokenValues[3]);
					}
					else
					{
						if (tupleTokens.size() == 3)
						{
							tokenValues[0] = 255.0f;
						}
						return ColorARGBValueType(static_cast<GLubyte>(tokenValues[0]), static_cast<GLubyte>(tokenValues[1]), static_cast<GLubyte>(tokenValues[2]), static_cast<GLubyte>(tokenValues[3]));
					}

					return ColorARGBValueType::kTransparent;
				}

				//------------------------------------------------------------------------------
				Vector2DValueType ParseVector2DValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return Vector2DValueType::kZero;
					}

					std::string stringStringValue(Utility::String::ASCIIStringUtility::StringTrim(stringValue));

					// Check call-style construct
					std::tuple<bool, std::string, std::string> callTuple = Parsing::ParsingUtility::ExtractFunctionStyleCall(stringStringValue);
					if (std::get<0>(callTuple))
					{
						if (std::get<1>(callTuple).empty())
						{
							// Considered as tuple-style construct
							stringStringValue = std::get<2>(callTuple);
						}
						else
						{
							std::string vectorConstructor(std::get<1>(callTuple));
							std::vector<std::string> argumentTokens = Utility::String::ASCIIStringUtility::Tokenize(std::get<2>(callTuple), { ',' });
							for (size_t i = 0; i < argumentTokens.size(); ++i)
							{
								argumentTokens[i] = Utility::String::ASCIIStringUtility::StringTrim(argumentTokens[i]);
							}
							if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(vectorConstructor, "vec"))
							{
								switch (argumentTokens.size())
								{
									case 0:	return Vector2DValueType::kZero;	// Default Constructor

									case 1:
										{
											if (argumentTokens[0].empty())
											{
												// Also Default Constructor
												return Vector2DValueType::kZero;
											}

											FloatingPointValueType componentValue = ParseFloatingPointValueFromString(argumentTokens[0].c_str());
											return Vector2DValueType(componentValue, componentValue);
										}

									default:
										{
											return Vector2DValueType(ParseFloatingPointValueFromString(argumentTokens[0].c_str()), ParseFloatingPointValueFromString(argumentTokens[1].c_str()));
										}
								}
							}
							else
							{
								// Something wrong
								return Vector2DValueType::kZero;
							}
						}
					}

					// Tuple-style
					std::vector<std::string> tupleTokens = Utility::String::ASCIIStringUtility::Tokenize(stringStringValue, { ',' });
					if (tupleTokens.empty() || (tupleTokens.size() == 1 && tupleTokens[0].empty()))
					{
						// Default Constructor
						return Vector2DValueType::kZero;
					}

					switch (tupleTokens.size())
					{
						case 1:
							{
								FloatingPointValueType componentValue = ParseFloatingPointValueFromString(tupleTokens[0].c_str());
								return Vector2DValueType(componentValue, componentValue);
							}
					}

					return Vector2DValueType(ParseFloatingPointValueFromString(tupleTokens[0].c_str()), ParseFloatingPointValueFromString(tupleTokens[1].c_str()));
				}

				//------------------------------------------------------------------------------
				Vector3DValueType ParseVector3DValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return Vector3DValueType::kZero;
					}

					std::string stringStringValue(Utility::String::ASCIIStringUtility::StringTrim(stringValue));

					// Check call-style construct
					std::tuple<bool, std::string, std::string> callTuple = Parsing::ParsingUtility::ExtractFunctionStyleCall(stringStringValue);
					if (std::get<0>(callTuple))
					{
						if (std::get<1>(callTuple).empty())
						{
							// Considered as tuple-style construct
							stringStringValue = std::get<2>(callTuple);
						}
						else
						{
							std::string vectorConstructor(std::get<1>(callTuple));
							std::vector<std::string> argumentTokens = Utility::String::ASCIIStringUtility::Tokenize(std::get<2>(callTuple), { ',' });
							for (size_t i = 0; i < argumentTokens.size(); ++i)
							{
								argumentTokens[i] = Utility::String::ASCIIStringUtility::StringTrim(argumentTokens[i]);
							}
							if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(vectorConstructor, "vec2")
								|| Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(vectorConstructor, "vector2"))
							{
								switch (argumentTokens.size())
								{
									case 0:	return Vector3DValueType::kZero;	// Default Constructor

									case 1:
										{
											if (argumentTokens[0].empty())
											{
												// Also Default Constructor
												return Vector3DValueType::kZero;
											}

											// 2D vector to 3D vector conversion
											FloatingPointValueType componentValue = ParseFloatingPointValueFromString(argumentTokens[0].c_str());
											return Vector3DValueType(componentValue, componentValue, static_cast<FloatingPointValueType>(0.0f));
										}

									default:
										{
											return Vector3DValueType(ParseFloatingPointValueFromString(argumentTokens[0].c_str()), ParseFloatingPointValueFromString(argumentTokens[1].c_str()), static_cast<FloatingPointValueType>(0.0f));
										}
								}
							}
							else if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(vectorConstructor, "vec"))
							{
								switch (argumentTokens.size())
								{
									case 0:	return Vector3DValueType::kZero;	// Default Constructor

									case 1:
										{
											if (argumentTokens[0].empty())
											{
												// Also Default Constructor
												return Vector3DValueType::kZero;
											}

											FloatingPointValueType componentValue = ParseFloatingPointValueFromString(argumentTokens[0].c_str());
											return Vector3DValueType(componentValue, componentValue, componentValue);
										}

									case 2:
										{
											return Vector3DValueType(ParseFloatingPointValueFromString(argumentTokens[0].c_str()), ParseFloatingPointValueFromString(argumentTokens[1].c_str()), static_cast<FloatingPointValueType>(0.0f));
										}

									default:
										{
											return Vector2DValueType(ParseFloatingPointValueFromString(argumentTokens[0].c_str()), ParseFloatingPointValueFromString(argumentTokens[1].c_str()));
										}
								}
							}
							else
							{
								// Something wrong
								return Vector3DValueType::kZero;
							}
						}
					}

					// Tuple-style
					std::vector<std::string> tupleTokens = Utility::String::ASCIIStringUtility::Tokenize(stringStringValue, { ',' });
					if (tupleTokens.empty() || (tupleTokens.size() == 1 && tupleTokens[0].empty()))
					{
						// Default Constructor
						return Vector3DValueType::kZero;
					}

					switch (tupleTokens.size())
					{
						case 1:
							{
								FloatingPointValueType componentValue = ParseFloatingPointValueFromString(tupleTokens[0].c_str());
								return Vector3DValueType(componentValue, componentValue, componentValue);
							}

						case 2:
							{
								return Vector3DValueType(ParseFloatingPointValueFromString(tupleTokens[0].c_str()), ParseFloatingPointValueFromString(tupleTokens[1].c_str()), static_cast<FloatingPointValueType>(0.0f));
							}
					}

					return Vector3DValueType(ParseFloatingPointValueFromString(tupleTokens[0].c_str()), ParseFloatingPointValueFromString(tupleTokens[1].c_str()), ParseFloatingPointValueFromString(tupleTokens[2].c_str()));
				}

				//------------------------------------------------------------------------------
				StringValueType ParseStringValueFromString(const char *stringValue)
				{
					if (stringValue == nullptr)
					{
						return "";
					}
					return StringValueType(stringValue);
				}


				//////////////////////////////////////////////////////////////////////////////// Evaluating

				namespace _Internal
				{
					//------------------------------------------------------------------------------
					template <typename ValueT>
					ValueT _DeferenceCastedVariable(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					BooleanValueType _DeferenceCastedVariable<BooleanValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kInteger:
								{
									std::shared_ptr<IntVariable> valueVaraible(std::dynamic_pointer_cast<IntVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									IntegerValueType evaluatedValue = valueVaraible->GetValue(processingContext);

									// False for 0, True for otherwise
									return !(evaluatedValue == 0);
								}

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									std::tuple<BooleanValueType, bool> resultWithValidity = _Internal::_ParseBooleanValueFromStringWithValidity(evaluatedValue.c_str());
									if (!std::get<1>(resultWithValidity))
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									return std::get<0>(resultWithValidity);
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					IntegerValueType _DeferenceCastedVariable<IntegerValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kFloatingPoint:
								{
									std::shared_ptr<FloatVariable> valueVaraible(std::dynamic_pointer_cast<FloatVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									FloatingPointValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return static_cast<IntegerValueType>(evaluatedValue);
								}

							case UIDescriptionValueType::kColor:
								{
									std::shared_ptr<ColorVariable> valueVaraible(std::dynamic_pointer_cast<ColorVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									ColorARGBValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return static_cast<IntegerValueType>(evaluatedValue.ToColorCodeARGB());
								}

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									if (evaluatedValue.length() < 1)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									return static_cast<IntegerValueType>(strtol(evaluatedValue.c_str(), nullptr, 0));
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					FloatingPointValueType _DeferenceCastedVariable<FloatingPointValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kInteger:
								{
									std::shared_ptr<IntVariable> valueVaraible(std::dynamic_pointer_cast<IntVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									IntegerValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return static_cast<FloatingPointValueType>(evaluatedValue);
								}

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									if (evaluatedValue.length() < 1)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									return static_cast<FloatingPointValueType>(strtof(evaluatedValue.c_str(), nullptr));
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					ColorARGBValueType _DeferenceCastedVariable<ColorARGBValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kInteger:
								{
									std::shared_ptr<IntVariable> valueVaraible(std::dynamic_pointer_cast<IntVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									IntegerValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return ColorARGBValueType::FromColorCodeARGB(static_cast<ColorARGBValueType::ColorCodeType>(evaluatedValue));
								}

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return ParseColorValueFromString(evaluatedValue.c_str());
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					Vector2DValueType _DeferenceCastedVariable<Vector2DValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return ParseVector2DValueFromString(evaluatedValue.c_str());
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					Vector3DValueType _DeferenceCastedVariable<Vector3DValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kVector2D:
								{
									std::shared_ptr<Vector2DVariable> valueVaraible(std::dynamic_pointer_cast<Vector2DVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									Vector2DValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return Vector3DValueType(evaluatedValue);
								}

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<StringVariable> valueVaraible(std::dynamic_pointer_cast<StringVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									StringValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return ParseVector3DValueFromString(evaluatedValue.c_str());
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					template <>
					StringValueType _DeferenceCastedVariable<StringValueType>(DescriptionProcessingContext &processingContext, const std::shared_ptr<IDescriptionObject> &targetObject)
					{
						std::shared_ptr<ITypeTagged> typeTagged(std::dynamic_pointer_cast<ITypeTagged>(targetObject));
						if (!typeTagged)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						switch (typeTagged->GetValueType())
						{
							case UIDescriptionValueType::kBoolean:
								{
									std::shared_ptr<BoolVariable> valueVaraible(std::dynamic_pointer_cast<BoolVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									BooleanValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									if (evaluatedValue)
									{
										return "True";
									}
									else
									{
										return "False";
									}
								}

							case UIDescriptionValueType::kInteger:
								{
									std::shared_ptr<IntVariable> valueVaraible(std::dynamic_pointer_cast<IntVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									IntegerValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return fmt::format("{0}", evaluatedValue);
								}

							case UIDescriptionValueType::kFloatingPoint:
								{
									std::shared_ptr<FloatVariable> valueVaraible(std::dynamic_pointer_cast<FloatVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									FloatingPointValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return fmt::format("{0}", evaluatedValue);
								}

							case UIDescriptionValueType::kColor:
								{
									std::shared_ptr<ColorVariable> valueVaraible(std::dynamic_pointer_cast<ColorVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									ColorARGBValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return fmt::format("#{0:02x}{1:02x}{2:02x}{3:02x}", evaluatedValue.alpha, evaluatedValue.red, evaluatedValue.green, evaluatedValue.blue);
								}

							case UIDescriptionValueType::kVector2D:
								{
									std::shared_ptr<Vector2DVariable> valueVaraible(std::dynamic_pointer_cast<Vector2DVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									Vector2DValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return fmt::format("vec2({0}, {1})", evaluatedValue.x, evaluatedValue.y);
								}

							case UIDescriptionValueType::kVector3D:
								{
									std::shared_ptr<Vector3DVariable> valueVaraible(std::dynamic_pointer_cast<Vector3DVariable>(targetObject));
									if (!valueVaraible)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}

									Vector3DValueType evaluatedValue = valueVaraible->GetValue(processingContext);
									return fmt::format("vec3({0}, {1}, {2})", evaluatedValue.x, evaluatedValue.y, evaluatedValue.z);
								}
						}

						throw new UIDescriptionExpressionTypeErrorException();
					}

					//------------------------------------------------------------------------------
					template <typename ValueT>
					ValueT GetDereferencedValue(DescriptionProcessingContext &processingContext, const VariablePathType &path)
					{
						ObjectEnvironmentPair foundResult(processingContext.Find(path));
						std::shared_ptr<IDescriptionObject> foundObject(std::get<0>(foundResult));
						if (!foundObject)
						{
							throw new UIDescriptionExpressionVariableNotExistException();
						}

						std::shared_ptr<Variable<ValueT> > valueVaraible(std::dynamic_pointer_cast<Variable<ValueT>>(foundObject));
						if (!valueVaraible)
						{
							return _DeferenceCastedVariable<ValueT>(std::get<1>(foundResult), foundObject);
						}

						return valueVaraible->GetValue(std::get<1>(foundResult));
					}

					//------------------------------------------------------------------------------
					// Casting
					template <typename FromT, typename ToT>
					ToT CastValue(FromT valueToCast)
					{
						throw new UIDescriptionExpressionTypeErrorException();
					}

					// Value Casting Implementation is in a separated file
#include "UIDescriptionExpression_CastValue.inl"

					//------------------------------------------------------------------------------
					std::shared_ptr<IArbitrarilyTypedExpression> CastExpression(std::shared_ptr<IArbitrarilyTypedExpression> originalExpression, UIDescriptionValueType toType)
					{
						if (!originalExpression)
						{
							return nullptr;
						}

						switch (originalExpression->GetValueType())
						{
							case UIDescriptionValueType::kBoolean:
								{
									std::shared_ptr<IExpression<BooleanValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<BooleanValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return originalTypedExpression;
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<BooleanValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<BooleanValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<BooleanValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<BooleanValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<BooleanValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return std::make_shared<Cast<BooleanValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kInteger:
								{
									std::shared_ptr<IExpression<IntegerValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<IntegerValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<IntegerValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return originalTypedExpression;
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<IntegerValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<IntegerValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<IntegerValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<IntegerValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return std::make_shared<Cast<IntegerValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kFloatingPoint:
								{
									std::shared_ptr<IExpression<FloatingPointValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<FloatingPointValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<FloatingPointValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<FloatingPointValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return originalTypedExpression;
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<FloatingPointValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<FloatingPointValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<FloatingPointValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return std::make_shared<Cast<FloatingPointValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kColor:
								{
									std::shared_ptr<IExpression<ColorARGBValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<ColorARGBValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<ColorARGBValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<ColorARGBValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<ColorARGBValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return originalTypedExpression;
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<ColorARGBValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<ColorARGBValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return std::make_shared<Cast<ColorARGBValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kVector2D:
								{
									std::shared_ptr<IExpression<Vector2DValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<Vector2DValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<Vector2DValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<Vector2DValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<Vector2DValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<Vector2DValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return originalTypedExpression;
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<Vector2DValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return std::make_shared<Cast<Vector2DValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kVector3D:
								{
									std::shared_ptr<IExpression<Vector3DValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<Vector3DValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<Vector3DValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<Vector3DValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<Vector3DValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<Vector3DValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<Vector3DValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return originalTypedExpression;
										case UIDescriptionValueType::kString: return std::make_shared<Cast<Vector3DValueType, StringValueType> >(originalTypedExpression);
									}
								}
								break;

							case UIDescriptionValueType::kString:
								{
									std::shared_ptr<IExpression<StringValueType> > originalTypedExpression(std::dynamic_pointer_cast<IExpression<StringValueType>>(originalExpression));
									if (!originalTypedExpression)
									{
										throw new UIDescriptionExpressionTypeErrorException();
									}
									switch (toType)
									{
										case UIDescriptionValueType::kBoolean: return std::make_shared<Cast<StringValueType, BooleanValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kInteger: return std::make_shared<Cast<StringValueType, IntegerValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kFloatingPoint: return std::make_shared<Cast<StringValueType, FloatingPointValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kColor: return std::make_shared<Cast<StringValueType, ColorARGBValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector2D: return std::make_shared<Cast<StringValueType, Vector2DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kVector3D: return std::make_shared<Cast<StringValueType, Vector3DValueType> >(originalTypedExpression);
										case UIDescriptionValueType::kString: return originalTypedExpression;
									}
								}
								break;
						}

						return nullptr;
					}

					//------------------------------------------------------------------------------
					template <typename ValueT>
					std::shared_ptr<IExpression<ValueT> > CastToTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> originalExpression)
					{
						if (!originalExpression)
						{
							return nullptr;
						}

						std::shared_ptr<IArbitrarilyTypedExpression> castedAuxTypedExpression(CastExpression(originalExpression, GetValueTypeTag<ValueT>()));
						if (!castedAuxTypedExpression)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						std::shared_ptr<IExpression<ValueT> > castedExpression(std::dynamic_pointer_cast<IExpression<ValueT>>(castedAuxTypedExpression));
						if (!castedExpression)
						{
							throw new UIDescriptionExpressionTypeErrorException();
						}

						return castedExpression;
					}
				}


				//////////////////////////////////////////////////////////////////////////////// IExpression

				//------------------------------------------------------------------------------
				template<>
				std::shared_ptr<IExpression<BooleanValueType> > IExpression<BooleanValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<BooleanValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<IntegerValueType> > IExpression<IntegerValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<IntegerValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<FloatingPointValueType> > IExpression<FloatingPointValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<FloatingPointValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<ColorARGBValueType> > IExpression<ColorARGBValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<ColorARGBValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<Vector2DValueType> > IExpression<Vector2DValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<Vector2DValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<Vector3DValueType> > IExpression<Vector3DValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<Vector3DValueType>(arbitrarilyTypedExpression);
				}

				template<>
				std::shared_ptr<IExpression<StringValueType> > IExpression<StringValueType>::GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression)
				{
					return _Internal::CastToTypedExpression<StringValueType>(arbitrarilyTypedExpression);
				}
			}
		}
	}
}

// Evaluation Implementation in a separated file
#include "UIDescriptionExpression_NodesEvaluation.inl"

// Reader Implementation in a separated file
#include "UIDescriptionExpression_Reader.inl"

// Value Getter Implementation using Expression Casting in a separated file
#include "UIDescriptionVariable_ValueGetter.inl"
