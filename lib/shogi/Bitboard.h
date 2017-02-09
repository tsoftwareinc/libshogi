/**
 *****************************************************************************

 @file       Bitboard.h

 @brief      Bitboard definitions
  
 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

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
  31st Mar 2016  0.1   Initial release
 
 *****************************************************************************/ 

#ifndef _GAME_BITBOARD_H
#define _GAME_BITBOARD_H

#include <BitOperations.h>
#include <Common.h>

#include <Square.h>
#include <iostream>

// begin namespace 'game'
namespace game {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _GAME_BITBOARD_DEBUG
#define _GAME_BITBOARD_CHECK(x) { assert ( (x) ); }
#define _GAME_BITBOARD_DEBUG_ERROR_STRING_MAX 256
#define _GAME_BITBOARD_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "GAME_BITBOARD_DEBUG     : " fmt, ## args); \
        }
#define _GAME_BITBOARD_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_GAME_BITBOARD_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _GAME_BITBOARD_CHECK(x)
#define _GAME_BITBOARD_DEBUG_OUT(fmt, args...)
#define _GAME_BITBOARD_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ---------------------------- Bitboard class ----------------------------- */

/**
 *  Square of the board : Correspondence with the bit-index.
 *
 *                    RNK1 RNK2 RNK3 RNK4 RNK5 RNK6 RNK7 RNK8 RNK9
 *                   +----+----+----+----+----+----+----+----+----+
 *  Square      FIL1 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 |
 *  bit-index        | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL2 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 |
 *                   | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL3 | 31 | 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
 *                   | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL4 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 | 49 |
 *                   | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL5 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 |
 *                   | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL6 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 |
 *                   | 45 | 46 | 47 | 48 | 49 | 50 | 51 | 52 | 53 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL7 | 71 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 |
 *                   | 54 | 55 | 56 | 57 | 58 | 59 | 60 | 61 | 62 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL8 | 81 | 82 | 83 | 84 | 85 | 86 | 87 | 88 | 89 |
 *                   | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 | 72 |
 *                   +----+----+----+----+----+----+----+----+----+
 *              FIL9 | 91 | 92 | 93 | 94 | 95 | 96 | 97 | 98 | 99 |
 *                   | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 |
 *                   +----+----+----+----+----+----+----+----+----+
 *
 * The bit 63 should not be used. This hack makes it easy to clculate Kiki
 * of Hisha - YaneuraOu
 *
 */
class Bitboard
{

public:

    // Static members
    // 'set print static-members off' when printing Bitboard instances in GDB.

    /// Filled bitboard
    static const Bitboard   Fill;

    /// Zero bitboard
    static const Bitboard   Zero;

    /// Wall surrounding the board
    static const Bitboard   Wall;

    /// Moat surrounding the board
    static const Bitboard   Moat;

    /// Square bitboard
    static const Bitboard   Square [Square::Squares];

    /// Reversed square bitboard
    static const Bitboard   Invert [Square::Squares];

    /// Bitboard with a filled row
    static const Bitboard   Rank   [Square::Ranks  ];

    /// Bitboard with a filled column
    static const Bitboard   File   [Square::Files  ];
 
    /// Tables gives a number of bits to be shifted to the
    /// file on extreme right
    static const int        VShift [Square::Squares];

    /// Debug function to print the bitboard out
    static void             debug (uint64_t p0, uint64_t p1);

    //
    //
    //

    /// Bitboard variants
    union
    {
        uint64_t            p[2];
        foundation::
        uint128_t           m;
    };

    /// Default constructor
    constexpr Bitboard ()
     :  m(0) {}

    /// Constructor takes a 128-bit value
    constexpr Bitboard (foundation::uint128_t v)
     :  m(v) {}

    /// Constructor takes two 64-bit values
    constexpr Bitboard (uint64_t p0, uint64_t p1)
     :  m(  ((foundation::uint128_t)p1 << 64)
          +  (foundation::uint128_t)p0        ) {}

    /// Copy constructor
    constexpr Bitboard (const Bitboard &b)
     :  m(b.m) {}

    /// Not
    constexpr Bitboard operator~ () const
    {
        return Bitboard(
          this->m ^
          (((foundation::uint128_t)(0x3ffffULL) << 64) +
            (foundation::uint128_t) 0x7fffffffffffffffULL)
        );
    }

    /// Boolean
                            operator bool() const;

    /// Assigning operators
    Bitboard &              operator =  (const Bitboard &);
    Bitboard &              operator&=  (const Bitboard &);
    Bitboard &              operator|=  (const Bitboard &);
    Bitboard &              operator^=  (const Bitboard &);
    Bitboard &              operator+=  (const Bitboard &);
    Bitboard &              operator-=  (const Bitboard &);
    Bitboard &              operator>>= (int);
    Bitboard &              operator<<= (int);

    /// Binary operators
    Bitboard                operator&   (const Bitboard &) const;
    Bitboard                operator|   (const Bitboard &) const;
    Bitboard                operator^   (const Bitboard &) const;
    Bitboard                operator+   (const Bitboard &) const;
    Bitboard                operator>>  (int)              const;
    Bitboard                operator<<  (int)              const;

    /// Binary operators with Square
    Bitboard                operator&   (Square::Square  ) const;
    Bitboard                operator|   (Square::Square  ) const;
    Bitboard                operator^   (Square::Square  ) const;

    /// Assigning operator with Square
    Bitboard &              operator|=  (Square::Square  ); 
    Bitboard &              operator^=  (Square::Square  ); 

    /// Pick a LSB 
    Square::Square          lsb         (void) const;

    /// Pick a LSB and reset it
    Square::Square          pick        (void);

    /// Count 1-bits in the board
    uint32_t                popcnt      (void) const;

    /// Unite the 64 bit variables: p[0] and p[1] into one
    uint64_t                unite       (void) const;

    /// Generate an index to the effect tables
    uint64_t                index       (const Bitboard &) const;

    /// Return a part of the board to which the given square belongs
    uint64_t                part        (Square::Square  ) const;

    /// Bitbord to chose vacant columns
    const Bitboard &        column      (void) const;

    /// Show raw bits in the board
    void                    show        (void);

    /// Show raw bits in the board
    friend std::ostream &   operator<<  (std::ostream &, const Bitboard &);

}  __attribute__ ((aligned (16)));


/**
 * Pick a LSB of the board
 * The result won't be predictable if both p[0] and p[1] is zero.
 * @return Square (0 - 80)
 */
inline Square::Square Bitboard::lsb (void) const
{

    using namespace foundation;

    int                     sq;

    // bit 63 of p[0] is not used, therefore, bit 0 of p[1] is 63
    sq = p[0] != 0 ? Bitope::bsf(p[0]) : Bitope::bsf(p[1]) + 63;

    return sq;

}


/**
 * Pick a LSB of the board and reset that bit.
 * The result won't be predictable if both p[0] and p[1] is zero.
 * @return Square (0 - 80)
 */
inline Square::Square Bitboard::pick (void)
{

    using namespace foundation;

    int                     sq;

    // bit 63 of p[0] is not used, therefore, bit 0 of p[1] is 63
    sq = p[0] != 0 ? Bitope::bsfr(p[0]) : Bitope::bsfr(p[1]) + 63;

    return sq;

}



/**
 * Count bits that are set 
 * @return number of bits set
 */
inline uint32_t Bitboard::popcnt (void) const
{

    return foundation::Bitope::popcnt(p[0], p[1]);

}



/**
 * Unite two 64 bit board values into one 64 bit value.
 * @return united value
 */
inline uint64_t Bitboard::unite (void) const
{

    return (p[0] ^ p[1]);

}



/**
 * Generate an index to the effect tables
 * @parm mask effect mask
 * @return index of current board for that mask
 */
inline uint64_t Bitboard::index (const Bitboard & mask) const
{

    Bitboard bbd(mask & *this);
    return foundation::Bitope::pext(bbd.unite(), mask.unite());

}



/**
 * Return a part (p[0] or p[1]) of the board to which the
 * given square belongs
 * @parm sq to be distinguished
 * @return a part of the board comprehending the square 
 */
inline uint64_t Bitboard::part (Square::Square sq) const
{

    return p[((sq + 1) >> 6)];

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

#endif
