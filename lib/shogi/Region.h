/**
 *****************************************************************************

 @file       Region.h

 @brief      Definition of Region
  
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

#ifndef _GAME_REGION_H
#define _GAME_REGION_H

#include <Common.h>

// begin namespace 'game::Region'
namespace game { namespace Region {

/* ------------------------------ definitions ------------------------------ */

/// Rank of Shogi board with wall (RNK0 and RNKA)
enum Rank {
#define REGION_RNK_ALL \
    RNK0, \
    RNK1, \
    RNK2, \
    RNK3, \
    RNK4, \
    RNK5, \
    RNK6, \
    RNK7, \
    RNK8, \
    RNK9, \
    RNKA
    REGION_RNK_ALL,
    Ranks = 16
};
static const Rank            rankAll[] = { REGION_RNK_ALL };



/// File of Shogi board with the wall (FIL0 and FILA)
enum File {
#define REGION_FILE_ALL \
    FIL0, \
    FIL1, \
    FIL2, \
    FIL3, \
    FIL4, \
    FIL5, \
    FIL6, \
    FIL7, \
    FIL8, \
    FIL9, \
    FILA
    REGION_FILE_ALL,
    Files = 16
};
static const File            fileAll[] = { REGION_FILE_ALL };
static const File            fileALL[] = { 
    FILA,
    FIL9,
    FIL8,
    FIL7,
    FIL6,
    FIL5,
    FIL4,
    FIL3,
    FIL2,
    FIL1,
    FIL0
};



/// Shogi board surrounded by wall
using Region = int;
enum : Region {
#define REGION_REGION_ALL \
    SQ00, SQ01, SQ02, SQ03, SQ04, SQ05, SQ06, SQ07, SQ08, SQ09, SQ0A, \
    SQ10, SQ11, SQ12, SQ13, SQ14, SQ15, SQ16, SQ17, SQ18, SQ19, SQ1A, \
    SQ20, SQ21, SQ22, SQ23, SQ24, SQ25, SQ26, SQ27, SQ28, SQ29, SQ2A, \
    SQ30, SQ31, SQ32, SQ33, SQ34, SQ35, SQ36, SQ37, SQ38, SQ39, SQ3A, \
    SQ40, SQ41, SQ42, SQ43, SQ44, SQ45, SQ46, SQ47, SQ48, SQ49, SQ4A, \
    SQ50, SQ51, SQ52, SQ53, SQ54, SQ55, SQ56, SQ57, SQ58, SQ59, SQ5A, \
    SQ60, SQ61, SQ62, SQ63, SQ64, SQ65, SQ66, SQ67, SQ68, SQ69, SQ6A, \
    SQ70, SQ71, SQ72, SQ73, SQ74, SQ75, SQ76, SQ77, SQ78, SQ79, SQ7A, \
    SQ80, SQ81, SQ82, SQ83, SQ84, SQ85, SQ86, SQ87, SQ88, SQ89, SQ8A, \
    SQ90, SQ91, SQ92, SQ93, SQ94, SQ95, SQ96, SQ97, SQ98, SQ99, SQ9A, \
    SQA0, SQA1, SQA2, SQA3, SQA4, SQA5, SQA6, SQA7, SQA8, SQA9, SQAA
    REGION_REGION_ALL, Regions = 128
};
static const Region         all[]    = { REGION_REGION_ALL };



/// Index table to check if the square of Region is wall
static const bool           checkWall[] = {
    true, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, false, false, false, false, false, false, false, false, false, true,
    true, true,  true,  true,  true,  true,  true,  true,  true,  true,  true
};



/// Index table to get File from Region
static const Rank            regionToRank[] = {
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA,
    RNK0, RNK1, RNK2, RNK3, RNK4, RNK5, RNK6, RNK7, RNK8, RNK9, RNKA
};



/// Index table to get File from Region
static const File            regionToFile[] = {
    FIL0, FIL0, FIL0, FIL0, FIL0, FIL0, FIL0, FIL0, FIL0, FIL0, FIL0,
    FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1, FIL1,
    FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2, FIL2,
    FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3, FIL3,
    FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4, FIL4,
    FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5, FIL5,
    FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6, FIL6,
    FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7, FIL7,
    FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8, FIL8,
    FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9, FIL9,
    FILA, FILA, FILA, FILA, FILA, FILA, FILA, FILA, FILA, FILA, FILA
};



/// Index table to get Region from Square 
static const Region         squareToRegion[] = {
    SQ11, SQ12, SQ13, SQ14, SQ15, SQ16, SQ17, SQ18, SQ19,
    SQ21, SQ22, SQ23, SQ24, SQ25, SQ26, SQ27, SQ28, SQ29,
    SQ31, SQ32, SQ33, SQ34, SQ35, SQ36, SQ37, SQ38, SQ39,
    SQ41, SQ42, SQ43, SQ44, SQ45, SQ46, SQ47, SQ48, SQ49,
    SQ51, SQ52, SQ53, SQ54, SQ55, SQ56, SQ57, SQ58, SQ59,
    SQ61, SQ62, SQ63, SQ64, SQ65, SQ66, SQ67, SQ68, SQ69,
    SQ71, SQ72, SQ73, SQ74, SQ75, SQ76, SQ77, SQ78, SQ79,
    SQ81, SQ82, SQ83, SQ84, SQ85, SQ86, SQ87, SQ88, SQ89,
    SQ91, SQ92, SQ93, SQ94, SQ95, SQ96, SQ97, SQ98, SQ99
};



/// Delta of a step
using Direction = int;
static const Direction      NOFDR =   0;                // None of direction
static const Direction      RWARD = -11;                // Rightward
static const Direction      UWARD = - 1;                // Upward
static const Direction      LWARD =  11;                // Leftward
static const Direction      DWARD =   1;                // Downward
static const Direction      UWRDR = UWARD + RWARD;      // Upward right
static const Direction      UWRDL = UWARD + LWARD;      // Upward left
static const Direction      DWRDL = DWARD + LWARD;      // Downward left
static const Direction      DWRDR = DWARD + RWARD;      // Downward right
static const Direction      UWDKR = UWARD * 2 + RWARD;  // Upward keima right
static const Direction      UWDKL = UWARD * 2 + LWARD;  // Upward keima right
static const Direction      DWDKL = DWARD * 2 + LWARD;  // Upward keima right
static const Direction      DWDKR = DWARD * 2 + RWARD;  // Upward keima right

/// Table contains direction delta to square2 from square1
/// ADirectionTable[square1][square2]
extern const Direction      ADirectionTable [Regions]
                                            [Regions];

/// Table contains direction to square2 distant from square1
/// LDirectionTable[square1][square2]
extern const Direction      LDirectionTable [Regions]
                                            [Regions];




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
 * Calculate Region from Rank and File
 * @param r rank of the board
 * @param f file of the board
 * @return Region
 */
inline Region operator| (Rank r, File c)
{

    return (c * 11 + r);

}



/**
 * Calculate Region from Rank and File
 * @param f file of the board
 * @param r rank of the board
 * @return Region
 */
inline Region operator| (File f, Rank r)
{

    return (f * 11 + r);

}


/**
 * Convert Region to Rank
 * @param s Region
 * @return Rank
 */
inline Rank toRank(Region s)
{

    return regionToRank[s];

}



/**
 * Convert Region to Rank
 * @param s Region
 * @return File
 */
inline File toFile(Region s)
{

    return regionToFile[s];

}



/**
 * Check if the given position is wall
 * @param r region 
 * @return true if the place is wall
 */
inline bool isWall (Region r)
{

    return checkWall[r];

}



/**
 * Give direction delta to adjacent squares
 * @param from square from
 * @param to   square directed to
 * @return direction delta
 */
inline Direction adjacntDirection (Region r1, Region r2)
{

    return ADirectionTable[r1][r2];

}



/**
 * Give distant direction of squares
 * @param from square from
 * @param to   square directed to
 * @return direction
 */
inline Direction distantDirection (Region r1, Region r2)
{

    return LDirectionTable[r1][r2];

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Region'
} }

#endif
