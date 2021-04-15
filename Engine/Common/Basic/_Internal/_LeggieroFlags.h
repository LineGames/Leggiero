////////////////////////////////////////////////////////////////////////////////
// _Internal/_LeggieroFlags.h (Leggiero - Basic)
//   * DO NOT directly include this header file outside of Basic project
//
// Leggiero Engine Preprocessor Definition Stuffs
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASIC___LEGGIERO_FLAGS_H
#define __BASIC___LEGGIERO_FLAGS_H


// Detect Platform
#if defined(_WIN32) || defined(__WIN32__)
	#define _LEGGIERO_WINPC
#elif defined(__APPLE__)
	#define _LEGGIERO_IOS
#elif defined(ANDROID) || defined(__ANDROID__)
	#define _LEGGIERO_ANDROID
#else
	// Unknown Platform
#endif


// Detect Debug Mode
#ifdef NDEBUG
	#define _LEGGIERO_RELEASE 1
#else
	#define _LEGGIERO_DEBUG 1
#endif


#endif
