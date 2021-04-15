////////////////////////////////////////////////////////////////////////////////
// _Internal/_CommonTypes.h (Leggiero - Basic)
//   * DO NOT directly include this header file outside of Basic project
//
// Definitions for common basic types
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASIC___COMMON_TYPES_H
#define __BASIC___COMMON_TYPES_H

// Standard Library
#include <cstdint>
#include <chrono>
#include <limits>
#include <string>

// External Library
#include <utfcpp/utf8.h>


namespace Leggiero
{
	// Standard clock to represent game time
	using GameTimeClockType = std::chrono::steady_clock;

	// Game data string type(UTF-8)
	using GameDataString = std::string;
	
	extern const GameDataString g_EmptyGameDataString;

	// Game Frame Number
	using GameFrameNumberType = int64_t;
	constexpr GameFrameNumberType kNoFrame = static_cast<GameFrameNumberType>(0);
}

#endif
