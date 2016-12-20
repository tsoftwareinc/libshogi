/**
 *****************************************************************************

 @file       Direction.h

 @brief      Definition of the direction
  
 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

 @date       2016-03-31

 @version    $Id:$


  Copyright 2014, 2015, 2016 Hiroki Takada

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

#ifndef _GAME_DIRECTION_H
#define _GAME_DIRECTION_H

#include <Common.h>
#include <Square.h>

// begin namespace 'game::Direction'
namespace game { namespace Direction {

/* ------------------------------ definitions ------------------------------ */

/// Directions (bitmap)
using DirectionBits = unsigned int;

static const DirectionBits  NONE = 0x000;   /// None of direction
static const DirectionBits  RWRD = 0x001;   /// Rightward
static const DirectionBits  UWDR = 0x002;   /// Upward right
static const DirectionBits  UWRD = 0x004;   /// Upward
static const DirectionBits  UWDL = 0x008;   /// Upward left

static const DirectionBits  UWKR = 0x010;   /// Upwward keima right
static const DirectionBits  UWKL = 0x020;   /// Upwward keima left
static const DirectionBits  DWKL = 0x040;   /// Downward keima left
static const DirectionBits  DWKR = 0x080;   /// Downward keima right

static const DirectionBits  LWRD = 0x100;   /// Leftward
static const DirectionBits  DWDL = 0x200;   /// Downward left
static const DirectionBits  DWRD = 0x400;   /// Downward
static const DirectionBits  DWDR = 0x800;   /// Downward right

static const DirectionBits  bitAll[] = {
                                        RWRD,
                                        UWDR,
                                        UWRD,
                                        UWDL,
                                        LWRD,
                                        DWDL,
                                        DWRD,
                                        DWDR,
                                        UWKR,
                                        UWKL,
                                        DWKL,
                                        DWKR
                                       };



/// Directions
/// For the octant direction, XOR Reverse gives opposite direction
enum Direction {
    NOFDR = 0,
    RWARD = 1,
    UWRDR = 2,
    UWARD = 3,
    UWRDL = 4,
    LWARD = 9,
    DWRDL = 10,
    DWARD = 11,
    DWRDR = 12,
    UWDKR = 5,
    UWDKL = 6,
    DWDKL = 7,
    DWDKR = 8
};
static const Direction      all[]    = {
                                        RWARD, UWRDR, UWARD, UWRDL,
                                        LWARD, DWRDL, DWARD, DWRDR,
                                        UWDKR, UWDKL, DWDKL, DWDKR
                                       };
 
/// Variety of the directions is 12 (16 for memory alignment)
static const int            Directions = 16;

/// Mask to obtain opposite direction
static const unsigned int   Reverse    =  8;

/// Table contains directionbits to square2 from square1
/// ADirectionBitsTable[square1][square2]
extern const DirectionBits  ADirectionBitsTable[Square::Squares]
                                               [Square::Squares];

/// Table contains direction delta to square2 from square1
/// ADirectionTable[square1][square2]
extern const Direction      ADirectionTable    [Square::Squares]
                                               [Square::Squares];

/// Table contains direction to square2 distant from square1
/// LDirectionTable[square1][square2]
extern const Direction      LDirectionTable    [Square::Squares]
                                               [Square::Squares];


/**
 * Calculate DirectionBits from Direction
 * @param d direction
 * @return DirectionBits
 */
inline DirectionBits  directionToBits(Direction d)
{

    return (1 << d);

}



/**
 * Give direction bits to adjacent squares
 * @param from square from
 * @param to   square directed to
 * @return direction bits
 */
inline DirectionBits adjacntDirectionBit (Square::Square sq1,
                                      Square::Square sq2)
{

    return ADirectionBitsTable[sq1][sq2];

}



/**
 * Give direction to adjacent squares
 * @param from square from
 * @param to   square directed to
 * @return direction
 */
inline Direction adjacntDirection (Square::Square sq1,
                                   Square::Square sq2)
{

    return ADirectionTable[sq1][sq2];

}



/**
 * Give distant direction to the squares
 * @param from square from
 * @param to   square directed to
 * @return distant direction
 */
inline Direction distantDirection (Square::Square sq1,
                                   Square::Square sq2)
{

    return LDirectionTable[sq1][sq2];

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Direction'
} }

#endif
