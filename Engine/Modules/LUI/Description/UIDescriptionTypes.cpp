////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionTypes.cpp (Leggiero/Modules - LegacyUI)
//
// Common implementation for UI Description types
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionTypes.h"

// Standard Library
#include <cstring>
#include <string>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace TypeUtility
			{
				//------------------------------------------------------------------------------
				const VariableNameType kInvalidName = ToVariableName("");

				//------------------------------------------------------------------------------
				VariablePathType ParseVariablePath(const char *variablePath)
				{
					VariablePathType resultPath;

					size_t rawPathLength = strlen(variablePath);
					if (rawPathLength == 0)
					{
						return resultPath;
					}

					const char *currentPtr = variablePath;
					size_t lastIndex = 0;
					for (size_t i = 0; i < rawPathLength; ++i)
					{
						if (variablePath[i] == StringFormat::kVariablePathDelimiter)
						{
							resultPath.emplace_back(currentPtr, i - lastIndex);
							currentPtr = variablePath + i + 1;
							lastIndex = i + 1;
						}
					}
					resultPath.emplace_back(currentPtr);

					return resultPath;
				}

				//------------------------------------------------------------------------------
				VariablePathType ParseVariablePath(const std::string &variablePath)
				{
					VariablePathType resultPath;

					size_t lastStartIndex = 0;
					for (size_t i = 0; i < variablePath.length(); ++i)
					{
						if (variablePath[i] == StringFormat::kVariablePathDelimiter)
						{
							resultPath.emplace_back(variablePath, lastStartIndex, i - lastStartIndex);
							lastStartIndex = i + 1;
						}
					}
					resultPath.emplace_back(variablePath, lastStartIndex);

					return resultPath;
				}
			}

			//------------------------------------------------------------------------------
			// Data String Bag

			namespace _Internal
			{
				static GameDataString kEmptyStringValue(
					#ifdef _LEGGIERO_DEBUG
						"<NO STRING>"
					#else
						""
					#endif
				);

				class EmptyStringBag
					: public Utility::String::IStringBag<DataStringKeyType, DataStringValueType>
				{
				public:	// IStringBag
					virtual DataStringValueType GetString(const DataStringKeyType &key) { return kEmptyStringValue; }

					virtual DataStringValueType GetStringByKeyString(const std::string &keyString) { return kEmptyStringValue; }
					virtual DataStringValueType GetStringByKeyString(const char *keyString, size_t keyLength) { return kEmptyStringValue; }

					virtual DataStringKeyType ToKey(const std::string &keyString) { return DataStringKeyType(keyString); }
					virtual DataStringKeyType ToKey(const char *keyString, size_t keyLength) { return DataStringKeyType(keyString, keyLength); }
				};

				static EmptyStringBag kEmptyStringBag;
			}

			DataStringBagType &GetEmptyStringBag()
			{
				return _Internal::kEmptyStringBag;
			}

			//------------------------------------------------------------------------------
			QuantizeType ParseQuantizeType(const char *quantizationCString)
			{
				if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(quantizationCString, "floor"))
				{
					return QuantizeType::kFloor;
				}
				else if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(quantizationCString, "round"))
				{
					return QuantizeType::kRound;
				}
				else if (Utility::String::ASCIIStringUtility::IsStartWithNonCaseSensitive(quantizationCString, "ceil"))
				{
					return QuantizeType::kCeil;
				}

				return QuantizeType::kNo;
			}
		}
	}
}
