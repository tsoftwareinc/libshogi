/**
 *****************************************************************************

 @file       Piece.h

 @brief      Definition of the pieces
  
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

#ifndef _GAME_PIECE_H
#define _GAME_PIECE_H

#include <string>
#include <Common.h>
#include <Color.h>

// begin namespace 'game::Piece'
namespace game { namespace Piece {

/* ------------------------------ definitions ------------------------------ */

// Definitions for Shogi pieces
using Piece = unsigned int;

static const int            PromotionShift = 3;

enum : Piece {

    /// Blank
    EMP   = 0,
    EM    = 0,

    /// Variation of the pieces
    FU    = 1,
    KY    = 2,
    KE    = 3,
    GI    = 4,
    KA    = 5,
    HI    = 6,
    KI    = 7,
    OU    = 8,

    /// Promotion bit
    Plane    = 0,
    Promoted = 1 << PromotionShift,

    TO    = FU   + Promoted,
    NY    = KY   + Promoted,
    NK    = KE   + Promoted,
    NG    = GI   + Promoted,
    UM    = KA   + Promoted,
    RY    = HI   + Promoted,

    /// Black or white bit
    Color = 1 << 4,           // color bit
    Black = 0     ,           // black to move first (Sente)
    White = 1 << 4,

    /// Black pieces
    BFU   = FU   + Black,     // black FU ( 1)
    BKY   = KY   + Black,     // black KY ( 2)
    BKE   = KE   + Black,     // black KE ( 3)
    BGI   = GI   + Black,     // black GI ( 4)
    BKA   = KA   + Black,     // black KA ( 5)
    BHI   = HI   + Black,     // black HI ( 6)
    BKI   = KI   + Black,     // black KI ( 7)
    BOU   = OU   + Black,     // black OU ( 8)
    BTO   = BFU  + Promoted,  // black TO ( 9)
    BNY   = BKY  + Promoted,  // black NY (10)
    BNK   = BKE  + Promoted,  // black NK (11)
    BNG   = BGI  + Promoted,  // black NG (12)
    BUM   = BKA  + Promoted,  // black UM (13)
    BRY   = BHI  + Promoted,  // black RY (14)

    /// White pieces
    WFU   = FU   + White,     // white FU (17)
    WKY   = KY   + White,     // white KY (18)
    WKE   = KE   + White,     // white KE (19)
    WGI   = GI   + White,     // white GI (20)
    WKA   = KA   + White,     // white KA (21)
    WHI   = HI   + White,     // white HI (22)
    WKI   = KI   + White,     // white KI (23)
    WOU   = OU   + White,     // white OU (24)
    WTO   = WFU  + Promoted,  // white TO (25)
    WNY   = WKY  + Promoted,  // white NY (26)
    WNK   = WKE  + Promoted,  // white NK (27)
    WNG   = WGI  + Promoted,  // white NG (28)
    WUM   = WKA  + Promoted,  // white UM (29)
    WRY   = WHI  + Promoted   // white RY (30)

};

/// All pieces
static const Piece          all[]   = {
                              EMP,
                              BFU, BKY, BKE, BGI, BKA, BHI, BKI,
                              BOU, BTO, BNY, BNK, BNG, BUM, BRY,
                              WFU, WKY, WKE, WGI, WKA, WHI, WKI,
                              WOU, WTO, WNY, WNK, WNG, WUM, WRY
                            };

/// All pieces have adjacent effect
static const Piece          effect[] = {
                              BKE, BGI, BKI,
                              BOU, BTO, BNY, BNK, BNG, BUM, BRY,
                              WKE, WGI, WKI,
                              WOU, WTO, WNY, WNK, WNG, WUM, WRY
                            };

/// Piece name string with polar
static const char * const   str[]   = {
                              "   ", "BFU", "BKY", "BKE", "BGI", "BKA", "BHI", "BKI",
                              "BOU", "BTO", "BNY", "BNK", "BNG", "BUM", "BRY", "   ",
                              "   ", "WFU", "WKY", "WKE", "WGI", "WKA", "WHI", "WKI",
                              "WOU", "WTO", "WNY", "WNK", "WNG", "WUM", "WRY", "   "
                            };

/// Piece name string
static const char * const   vst[]   = {
                               "  ",  "FU",  "KY",  "KE",  "GI",  "KA",  "HI",  "KI",
                               "OU",  "TO",  "NY",  "NK",  "NG",  "UM",  "RY",  "  ",
                               "  ",  "FU",  "KY",  "KE",  "GI",  "KA",  "HI",  "KI",
                               "OU",  "TO",  "NY",  "NK",  "NG",  "UM",  "RY",  "  "
                            };

/// CSA protocol 
static const char * const   csa[]   = {
                              " * ", "+FU", "+KY", "+KE", "+GI", "+KA", "+HI", "+KI",
                              "+OU", "+TO", "+NY", "+NK", "+NG", "+UM", "+RY", "   ",
                              "   ", "-FU", "-KY", "-KE", "-GI", "-KA", "-HI", "-KI",
                              "-OU", "-TO", "-NY", "-NK", "-NG", "-UM", "-RY", "   ",
                              "   ",  "FU",  "KY",  "KE",  "GI",  "KA",  "HI",  "KI",
                               "OU",  "TO",  "NY",  "NK",  "NG",  "UM",  "RY", "   "
                            };

/// Piece variation
static const Piece          var[]   = {
                              EM, FU, KY, KE, GI, KA, HI, KI, OU
                            };

/// Piece lining up in acordance with their values
static const Piece          val[]   = {
                              FU, KY, KE, GI, KI, KA, HI, OU
                            };

/// Piece in hands
static const Piece          hand[]   = {
                              FU, KY, KE, GI, KA, HI, KI
                            };

/// Kind of pieces
static const int            Kind    =  8;

/// Number of kinds of fundamental pieces
static const int            Variety = 16;

/// Mask to obtain fundamental piece
static const unsigned int   Mask    =  7;

/// Mask to obatin piece without polar
static const unsigned int   Neutral = 15;

/// Number of kinds of pieces with polarity
static const int            Pieces  = 32;

/// Number of same pieces at most
static const int            Depth   = 18;


/**
* Give the piece polarity (black or white)
* @param p fundamental piece
* @param c color
* @return colored piece
*/
inline Piece                polar(Piece p, Color::Color c)
{

    // compromising
    // this is assuming Color::Black is 0 and Color::White is 1
    const Piece             color[] = { Black, White };

    return (p | color[c]);

}



/**
* Remove polarity (black or white) from the piece 
* @param p fundamental piece
* @return neutral piece
*/
inline Piece                neutral(Piece p)
{

    return (p & Mask);

}



/**
* Return the piece's polarity (black or white)
* @param p piece
* @return color
*/
inline Color::Color         color(Piece p)
{

    return ((p & Color) >> 4);

}



/**
 * Convert CSA piece string to numeric piece value
 * @param str CSA piece string
 * @return numeric piece value
 */
inline Piece csaToPiece (const std::string &str)
{

    // board pieces
    for (int i =  0    ; i < Pieces       ; ++i) {
        if (str == csa[i]) {
            return i;
        }
    }

    // pieces in hands
    for (int i = Pieces; i < (Pieces * 2) ; ++i) {
        if (str == csa[i]) {
            return (i & Neutral);
        }
    }

    return EMP;

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Piece'
} }

#endif
