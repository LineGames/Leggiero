////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionTypes.h (Leggiero/Modules - LegacyUI)
//
// Common type definitions for UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_TYPES_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_TYPES_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>
#include <vector>

// Leggiero.Utility
#include <Utility/Math/Vector.h>
#include <Utility/String/IStringBag.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


// Forward Declaration
namespace tinyxml2
{
	class XMLElement;
}


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Variable name for referencing
			using VariableNameType = std::string;
			using VariablePathType = std::vector<VariableNameType>;

			using UnitNameType = std::string;
			using SavedUnitNameType = std::string;

			namespace StringFormat
			{
				constexpr char kVariablePathDelimiter = ':';
				constexpr char kExpressionTypeDelimiter = ';';
			}

			namespace TypeUtility
			{
				inline VariableNameType ToVariableName(const char *variableName)
				{
					return VariableNameType(variableName);
				}

				inline VariableNameType ToVariableName(const std::string &variableName)
				{
					return VariableNameType(variableName);
				}

				inline SavedUnitNameType ToSavedUnitName(const UnitNameType &unitName)
				{
					return SavedUnitNameType(unitName);
				}

				VariablePathType ParseVariablePath(const char *variablePath);
				VariablePathType ParseVariablePath(const std::string &variablePath);

				extern const VariableNameType kInvalidName;
			}


			// Type tag for description object
			enum class UIDescriptionObjectType
			{
				kINVALID,

				kImportedUnit,
				kNameSpaceGroup,
				kOverridingNameSpace,

				kAliasName,
				kAliasPath,

				kUIPrefab,

				kVariable,

				kTextureSegment,
				kTextureSource,

				kFontClass,

				kThreePatchHClass,
				kThreePatchVClass,
				kNinePatchClass,
			};


			// Type tag for values in description
			enum class UIDescriptionValueType
			{
				kINVALID,

				kBoolean,

				kInteger,
				kFloatingPoint,

				kColor,

				kVector2D,
				kVector3D,

				kString,
			};

			using BooleanValueType = bool;
			using IntegerValueType = int;
			using FloatingPointValueType = float;
			using ColorARGBValueType = Graphics::GLByteARGB;
			using Vector2DValueType = Utility::Math::Vector2D<FloatingPointValueType>;
			using Vector3DValueType = Utility::Math::Vector3D<FloatingPointValueType>;
			using StringValueType = GameDataString;

			template <typename ValueT>
			inline constexpr UIDescriptionValueType GetValueTypeTag()
			{
				return UIDescriptionValueType::kINVALID;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<BooleanValueType>()
			{
				return UIDescriptionValueType::kBoolean;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<IntegerValueType>()
			{
				return UIDescriptionValueType::kInteger;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<FloatingPointValueType>()
			{
				return UIDescriptionValueType::kFloatingPoint;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<ColorARGBValueType>()
			{
				return UIDescriptionValueType::kColor;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<Vector2DValueType>()
			{
				return UIDescriptionValueType::kVector2D;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<Vector3DValueType>()
			{
				return UIDescriptionValueType::kVector3D;
			}

			template<>
			inline constexpr UIDescriptionValueType GetValueTypeTag<StringValueType>()
			{
				return UIDescriptionValueType::kString;
			}


			// Data String Bag
			using DataStringKeyType = std::string;
			using DataStringValueType = GameDataString;
			using DataStringBagType = Utility::String::IStringBag<DataStringKeyType, DataStringValueType>;

			DataStringBagType &GetEmptyStringBag();


			// Other Setting Types

			enum class QuantizeType
			{
				kNo,
				kFloor,
				kRound,
				kCeil,
			};

			QuantizeType ParseQuantizeType(const char *quantizationCString);


			// Parsing from XML
			using ParsingXMLElementType = tinyxml2::XMLElement;
		}
	}
}

#endif
