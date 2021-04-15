////////////////////////////////////////////////////////////////////////////////
// String/IStringBag.h (Leggiero - Utility)
//
// Game data string bag interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__STRING__I_STRING_BAG_H
#define __UTILITY__STRING__I_STRING_BAG_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standrad Library
#include <string>


namespace Leggiero
{
	namespace Utility
	{
		namespace String
		{
			// Base Interface for String Bag
			template <typename KeyT, typename DataT>
			class IStringBag
			{
			public:	// Types
				using KeyType = KeyT;
				using ValueType = DataT;

			public:
				virtual ~IStringBag() { }

			public:
				virtual DataT GetString(const KeyT &key) = 0;

				virtual DataT GetStringByKeyString(const std::string &keyString) { return GetString(ToKey(keyString)); }
				virtual DataT GetStringByKeyString(const char *keyString, size_t keyLength) { return GetString(ToKey(keyString, keyLength)); }

			public:
				virtual KeyT ToKey(const std::string &keyString) = 0;
				virtual KeyT ToKey(const char *keyString, size_t keyLength) = 0;
			};
		}
	}
}

#endif
