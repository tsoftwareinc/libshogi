/**
 *****************************************************************************

 @file       Square.h

 @brief      Definition of the square
  
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

#ifndef _GAME_SQUARE_H
#define _GAME_SQUARE_H

#include <Common.h>

// begin namespace 'game::Square'
namespace game { namespace Square {

/* ------------------------------ definitions ------------------------------ */

/// Rank of Shogi board
enum Rank {
#define SQUARE_RANK_ALL \
    RNK1, \
    RNK2, \
    RNK3, \
    RNK4, \
    RNK5, \
    RNK6, \
    RNK7, \
    RNK8, \
    RNK9
    SQUARE_RANK_ALL,
    Ranks = 16
};
static const Rank           rankAll[] = { SQUARE_RANK_ALL };
static const Rank           rankRev[] = {
    RNK9,
    RNK8,
    RNK7,
    RNK6,
    RNK5,
    RNK4,
    RNK3,
    RNK2,
    RNK1
};
static const Rank           rankInt[] = {
    RNK2,
    RNK3,
    RNK4,
    RNK5,
    RNK6,
    RNK7,
    RNK8
};



/// File of Shogi board
enum File {
#define SQUARE_FILE_ALL \
    FIL1, \
    FIL2, \
    FIL3, \
    FIL4, \
    FIL5, \
    FIL6, \
    FIL7, \
    FIL8, \
    FIL9
    SQUARE_FILE_ALL,
    Files = 16
};
static const File           fileAll[] = { SQUARE_FILE_ALL };
static const File           fileRev[] = { 
    FIL9,
    FIL8,
    FIL7,
    FIL6,
    FIL5,
    FIL4,
    FIL3,
    FIL2,
    FIL1
};
static const File           fileInt[] = { 
    FIL2,
    FIL3,
    FIL4,
    FIL5,
    FIL6,
    FIL7,
    FIL8,
};



/// Square in the shogi board rotated counterclockwise
using Square = int;
enum : Square {
#define SQUARE_SQUARE_ALL \
    SQ11, SQ12, SQ13, SQ14, SQ15, SQ16, SQ17, SQ18, SQ19, \
    SQ21, SQ22, SQ23, SQ24, SQ25, SQ26, SQ27, SQ28, SQ29, \
    SQ31, SQ32, SQ33, SQ34, SQ35, SQ36, SQ37, SQ38, SQ39, \
    SQ41, SQ42, SQ43, SQ44, SQ45, SQ46, SQ47, SQ48, SQ49, \
    SQ51, SQ52, SQ53, SQ54, SQ55, SQ56, SQ57, SQ58, SQ59, \
    SQ61, SQ62, SQ63, SQ64, SQ65, SQ66, SQ67, SQ68, SQ69, \
    SQ71, SQ72, SQ73, SQ74, SQ75, SQ76, SQ77, SQ78, SQ79, \
    SQ81, SQ82, SQ83, SQ84, SQ85, SQ86, SQ87, SQ88, SQ89, \
    SQ91, SQ92, SQ93, SQ94, SQ95, SQ96, SQ97, SQ98, SQ99  
    SQUARE_SQUARE_ALL, SQVD, Squares = 128
};
static const Square         all[]     = { SQUARE_SQUARE_ALL };



/// Index table to get File from Square
static const Rank           squareToRank[]   = {
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9,
    RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9
};



/// Index table to get File from Square
static const File           squareToFile[]   = {
    FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1,
    FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2,
    FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3,
    FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4,
    FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5,
    FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6,
    FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7,
    FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8,
    FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9
};



/// Index table to get Square from Region
static const Square         regionToSquare[] = {
    SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
    SQVD, SQ11, SQ12, SQ13, SQ14, SQ15, SQ16, SQ17, SQ18, SQ19, SQVD,
    SQVD, SQ21, SQ22, SQ23, SQ24, SQ25, SQ26, SQ27, SQ28, SQ29, SQVD,
    SQVD, SQ31, SQ32, SQ33, SQ34, SQ35, SQ36, SQ37, SQ38, SQ39, SQVD,
    SQVD, SQ41, SQ42, SQ43, SQ44, SQ45, SQ46, SQ47, SQ48, SQ49, SQVD,
    SQVD, SQ51, SQ52, SQ53, SQ54, SQ55, SQ56, SQ57, SQ58, SQ59, SQVD,
    SQVD, SQ61, SQ62, SQ63, SQ64, SQ65, SQ66, SQ67, SQ68, SQ69, SQVD,
    SQVD, SQ71, SQ72, SQ73, SQ74, SQ75, SQ76, SQ77, SQ78, SQ79, SQVD,
    SQVD, SQ81, SQ82, SQ83, SQ84, SQ85, SQ86, SQ87, SQ88, SQ89, SQVD,
    SQVD, SQ91, SQ92, SQ93, SQ94, SQ95, SQ96, SQ97, SQ98, SQ99, SQVD,
    SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD
};



/// Index table to check if the square is horizontal wall
static const bool           checkHWll[]      = {
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
};



/// Index table to check if the square is vertical wall
static const bool           checkVWll[]      = {
    true,  true,  true,  true,  true,  true,  true,  true,  true,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false,
    true,  true,  true,  true,  true,  true,  true,  true,  true
};



/// Index table to check if the square is wall
static const bool           checkWall[]      = {
    true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, true,
    true,  true,  true,  true,  true,  true,  true,  true,  true
};



/// Delta of a step
using Direction = int;
static const Direction      NOFDR =   0;                // None of direction
static const Direction      RWARD = - 9;                // Rightward
static const Direction      UWARD = - 1;                // Upward
static const Direction      LWARD =   9;                // Leftward
static const Direction      DWARD =   1;                // Downward
static const Direction      UWRDR = UWARD     + RWARD;  // Upward right
static const Direction      UWRDL = UWARD     + LWARD;  // Upward left
static const Direction      DWRDL = DWARD     + LWARD;  // Downward left
static const Direction      DWRDR = DWARD     + RWARD;  // Downward right
static const Direction      UWDKR = UWARD * 2 + RWARD;  // Upward keima right
static const Direction      UWDKL = UWARD * 2 + LWARD;  // Upward keima right
static const Direction      DWDKL = DWARD * 2 + LWARD;  // Upward keima right
static const Direction      DWDKR = DWARD * 2 + RWARD;  // Upward keima right

/// Delta iterator
static const Direction      dirAll[]   = {NOFDR, RWARD, UWARD, LWARD, DWARD,
                                          UWRDR, UWRDL, DWRDL, DWRDR, UWDKR,
                                          UWDKL, DWDKL, DWDKR               };

/// Variation of directions (actually 13 but 16 for alignment)
static const int            Directions = 16;

/// Table contains direction delta to square2 from square1
/// ADirectionTable[square1][square2]
extern const Direction      ADirectionTable [Squares]
                                            [Squares];

/// Table contains direction to square2 distant from square1
/// LDirectionTable[square1][square2]
extern const Direction      LDirectionTable [Squares]
                                            [Squares];



/**
 * Increment Rank
 * @param r rank of the board
 * @return rank++
 */
inline Rank operator++ (Rank &r)
{

    r = static_cast<Rank>(r + 1);
    return r;

}



/**
 * Decrement Rank
 * @param r rank of the board
 * @return rank--
 */
inline Rank operator-- (Rank &r)
{

    r = static_cast<Rank>(r - 1);
    return r;

}



/**
 * Calculate Square from Rank and File
 * @param r rank of the board
 * @param f file of the board
 * @return Square
 */
inline Square operator| (Rank r, File f)
{

    return (f * 9 + r);

}



/**
 * Calculate Square from Rank and File
 * @param f file of the board
 * @param r rank of the board
 * @return Square
 */
inline Square operator| (File f, Rank r)
{

    return (f * 9 + r);

}



/**
 * Convert Square to Rank
 * @param s Square
 * @return Rank
 */
inline Rank toRank(Square s)
{

    return squareToRank[s];

}



/**
 * Convert Square to Rank
 * @param s Square
 * @return File
 */
inline File toFile(Square s)
{

    return squareToFile[s];

}



/**
 * Check if the given position is wall
 * @param s square
 * @return true if the place is wall.
 */
inline bool isWall (Square s)
{

    return checkWall[s];

}



/**
 * Check if the given position is horizontal wall
 * @param s square
 * @return true if the place is wall.
 */
inline bool isHWll (Square s)
{

    return checkHWll[s];

}



/**
 * Check if the given position is vertical wall
 * @param s square
 * @return true if the place is wall.
 */
inline bool isVWll (Square s)
{

    return checkVWll[s];

}



/**
 * Give direction delta to adjacent squares
 * @param from square from
 * @param to   square directed to
 * @return direction delta
 */
inline Direction adjacntDirection (Square sq1, Square sq2)
{

    return ADirectionTable[sq1][sq2];

}



/**
 * Give distant direction of squares
 * @param from square from
 * @param to   square directed to
 * @return direction
 */
inline Direction distantDirection (Square sq1, Square sq2)
{

    return LDirectionTable[sq1][sq2];

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Square'
} }

#endif
