////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionSourcedString.h (Leggiero/Modules - LegacyUI)
//
// Sourced generated string of UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_SOURCED_STRING_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_SOURCED_STRING_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Forward Declarations
			class DescriptionProcessingContext;

			namespace Expression
			{
				template <typename ValueT>
				class IExpression;
			}

			namespace Prefab
			{
				class IUIPrefabPlaceProcessor;
			}


			// Source type of string
			enum class StringSourceType
			{
				kTextInBag,
				kRuntimeGenerating,
				kVariable,
				kInline,
				kRawString,
			};


			// Sourced String Data
			struct SourcedStringEntry
			{
			public:
				using StringGeneratingKeyType = std::string;

			public:
				StringSourceType											stringSourceType;

				DataStringKeyType											stringKey;
				StringGeneratingKeyType										generateKey;
				VariablePathType											stringVariablePath;
				std::shared_ptr<Expression::IExpression<StringValueType> >	inlineStringExpression;
				GameDataString												rawString;

			public:
				bool ReadFromFromXMLElement(ParsingXMLElementType *elem, const std::string &prefix = "");
				GameDataString GetStringValue(DescriptionProcessingContext &processingContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);

			public:
				SourcedStringEntry()
					: stringSourceType(StringSourceType::kRawString)
				{ }
			};
		}
	}
}

#endif
