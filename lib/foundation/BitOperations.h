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

/**
 * Scan the given 32 bit value for LSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsf (uint32_t v)
{

    __asm__ __volatile__ (
    "           bsf     %0, %0      \n\t"
    : "+r" (v)
    :
    : "cc"
    );

    return v;

}



/**
 * Scan the given 64 bit value for LSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsf (uint64_t v)
{

    __asm__ __volatile__ (
    "           bsf     %0, %0      \n\t"
    : "+r" (v)
    :
    : "cc"
    );

    return static_cast<uint32_t>(v);

}



/**
 * Scan the given 64 bit value for LSB and reset that bit
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsfr (uint64_t &v)
{

    uint64_t    result;

    __asm__ __volatile__ (
    "           bsf     %0, %1      \n\t"
    "           btr     %1, %0      \n\t"
    : "+r" (v), "=r" (result)
    :
    : "cc"
    );

    return static_cast<uint32_t>(result);

}



/**
 * Scan the given 32 bit value for MSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsr (uint32_t v)
{

    __asm__ __volatile__ (
    "           bsr     %0, %0      \n\t"
    : "+r" (v)
    :
    : "cc"
    );

    return v;

}



/**
 * Scan the given 64 bit value for MSB.
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsr (uint64_t v)
{

    __asm__ __volatile__ (
    "           bsr     %0, %0      \n\t"
    : "+r" (v)
    :
    : "cc"
    );

    return static_cast<uint32_t>(v);

}



/**
 * Scan the given 64 bit value for MSB and reset that bit
 * @param  v value to be scanned.
 * @return bit index.
 */
inline uint32_t bsrr (uint64_t &v)
{

    uint64_t    result;

    __asm__ __volatile__ (
    "           bsr     %0, %1      \n\t"
    "           btr     %1, %0      \n\t"
    : "+r" (v), "=r" (result)
    :
    : "cc"
    );

    return static_cast<uint32_t>(result);

}



/**
 * POPCNT - count bits set in 128 bits data
 * @param  p1 
 * @param  p2 
 * @return number of 1-bits in p1 and p2
 */
inline uint32_t popcnt (uint64_t p1, uint64_t p2)
{

    uint64_t                result, count;

    __asm__ __volatile__ (
    "           popcnt  %2, %0      \n\t"
    "           popcnt  %3, %1      \n\t"
    "           add     %1, %0      \n\t"
    : "=&r" (result), "=&r" (count)
    : "r" (p1), "r" (p2)
    : "cc"
    );

    return static_cast<uint32_t>(result);

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

    uint64_t    result, shift, index;

    __asm__ __volatile__ (
    "           xor     %0, %0      \n\t" // result = 0
    "           xor     %1, %1      \n\t" // index  = 0
    "1:         and     %4, %4      \n\t" // loop:
    "           je      3f          \n\t" // if (mask == 0) goto quit
    "           bsf     %4, %2      \n\t" // shift = index of LSB of mask
    "           bt      %2, %3      \n\t" // if (bit is set) {
    "           jnc     2f          \n\t" //   set bit of result at index
    "           bts     %1, %0      \n\t" // }
    "2:         btr     %2, %4      \n\t" // reset LSB of mask 
    "           add     $1, %1      \n\t" // index++
    "           jmp     1b          \n\t" // goto loop
    "3:                             \n\t" // quit:
    : "=&r" (result), "=&r" (index), "=&r" (shift)
    : "r" (v), "r" (mask)
    : "cc"
    );

    return result;

}
#else
inline uint64_t pext (uint64_t v, uint64_t mask)
{

    uint64_t                result;

    __asm__ __volatile__ (
    "           pext    %2, %1, %0  \n\t"
    : "=&r" (result)
    : "r" (v), "r" (mask)
    : "cc"
    );

    return result;

}
#endif

/* ------------------------------------------------------------------------- */

// end namespace 'foundation::Bitope'
} }

#endif
