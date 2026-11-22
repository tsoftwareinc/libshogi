/**
 *****************************************************************************

 @file       BitOperations.h

 @brief      Wrapper functions for processor specific bitoperation.
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2016-03-31

 @version    $Id:$


  Copyright 2014, 2015, 2016, 2017 Hiroki Takada

  This file is part of libshogi.

  libshogi is free software: you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  libshogi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with libshogi. If not, see <http://www.gnu.org/licenses/>.


  ----------------------------------------------------------------------------
  RELEASE NOTE :

   DATE          REV    REMARK
  ============= ====== =======================================================
  31st Mar 2016  0.1    Initial release
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_BITOPE_H
#define _FOUNDATION_BITOPE_H

#include <stdint.h>
#if defined(USE_NATIVE_PEXTINSTRUCTION) && defined(__BMI2__)
#include <immintrin.h>
#endif

// begin namespace 'foundation::Bitope'
namespace foundation { namespace Bitope {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_BITOPE_DEBUG
#define _FOUNDATION_BITOPE_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_BITOPE_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_BITOPE_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_BITOPE_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_BITOPE_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_BITOPE_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_BITOPE_CHECK(x)
#define _FOUNDATION_BITOPE_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_BITOPE_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */

/* ------------------------------------------------------------------------- */



/* ----------------------------- bitoperations ----------------------------- */

/*
 * The scan and count functions use compiler builtins rather than inline
 * assembly. The compiler can then schedule them, reuse their results and
 * pick the best instruction (TZCNT, BLSR, POPCNT ...) for the target.
 * As before, the result of a scan is undefined when the value is zero.
 */

/**
 * Scan the given 32 bit value for LSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsf (uint32_t v)
{

    return static_cast<uint32_t>(__builtin_ctz(v));

}



/**
 * Scan the given 64 bit value for LSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsf (uint64_t v)
{

    return static_cast<uint32_t>(__builtin_ctzll(v));

}



/**
 * Scan the given 64 bit value for LSB and reset that bit
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsfr (uint64_t &v)
{

    uint32_t    result = static_cast<uint32_t>(__builtin_ctzll(v));
    v &= v - 1;

    return result;

}



/**
 * Scan the given 32 bit value for MSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsr (uint32_t v)
{

    return static_cast<uint32_t>(31 ^ __builtin_clz(v));

}



/**
 * Scan the given 64 bit value for MSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsr (uint64_t v)
{

    return static_cast<uint32_t>(63 ^ __builtin_clzll(v));

}



/**
 * Scan the given 64 bit value for MSB and reset that bit
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsrr (uint64_t &v)
{

    uint32_t    result = static_cast<uint32_t>(63 ^ __builtin_clzll(v));
    v ^= (1ULL << result);

    return result;

}



/**
 * POPCNT - count bits set in 128 bits data
 * The builtin is used when the compiler targets POPCNT (-mpopcnt or
 * -march=...). Otherwise the builtin would become a slow library call,
 * so the POPCNT instruction is issued directly as before.
 * @param  p1 
 * @param  p2 
 * @return number of 1-bits in p1 and p2
 */
inline uint32_t popcnt (uint64_t p1, uint64_t p2)
{

#ifdef __POPCNT__
    return static_cast<uint32_t>(__builtin_popcountll(p1) +
                                 __builtin_popcountll(p2));
#else
    uint64_t                result, count;

    __asm__ (
    "           popcnt  %2, %0      \n\t"
    "           popcnt  %3, %1      \n\t"
    "           add     %1, %0      \n\t"
    : "=&r" (result), "=&r" (count)
    : "r" (p1), "r" (p2)
    : "cc"
    );

    return static_cast<uint32_t>(result);
#endif

}



/**
 * PEXT - parallel bits extract for 64 bit value.
 * @param  v    a value to be examined
 * @param  mask indicates which bits to be packed
 * @return packed bits.
 */
#ifndef USE_NATIVE_PEXTINSTRUCTION
inline uint64_t pext (uint64_t v, uint64_t mask)
{

    // Plain C++ instead of inline assembly: the former assembly modified
    // its 'mask' input operand, which the compiler was told is read-only.
    uint64_t    result = 0;

    for (uint64_t bit = 1; mask != 0; bit <<= 1) {
        if (v & mask & (~mask + 1)) {   // LSB of mask is set in v
            result |= bit;
        }
        mask &= mask - 1;               // reset LSB of mask
    }

    return result;

}
#else
#ifdef __BMI2__
inline uint64_t pext (uint64_t v, uint64_t mask)
{

    return _pext_u64(v, mask);

}
#else
inline uint64_t pext (uint64_t v, uint64_t mask)
{

    uint64_t                result;

    __asm__ (
    "           pext    %2, %1, %0  \n\t"
    : "=&r" (result)
    : "r" (v), "r" (mask)
    : "cc"
    );

    return result;

}
#endif
#endif

/* ------------------------------------------------------------------------- */

// end namespace 'foundation::Bitope'
} }

#endif
