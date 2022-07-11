////////////////////////////////////////////////////////////////////////////////
// _Internal/_Warning.h (Leggiero - Basic)
//   * DO NOT directly include this header file outside of Basic project
//
// C++ language setting for compiler warnings
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASIC___WARNINGS_H
#define __BASIC___WARNINGS_H

// Ignore VS C++ Warnings
#ifdef _MSC_VER
#pragma warning( disable : 4290 )	// C++ Exception
#pragma warning( disable : 26439 )	// Exception Spec
#pragma warning( disable : 26451 )	// Casting to Wider Range First
#pragma warning( disable : 26495 )	// Initialization
#pragma warning( disable : 26812 )	// Enum Scope
#endif

// Ignore C/C++ RT Secure Stuffs
#define _CRT_SECURE_NO_WARNINGS

// Ignore Clang/GCC Warnings
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Winstantiation-after-specialization"
#endif

#endif
