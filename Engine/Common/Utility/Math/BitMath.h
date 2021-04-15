////////////////////////////////////////////////////////////////////////////////
// Math/BitMath.h (Leggiero - Utility)
//
// Bit Manipulation Mathmatical Utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__BIT_MATH_H
#define __UTILITY__MATH__BIT_MATH_H


// Standard Library
#include <cmath>
#include <cstdint>

// External Library
#if _MSC_VER && !__INTEL_COMPILER
#include <intrin.h>
#endif


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
            template <typename ValueT>
            inline size_t PopCount(ValueT x)
            {
                // although there are some tricky algorithms like MIT HACKAM and etc.., lookup is the fastest(except CPU instruction).
                // https://gurmeet.net/puzzles/fast-bit-counting-routines/

                static const size_t kNibbleBitCount[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

                size_t byteSize = sizeof(x);
                size_t bitCount = 0;
                for (size_t i = 0; i < byteSize; ++i)
                {
                    bitCount += kNibbleBitCount[(x >> (i * 8)) & 15];
                    bitCount += kNibbleBitCount[(x >> (i * 8 + 4)) & 15];
                }
                return bitCount;
            }

            template <>
            inline size_t PopCount(uint8_t x)
            {
                switch (x)
                {
                    case 0: return 0;
                    case 1: return 1;
                    case 2: return 1;
                    case 3: return 2;
                    case 4: return 1;
                    case 5: return 2;
                    case 6: return 2;
                    case 7: return 3;
                    case 8: return 1;
                    case 9: return 2;
                    case 10: return 2;
                    case 11: return 3;
                    case 12: return 2;
                    case 13: return 3;
                    case 14: return 3;
                    case 15: return 4;
                    case 16: return 1;
                    case 17: return 2;
                    case 18: return 2;
                    case 19: return 3;
                    case 20: return 2;
                    case 21: return 3;
                    case 22: return 3;
                    case 23: return 4;
                    case 24: return 2;
                    case 25: return 3;
                    case 26: return 3;
                    case 27: return 4;
                    case 28: return 3;
                    case 29: return 4;
                    case 30: return 4;
                    case 31: return 5;
                    case 32: return 1;
                    case 33: return 2;
                    case 34: return 2;
                    case 35: return 3;
                    case 36: return 2;
                    case 37: return 3;
                    case 38: return 3;
                    case 39: return 4;
                    case 40: return 2;
                    case 41: return 3;
                    case 42: return 3;
                    case 43: return 4;
                    case 44: return 3;
                    case 45: return 4;
                    case 46: return 4;
                    case 47: return 5;
                    case 48: return 2;
                    case 49: return 3;
                    case 50: return 3;
                    case 51: return 4;
                    case 52: return 3;
                    case 53: return 4;
                    case 54: return 4;
                    case 55: return 5;
                    case 56: return 3;
                    case 57: return 4;
                    case 58: return 4;
                    case 59: return 5;
                    case 60: return 4;
                    case 61: return 5;
                    case 62: return 5;
                    case 63: return 6;
                    case 64: return 1;
                    case 65: return 2;
                    case 66: return 2;
                    case 67: return 3;
                    case 68: return 2;
                    case 69: return 3;
                    case 70: return 3;
                    case 71: return 4;
                    case 72: return 2;
                    case 73: return 3;
                    case 74: return 3;
                    case 75: return 4;
                    case 76: return 3;
                    case 77: return 4;
                    case 78: return 4;
                    case 79: return 5;
                    case 80: return 2;
                    case 81: return 3;
                    case 82: return 3;
                    case 83: return 4;
                    case 84: return 3;
                    case 85: return 4;
                    case 86: return 4;
                    case 87: return 5;
                    case 88: return 3;
                    case 89: return 4;
                    case 90: return 4;
                    case 91: return 5;
                    case 92: return 4;
                    case 93: return 5;
                    case 94: return 5;
                    case 95: return 6;
                    case 96: return 2;
                    case 97: return 3;
                    case 98: return 3;
                    case 99: return 4;
                    case 100: return 3;
                    case 101: return 4;
                    case 102: return 4;
                    case 103: return 5;
                    case 104: return 3;
                    case 105: return 4;
                    case 106: return 4;
                    case 107: return 5;
                    case 108: return 4;
                    case 109: return 5;
                    case 110: return 5;
                    case 111: return 6;
                    case 112: return 3;
                    case 113: return 4;
                    case 114: return 4;
                    case 115: return 5;
                    case 116: return 4;
                    case 117: return 5;
                    case 118: return 5;
                    case 119: return 6;
                    case 120: return 4;
                    case 121: return 5;
                    case 122: return 5;
                    case 123: return 6;
                    case 124: return 5;
                    case 125: return 6;
                    case 126: return 6;
                    case 127: return 7;
                    case 128: return 1;
                    case 129: return 2;
                    case 130: return 2;
                    case 131: return 3;
                    case 132: return 2;
                    case 133: return 3;
                    case 134: return 3;
                    case 135: return 4;
                    case 136: return 2;
                    case 137: return 3;
                    case 138: return 3;
                    case 139: return 4;
                    case 140: return 3;
                    case 141: return 4;
                    case 142: return 4;
                    case 143: return 5;
                    case 144: return 2;
                    case 145: return 3;
                    case 146: return 3;
                    case 147: return 4;
                    case 148: return 3;
                    case 149: return 4;
                    case 150: return 4;
                    case 151: return 5;
                    case 152: return 3;
                    case 153: return 4;
                    case 154: return 4;
                    case 155: return 5;
                    case 156: return 4;
                    case 157: return 5;
                    case 158: return 5;
                    case 159: return 6;
                    case 160: return 2;
                    case 161: return 3;
                    case 162: return 3;
                    case 163: return 4;
                    case 164: return 3;
                    case 165: return 4;
                    case 166: return 4;
                    case 167: return 5;
                    case 168: return 3;
                    case 169: return 4;
                    case 170: return 4;
                    case 171: return 5;
                    case 172: return 4;
                    case 173: return 5;
                    case 174: return 5;
                    case 175: return 6;
                    case 176: return 3;
                    case 177: return 4;
                    case 178: return 4;
                    case 179: return 5;
                    case 180: return 4;
                    case 181: return 5;
                    case 182: return 5;
                    case 183: return 6;
                    case 184: return 4;
                    case 185: return 5;
                    case 186: return 5;
                    case 187: return 6;
                    case 188: return 5;
                    case 189: return 6;
                    case 190: return 6;
                    case 191: return 7;
                    case 192: return 2;
                    case 193: return 3;
                    case 194: return 3;
                    case 195: return 4;
                    case 196: return 3;
                    case 197: return 4;
                    case 198: return 4;
                    case 199: return 5;
                    case 200: return 3;
                    case 201: return 4;
                    case 202: return 4;
                    case 203: return 5;
                    case 204: return 4;
                    case 205: return 5;
                    case 206: return 5;
                    case 207: return 6;
                    case 208: return 3;
                    case 209: return 4;
                    case 210: return 4;
                    case 211: return 5;
                    case 212: return 4;
                    case 213: return 5;
                    case 214: return 5;
                    case 215: return 6;
                    case 216: return 4;
                    case 217: return 5;
                    case 218: return 5;
                    case 219: return 6;
                    case 220: return 5;
                    case 221: return 6;
                    case 222: return 6;
                    case 223: return 7;
                    case 224: return 3;
                    case 225: return 4;
                    case 226: return 4;
                    case 227: return 5;
                    case 228: return 4;
                    case 229: return 5;
                    case 230: return 5;
                    case 231: return 6;
                    case 232: return 4;
                    case 233: return 5;
                    case 234: return 5;
                    case 235: return 6;
                    case 236: return 5;
                    case 237: return 6;
                    case 238: return 6;
                    case 239: return 7;
                    case 240: return 4;
                    case 241: return 5;
                    case 242: return 5;
                    case 243: return 6;
                    case 244: return 5;
                    case 245: return 6;
                    case 246: return 6;
                    case 247: return 7;
                    case 248: return 5;
                    case 249: return 6;
                    case 250: return 6;
                    case 251: return 7;
                    case 252: return 6;
                    case 253: return 7;
                    case 254: return 7;
                    default: return 8;
                }
            }

            template <>
            inline size_t PopCount(uint16_t x)
            {
                #if _MSC_VER && !__INTEL_COMPILER
                    return __popcnt16(x);
                #else
                    return PopCount<uint8_t>(static_cast<uint8_t>(x & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 8) & 0xFF));
                #endif
            }

            template <>
            inline size_t PopCount(uint32_t x)
            {
                #if _MSC_VER && !__INTEL_COMPILER
                    return __popcnt(x);
                #else
                    // Can we use __builtin_popcount ?
                    return PopCount<uint8_t>(static_cast<uint8_t>(x & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 8) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 16) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 24) & 0xFF));
                #endif
            }

            template <>
            inline size_t PopCount(uint64_t x)
            {
                #if _MSC_VER && !__INTEL_COMPILER
                    return static_cast<size_t>(__popcnt64(x));
                #else
                    // Can we use __builtin_popcountll ?
                    return PopCount<uint8_t>(static_cast<uint8_t>(x & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 8) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 16) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 24) & 0xFF))
                        + PopCount<uint8_t>(static_cast<uint8_t>((x >> 32) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 40) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 48) & 0xFF)) + PopCount<uint8_t>(static_cast<uint8_t>((x >> 56) & 0xFF));
                #endif
            }

            template <>
            inline size_t PopCount(int8_t x)
            {
                union
                {
                    int8_t source;
                    uint8_t target;
                } a = { x };
                return PopCount<uint8_t>(a.target);
            }

            template <>
            inline size_t PopCount(int16_t x)
            {
                union
                {
                    int16_t source;
                    uint16_t target;
                } a = { x };
                return PopCount<uint16_t>(a.target);
            }

            template <>
            inline size_t PopCount(int32_t x)
            {
                union
                {
                    int32_t source;
                    uint32_t target;
                } a = { x };
                return PopCount<uint32_t>(a.target);
            }

            template <>
            inline size_t PopCount(int64_t x)
            {
                union
                {
                    int64_t source;
                    uint64_t target;
                } a = { x };
                return PopCount<uint64_t>(a.target);
            }
		}
	}
}

#endif
