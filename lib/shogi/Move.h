/**
 *****************************************************************************

 @file       Move.h

 @brief      Definition of the move
  
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

#ifndef _GAME_MOVE_H
#define _GAME_MOVE_H

#include <Common.h>
#include <Square.h>
#include <Piece.h>

// begin namespace 'game::Move'
namespace game { namespace Move {

/* ------------------------------ definitions ------------------------------ */

/**
 *  Move
 *   is 16 bit-value expressed as the followings;
 *
 *   MSB                                                         LSB
 *   15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
 *  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *  |   |   |                           |                           |
 *  |   |   |    Square moved from      |     Square moved to       |
 *  |   |   |                           |                           |
 *  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *    |   |
 *    |   +--- indicator for dropping moves.
 *    |    
 *    +------- Indicator for promoting moves.
 *
 *    YaneuraOu combines this with 16 bits evaluation value to 32 bits
 *    value. This makes it easy for CPU to line the values in its cache.
 *
 */

static const int            MoveMask       = 0x0000ffff;
static const int            PromotionShift = 15;
static const int            ValueShift     = 16;
static const int            ValueMask      = 0xffff0000;
static const int            ValueOffset    = 0x8000;

enum Move : uint32_t {

    /// Void move
    None    = 0,

    /// Dropping move
    Drop    = 1 << 14,

    /// Promoting move
    Promote = 1 << PromotionShift

};


/// Number of move at most
static const int            Max = 608;



/**
 * Check if the move is null
 * @param m move value
 * @return true if the move is null
 */
inline bool isNull (Move m)
{

    return ((m & MoveMask) == None);

}



/**
 * Read Square moved from in Move value
 * @param m move value
 * @return Square
 */
inline Square::Square from (Move m)
{

    return ((m >> 7) & 0x7f);

}



/**
 * Read Square moved to in Move value
 * @param m move value
 * @return Square
 */
inline Square::Square to (Move m)
{

    return (m & 0x7f);

}



/**
 * Generate Move from Square values
 * @param frm Square moved from
 * @param to  Square moved to
 * @return Move 
 */
inline Move move (Square::Square frm, Square::Square to)
{

    return static_cast<Move>((frm << 7) | to);

}



/**
 * Generate promoting Move from Square values
 * @param frm Square moved from
 * @param to  Square moved to
 * @return Move 
 */
inline Move promote (Square::Square frm, Square::Square to)
{

    return static_cast<Move>(Promote | (frm << 7) | to);

}



/**
 * Generate dropping Move
 * @param p  Piece dropped to
 * @param to Square dropped to
 * @return Move 
 */
inline Move drop (Piece::Piece p, Square::Square to)
{

    return static_cast<Move>(Drop | (p << 7) | to);

}



/**
 * Set value to the move
 * @param v value to set
 * @return move with vaue
 */
inline Move setValue (int v, const Move &m)
{

    v += ValueOffset;
    return static_cast<Move>((m & (~ValueMask)) | (v << ValueShift));

}



/**
 * Set positive value to the move
 * @param v value to set
 * @return move with vaue
 */
inline Move setPValue (int v, const Move &m)
{

    return static_cast<Move>((m & (~ValueMask)) | (v << ValueShift));

}



/**
 * Set negative value to the move
 * @param v value to set
 * @return move with vaue
 */
inline Move setNValue (int v, const Move &m)
{

    v = -v;
    return static_cast<Move>((m & (~ValueMask)) | (v << ValueShift));

}



/**
 * Value of the move
 * @return  value of the move
 */
inline int value (const Move &m)
{

    int v = ((m & ValueMask) >> ValueShift);
    return (v - ValueOffset);

}



/**
 * Positive value of the move
 * @return  value of the move
 */
inline int pvalue (const Move &m)
{

    int v = ((m & ValueMask) >> ValueShift);
    return v;

}



/**
 * Negative value of the move
 * @return  value of the move
 */
inline int nvalue (const Move &m)
{

    int v = ((m & ValueMask) >> ValueShift);
    return -v;

}

/* ------------------------------------------------------------------------- */
// end namespace 'game::Move'
} }

#endif
