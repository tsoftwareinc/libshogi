/**
 *****************************************************************************

 @file       Effect.h

 @brief      Effect definitions
  
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

#ifndef _GAME_EFFECT_H
#define _GAME_EFFECT_H

#include <Color.h>
#include <Bitboard.h>
#include <Direction.h>
#include <Square.h>

// begin namespace 'game::Effect'
namespace game { namespace Effect {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _GAME_EFFECT_DEBUG
#define _GAME_EFFECT_CHECK(x) { assert ( (x) ); }
#define _GAME_EFFECT_DEBUG_ERROR_STRING_MAX 256
#define _GAME_EFFECT_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "GAME_EFFECT_DEBUG     : " fmt, ## args); \
        }
#define _GAME_EFFECT_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_GAME_EFFECT_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _GAME_EFFECT_CHECK(x)
#define _GAME_EFFECT_DEBUG_OUT(fmt, args...)
#define _GAME_EFFECT_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------------ definitions ------------------------------ */

/**
 * The functions and tables concerning the effects. 
 * Apery and YaneuraOu gave me great idea to implement this.
 * 
 */

/// Initialize
void                    initialize (void);

/// KA effect
const Bitboard &        KA         (Square::Square, const Bitboard &);

/// HI effect
const Bitboard &        HI         (Square::Square, const Bitboard &);

/// KY effect
const Bitboard &        KY         (Color::Color, Square::Square,
                                                    const Bitboard &);

/// BKY effect
const Bitboard &        KB         (Square::Square, const Bitboard &);

/// WKY effect
const Bitboard &        KW         (Square::Square, const Bitboard &);

/// Adjacent effect
const Bitboard &        AD         (Square::Square, Piece::Piece);

/// KA effect in empty board
const Bitboard &        FK         (Square::Square);

/// HI effect in empty board
const Bitboard &        FH         (Square::Square);

/// Half KA effect on empty board (right shoulder up)
const Bitboard &        RS         (Square::Square);

/// Half KA effect on empty board (falling to the right)
const Bitboard &        FR         (Square::Square);

/// Half HI horizontal
const Bitboard &        HH         (Square::Square);

/// Half HI vertical
const Bitboard &        HV         (Square::Square);

/// Direction mask for escaping from distant effect
const Bitboard &        ED         (Square::Square, Direction::Direction);

/// Masking bitboard adjacent squares for duodecimal directions
const Bitboard &        DU         (Square::Square);

/// Masking bitboard adjacent squares for octant directions
const Bitboard &        OC         (Square::Square);

/// Masking bitboard distant squares for octant directions
const Bitboard &        LD         (Square::Square);

/* ------------------------------------------------------------------------- */

// end namespace 'game::Effect'
} }

#endif
