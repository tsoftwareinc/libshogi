/**
 *****************************************************************************

 @file       Effect.h

 @brief      Effect definitions
  
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

#ifndef _GAME_EFFECT_H
#define _GAME_EFFECT_H

#include <Color.h>
#include <Piece.h>
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

/*
 * Lookup tables. They are defined in Effect.cpp and declared here only so
 * that the accessor functions below can be inlined. Use the accessors
 * instead of touching the tables directly.
 */

/// KA effect tables
extern Bitboard         _KAEffectT[20224  + 1];
extern Bitboard         _KAEffectM[Square::Squares];
extern int              _KAEffectI[Square::Squares];

/// HI effect tables
extern Bitboard         _HIEffectT[495616 + 1];
extern Bitboard         _HIEffectM[Square::Squares];
extern int              _HIEffectI[Square::Squares];

/// KY effect tables
extern Bitboard         _KYEffectT[Square::Squares][Color::Colors][128];
extern Bitboard         _KBEffectT[Square::Squares][128];
extern Bitboard         _KWEffectT[Square::Squares][128];

/// Adjacent effect table
extern Bitboard         _ADEffectT[Square::Squares][Piece::Pieces];

/// Direction mask for escaping from distant effect
extern Bitboard         _direction[Square::Squares][Direction::Directions];

/// Constant masks
extern const Bitboard   _duodecimal[Square::Squares];
extern const Bitboard   _octant    [Square::Squares];
extern const Bitboard   _distant   [Square::Squares];
extern const Bitboard   _rsup      [Square::Squares];
extern const Bitboard   _failtor   [Square::Squares];
extern const Bitboard   _hhorizont [Square::Squares];
extern const Bitboard   _hvertical [Square::Squares];

/// KA effect
inline const Bitboard & KA (Square::Square sq, const Bitboard &ocp)
{
    return _KAEffectT[_KAEffectI[sq] + ocp.index(_KAEffectM[sq])];
}

/// HI effect
inline const Bitboard & HI (Square::Square sq, const Bitboard &ocp)
{
    return _HIEffectT[_HIEffectI[sq] + ocp.index(_HIEffectM[sq])];
}

/// KY effect
inline const Bitboard & KY (Color::Color c, Square::Square sq,
                                              const Bitboard &ocp)
{
    return _KYEffectT[sq][c][(ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f];
}

/// BKY effect
inline const Bitboard & KB (Square::Square sq, const Bitboard &ocp)
{
    return _KBEffectT[sq][(ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f];
}

/// WKY effect
inline const Bitboard & KW (Square::Square sq, const Bitboard &ocp)
{
    return _KWEffectT[sq][(ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f];
}

/// Adjacent effect
inline const Bitboard & AD (Square::Square sq, Piece::Piece p)
{
    return _ADEffectT[sq][p];
}

/// KA effect in empty board
inline const Bitboard & FK (Square::Square sq)
{
    return _ADEffectT[sq][Piece::BKA];
}

/// HI effect in empty board
inline const Bitboard & FH (Square::Square sq)
{
    return _ADEffectT[sq][Piece::BHI];
}

/// Half KA effect on empty board (right shoulder up)
inline const Bitboard & RS (Square::Square sq)
{
    return _rsup[sq];
}

/// Half KA effect on empty board (falling to the right)
inline const Bitboard & FR (Square::Square sq)
{
    return _failtor[sq];
}

/// Half HI horizontal
inline const Bitboard & HH (Square::Square sq)
{
    return _hhorizont[sq];
}

/// Half HI vertical
inline const Bitboard & HV (Square::Square sq)
{
    return _hvertical[sq];
}

/// Direction mask for escaping from distant effect
inline const Bitboard & ED (Square::Square sq, Direction::Direction dr)
{
    return _direction[sq][dr];
}

/// Masking bitboard adjacent squares for duodecimal directions
inline const Bitboard & DU (Square::Square sq)
{
    return _duodecimal[sq];
}

/// Masking bitboard adjacent squares for octant directions
inline const Bitboard & OC (Square::Square sq)
{
    return _octant[sq];
}

/// Masking bitboard distant squares for octant directions
inline const Bitboard & LD (Square::Square sq)
{
    return _distant[sq];
}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Effect'
} }

#endif
