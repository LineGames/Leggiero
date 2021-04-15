////////////////////////////////////////////////////////////////////////////////
// Sugar/EnumClass.h (Leggiero - Utility)
//
// Enum Class Related Syntactic Sugars
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__ENUM_CLASS_H
#define __UTILITY__SUGAR__ENUM_CLASS_H


// Standard Library
#include <cstddef>
#include <type_traits>


namespace Leggiero
{
	namespace Utility
	{
		namespace SyntacticSugar
		{
			// Hash Functor for Enum Class
			struct EnumClassHash
			{
				template <typename T>
				std::size_t operator()(T t) const
				{
					return static_cast<std::size_t>(t);
				}
			};

			// Flag Enum
			template <typename FlagEnumT>
			inline bool HasFlag(FlagEnumT v, FlagEnumT checkingFlag) { return ((v & checkingFlag) == checkingFlag); }
		}
	}
}

#define MAKE_ENUM_FLAG(ENUM_TYPE) \
inline ENUM_TYPE operator~ (ENUM_TYPE a) { return (ENUM_TYPE)~static_cast<std::underlying_type_t<ENUM_TYPE> >(a); } \
inline ENUM_TYPE operator| (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) | static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); } \
inline ENUM_TYPE operator& (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) & static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); } \
inline ENUM_TYPE operator^ (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) ^ static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); } \
inline ENUM_TYPE &operator|= (ENUM_TYPE &a, ENUM_TYPE b) { a = (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) | static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); return a; } \
inline ENUM_TYPE &operator&= (ENUM_TYPE &a, ENUM_TYPE b) { a = (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) & static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); return a; } \
inline ENUM_TYPE &operator^= (ENUM_TYPE &a, ENUM_TYPE b) { a = (ENUM_TYPE)(static_cast<std::underlying_type_t<ENUM_TYPE> >(a) ^ static_cast<std::underlying_type_t<ENUM_TYPE> >(b)); return a; }

#endif
