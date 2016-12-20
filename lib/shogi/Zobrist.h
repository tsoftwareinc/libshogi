/**
 *****************************************************************************

 @file       Zobrist.h

 @brief      Definitions for Zobrist Hashing
  
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

#ifndef _GAME_ZOBRIST_H
#define _GAME_ZOBRIST_H

#include <Color.h>
#include <Piece.h>
#include <Square.h>

// begin namespace 'game::Zobrist'
namespace game { namespace Zobrist {

/* ------------------------------ definitions ------------------------------ */

/**
 * GPS Shogi and YaneuraOu proposed the modified Zobrist Hashing using ADD
 * instead of XOR. This trick efficiently reduces the cost of hasing for hand
 * pieces. libshogi is using XOR for pieces on the board and ADD for those
 * in the hand.
 * 
 */

/// Type of data being used for hasing key 
using key = uint64_t;

/// Bitboard with a filled particular row
extern key                  position[Square::Squares][Piece::Pieces];

/// Bitboard with a filled particular column
extern key                  hands[Color::Colors][Piece::Variety];

/// Initialize
void                        initialize (void);

/* ------------------------------------------------------------------------- */

// end namespace 'game::Zobrist'
} }

#endif
