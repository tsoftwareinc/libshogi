/**
 *****************************************************************************

 @file       Position.cpp

 @brief      Position implementation
  
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

#include <iomanip>
#include <sstream>
#include <cstring>

#include <Position.h>
#include <Piece.h>
#include <Effect.h>

using namespace foundation;

// begin namespace 'game'
namespace game {

/* ------------------------------ parameters ------------------------------- */

// ANSI terminal escape code
namespace Terminal {
static const char *         GRN = "\033[32;1m";
static const char *         YLW = "\033[33;1m";
static const char *         BLU = "\033[34;1m";
static const char *         WHT = "\033[37;0m";
}

// promotion area for black RNK1 - RNK3
static constexpr Bitboard   BlackPRMask(
                                  0x01c0e070381c0e07ULL,
                                  0x0000000000000e07ULL );
static constexpr Bitboard   BlackPNMask = ~BlackPRMask;

// promotion area for white RNK7 - RNK9
static constexpr Bitboard   WhitePRMask(
                                  0x70381c0e070381c0ULL,
                                  0x00000000000381c0ULL );
static constexpr Bitboard   WhitePNMask = ~WhitePRMask;

// the area not forcing promotion for FU : RNK2 - RNK3 for balack
// and RNK7 - RNK8 for white
static constexpr Bitboard   NFBFU(0x0180c06030180c06ULL,
                                  0x0000000000000c06ULL );
static constexpr Bitboard   NFWFU(0x30180c06030180c0ULL,
                                  0x00000000000180c0ULL );

// the area not forcing promotion for KY : RNK2 - RNK9 for balack
// and RNK1 - RNK8 for white
static constexpr Bitboard   NFBKY(0x7fbfdfeff7fbfdfeULL,
                                  0x000000000003fdfeULL );
static constexpr Bitboard   NFWKY(0x3fdfeff7fbfdfeffULL,
                                  0x000000000001feffULL );

// the area not forcing promotion for KE : RNK3 - RNK9 for balack
// and RNK1 - RNK7 for white
static constexpr Bitboard   NFBKE(0x7f3f9fcfe7f3f9fcULL,
                                  0x000000000003f9fcULL );
static constexpr Bitboard   NFWKE(0x1fcfe7f3f9fcfe7fULL,
                                  0x000000000000fe7fULL );

// bitboard to indicate the squares FU and KY can be placed
static constexpr Bitboard   CanDropBFU (
                                  0x7fbfdfeff7fbfdfeULL,
                                  0x000000000003fdfeULL
                            );
static constexpr Bitboard   CanDropWFU (
                                  0x3fdfeff7fbfdfeffULL,
                                  0x000000000001feffULL
                            );

// bitboard to indicate the squares KE can be placed
static constexpr Bitboard   CanDropBKE (
                                  0x7f3f9fcfe7f3f9fcULL,
                                  0x000000000003f9fcULL
                            );
static constexpr Bitboard   CanDropWKE (
                                  0x1fcfe7f3f9fcfe7fULL,
                                  0x000000000000fe7fULL
                            );

// boolean to chek promotion area 
static const bool           BlackCanPromote [Square::Squares] = {
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false,
             true,  true,  true, false, false, false, false, false, false };

static const bool           WhiteCanPromote [Square::Squares] = {
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true,
            false, false, false, false, false, false,  true,  true,  true };

// boolean to chek forced promotion area for FU and KY
static const bool           BFUMPromote [Square::Squares] = {
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false,
             true, false, false, false, false, false, false, false, false };

static const bool           WFUMPromote [Square::Squares] = {
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true,
            false, false, false, false, false, false, false, false,  true };

// boolean to chek forced promotion area for KE
static const bool           BKEMPromote [Square::Squares] = {
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false,
             true,  true, false, false, false, false, false, false, false };

static const bool           WKEMPromote [Square::Squares] = {
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true,
            false, false, false, false, false, false, false,  true,  true };

// table of functions give the effect for pinned direction
static const Bitboard &     (* DirectionMap[])(Square::Square) = {
                                nullptr,        // none of direction
                                Effect::HH,     // rightward
                                Effect::RS,     // upward right
                                Effect::HV,     // upward
                                Effect::FR,     // upward left
                                nullptr,        // upward keima right
                                nullptr,        // upward keima left
                                nullptr,        // downward keima left
                                nullptr,        // downward keima right
                                Effect::HH,     // leftward
                                Effect::RS,     // downward left
                                Effect::HV,     // downward
                                Effect::FR      // downward right
                            };

/* ------------------------------------------------------------------------- */



/* --------------------------- global  variables --------------------------- */

//
// Value table of pieces
//

// values for the pieces on the board
Evaluation::Eval            Position::_pieceVal [Piece::Pieces] = {
                                0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0
                            };

// values for the pieces in the hands
Evaluation::Eval            Position::_handsVal [EMPHandsMask + 1]
                                                 [Color::Colors][Piece::Kind] = {
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
                              {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}
                            };

//
// Context cache on thread local storage
//
#ifdef USE_THREADLOCALSTORAGE
// Pinned pieces
thread_local Bitboard       Position::_pinnd;

// Number of checks
thread_local int            Position::_nchek;

// All pieces giving check
thread_local Bitboard       Position::_chckp;

// Adjacent pieces giving check
thread_local Bitboard       Position::_chkAD;

// KY giving check
thread_local Bitboard       Position::_chkKY;

// KA giving check
thread_local Bitboard       Position::_chkKA;

// HI giving check
thread_local Bitboard       Position::_chkHI;

// Distant effect for intermediate blocking
thread_local Bitboard       Position::_chkDE;

// Minor moves
thread_local Array<Move::Move, Position::MinorMoves> Position::_m;

// Effect cache flag
thread_local Bitboard       Position::_effect;

// Effect cache flag
thread_local Move::Move     Position::_lastmove;
#endif

/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * Set values for pieces on board
 * @param v array of the values
 */
void Position::setValue (const Evaluation::Eval (& v)[Piece::Pieces])
{

    for (auto p : Piece::all) {
        _pieceVal[p] = v[p];
    }

}



/**
 * Set values for pieces in hands
 * @param v array of the values
 */
void Position::handsValue (const Evaluation::Eval (& v)[Piece::Depth + 1]
                                                       [Piece::Kind]     )
{

    for (int d = 0; d < Piece::Depth + 1; ++d) {
        for (auto p : Piece::hand) {
            _handsVal[d][Color::Black][p] =  v[d][p];
            _handsVal[d][Color::White][p] = -v[d][p];
        }
    }

}



/**
 * Get yourTurn in CSA game summary
 * @param g CSASummary
 * @return your turn in color
 */
Color::Color Position::myTurn (const CSASummary &g)
{

    return (g.yourTurn == "+" ? Color::Black : Color::White);

}



/**
 * Copy constructor avoiding copying context cache
 * @param v origin
 */
Position::Position (const Position &v)
 : _keyBoard(v._keyBoard), _keyHands(v._keyHands), _key(v._key),
   _kingSB(v._kingSB), _kingSW(v._kingSW), _ocupd(v._ocupd), _empty(v._empty),
   _exchg(v._exchg), _last(v._last), _next(v._next), _numMoves(v._numMoves)
{

    // copy the board
    memcpy(_board, v._board, sizeof(_board));

    // copy pieces in the hands
    for (auto p : Piece::hand) {
        _hands[Color::Black][p] = v._hands[Color::Black][p];
        _hands[Color::White][p] = v._hands[Color::White][p];
    }
    _hands[Color::Black][Piece::EMP] = 0;
    _hands[Color::White][Piece::EMP] = 0;

    // copy the bitboards
    memcpy(_bbord, v._bbord, sizeof(_bbord));

    // copy the occupied square
    _piece[Color::Black] = v._piece[Color::Black];
    _piece[Color::White] = v._piece[Color::White];

    // copy doesn't affect TLS cache.
    // we don't have to invoke makeCheck() here.
#ifndef USE_THREADLOCALSTORAGE
    makeCheck();
#endif

}



/**
 * Constructor takes CSA game summary
 * @param g game summary of CSA connection
 */
Position::Position (const CSASummary &g)
 : _exchg(0), _numMoves(0)
{

    // CSA expression for position and hands
    // ex. position[0] : "-KY-KE+KA *  * -KI-GI-KE-OU"
    //     position[1] : " * -HI *  *  * -GI *  * -KY"
    //     position[2] : "-FU * -FU-FU *  *  * -FU-FU"
    //     position[3] : " * -FU *  * -FU-KI+GI *  * "
    //     position[4] : " *  *  *  *  *  *  *  *  * "
    //     position[5] : " *  * +FU+FU *  *  *  *  * "
    //     position[6] : "+FU+FU *  * +FU * +FU+FU+FU"
    //     position[7] : " * +KA *  *  *  *  * +GI+KY"
    //     position[8] : "+KY+KE * +KI * +KI * +KE+OU"
    //     senteHnad   : "00FU00FU"
    //     goteHand    : "00HI00FU"
    //     firstPlayer : "-"
    //
    const int               boardStrLen = 3;
    const int               handsStrLen = 4;

    // clear the bitboards and the hands
    _clear();

    // make postion
    for (auto r : Square::rankAll) {
        for (auto f : Square::fileRev) {
            int  ptr   = (Square::FIL9 - f) * boardStrLen;
            auto pstr  = g.position.value(r);
            if (pstr[pstr.size() - 1] == '*') {
                pstr += ' ';
            }
            auto p     = Piece::csaToPiece(pstr.substr(ptr, boardStrLen));
            auto c     = Piece::color(p);
            auto s     = r | f;
            _board[s]  = static_cast<unsigned char>(p);
            _bbord[p] ^= Bitboard::Square[s];
            _piece[c] ^= p != Piece::EMP ? Bitboard::Square[s] : Bitboard::Zero;
            _exchg    += _pieceVal[p];
        }
    }

    // occupied bitboard
    _ocupd = ~_bbord[Piece::EMP];

    // empty bitboard 
    _empty =  _bbord[Piece::EMP];


    // hands for Black
    _hands[Color::Black][Piece::EMP] = 0;
    for (size_t i = 0; i < g.senteHand.size(); i += handsStrLen) {
        auto p   = Piece::csaToPiece(g.senteHand.substr(i + 2, 2));
        int &d   = _hands[Color::Black][p];
        _exchg  -= _handsVal[d++][Color::Black][p];
        _exchg  += _handsVal[d  ][Color::Black][p];
    }
    // hands for White
    _hands[Color::White][Piece::EMP] = 0;
    for (size_t i = 0; i < g.goteHand.size() ; i += handsStrLen) {
        auto p   = Piece::csaToPiece(g.goteHand.substr (i + 2, 2));
        int &d   = _hands[Color::White][p];
        _exchg  -= _handsVal[d++][Color::White][p];
        _exchg  += _handsVal[d  ][Color::White][p];
    }

    // square of OU
    _kingSB = Square::SQVD;
    _kingSW = Square::SQVD;
    if (_bbord[Piece::BOU]) {
        _kingSB = _bbord[Piece::BOU].lsb();
    }
    if (_bbord[Piece::WOU]) {
        _kingSW = _bbord[Piece::WOU].lsb();
    }

    // turn
    if (g.firstPlayer == "+") {
        _last = Color::White;
        _next = Color::Black;
    } else {
        _last = Color::Black;
        _next = Color::White;
    }

    // hashing
    _hashFull();

    // make check
    makeCheck();

}



/**
 * Clear the hands and the bitboards
 *
 */
void Position::_clear (void)
{

    // clear the hands
    for (auto p : Piece::hand) {
        _hands[Color::Black][p] = 0;
        _hands[Color::White][p] = 0;
    }
    _hands[Color::Black][Piece::EMP] = 0;
    _hands[Color::White][Piece::EMP] = 0;

    // clear the bitboard
    for (auto p : Piece::all) {
        _bbord[p] = Bitboard::Zero;
    }

    // clear the deployment
    _piece[Color::Black] = Bitboard::Zero;
    _piece[Color::White] = Bitboard::Zero;

    // reset the exchange and the number of moves
    _exchg    = 0;
    _numMoves = 0;

}



/**
 * Reset to the initial position of Shogi
 *
 */
void Position::init (void)
{

    using namespace Piece;

    // hirate position
    const unsigned char     h[Square::Squares] = {
                                WKY, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BKY,
                                WKE, WKA, WFU, EMP, EMP, EMP, BFU, BHI, BKE,
                                WGI, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BGI,
                                WKI, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BKI,
                                WOU, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BOU,
                                WKI, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BKI,
                                WGI, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BGI,
                                WKE, WHI, WFU, EMP, EMP, EMP, BFU, BKA, BKE,
                                WKY, EMP, WFU, EMP, EMP, EMP, BFU, EMP, BKY
                            };

    // clear the bitboards and the hands
    _clear();

    // make position 
    for (auto s : Square::all) {
        auto p     = h[s];
        auto c     = Piece::color(p);
        _board[s]  = p;
        _bbord[p] ^= Bitboard::Square[s];
        _piece[c] ^= (p != Piece::EMP ? Bitboard::Square[s] : Bitboard::Zero);
        _exchg    += _pieceVal[p];
    }

    // occupied bitboard
    _ocupd  = ~_bbord[Piece::EMP];

    // empty bitboard
    _empty  =  _bbord[Piece::EMP];

    // square of OU
    _kingSB = Square::SQ59;
    _kingSW = Square::SQ51;

    // turn
    _last   = Color::White;
    _next   = Color::Black;

    // hashing
    _hashFull();

    // make check
    makeCheck();

}



/**
 * Whole hash key
 * @return hash key for both the board and the hands
 */
Zobrist::key Position::hash (void) const
{

    return _key;

}



/**
 * Bitboard for the occupied squares
 * @return occupied bitboard
 */
const Bitboard & Position::occupied (void) const
{

    return _ocupd;

}



/**
 * Bitboard for the empty squares
 * @return empty bitboard
 */
const Bitboard & Position::empty (void) const
{

    return _empty;

}



/**
 * Exchange value
 * @return exchange value
 */
Evaluation::Eval Position::exchange (void) const
{

    return _exchg;

}



/**
 * Next player to move
 * @return next player
 */
Color::Color Position::turn (void) const
{

    return _next;

}



/**
 * Number of the pieces in the hands
 * @parm c color
 * @parm p piece 
 * @return number of pieces
 */
int Position::hand (Color::Color c, Piece::Piece p) const
{

    return _hands[c][p];

}



/**
 * Piece on the board
 * @parm sq square of the board
 * @return piece (including Piece::EMP)
 */
Piece::Piece Position::square (Square::Square sq) const
{

    return _board[sq];

}



/**
 * Bitboard
 * @param p piece
 * @return bitboard
 */
const Bitboard & Position::bitboard (Piece::Piece p) const
{

    return _bbord[p];

}



/**
 * Bitboard for the occupied squares by color
 * @param c color
 * @return occupied bitboard
 */
const Bitboard & Position::occupied (Color::Color c) const
{

    return _piece[c];

}



/**
 * Set next player to move
 * @param c color of the next player
 */
void Position::turn (Color::Color c)
{

    _last = Color::flip(c);
    _next = c;
    makeCheck();

}



/**
 * Perform a dropping move, assuming the given move is always legal 
 * @param m move to perform
 * @return the performed move
 */
Move::Move Position::drop (const Move::Move &m)
{

    // piece and square doropped to
    Piece ::Piece  pc  = Move::from(m);
    Square::Square to  = Move::to  (m);

    // reduce hands
    int &d             = _hands[_next][pc];
    _keyHands         -= Zobrist::hands[_next][pc];
    _exchg            -= _handsVal[d--][_next][pc];
    _exchg            += _handsVal[d  ][_next][pc];

    // drop
    pc                 = Piece::polar(pc, _next);
    _board[to]         = static_cast<unsigned char>(pc);
    _bbord[pc]        ^= Bitboard::Square[to];
    _piece[_next]     ^= Bitboard::Square[to];
    _keyBoard         ^= Zobrist::position[to][pc];
    _exchg            += _pieceVal[pc];

    // update occupied bitboard
    _ocupd            ^= Bitboard::Square[to];

    // update empty bitboard
    _empty            ^= Bitboard::Square[to];

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // flip the turn
    _last              = _next;
    _next              = Color::flip(_next);

    // number of moves
    ++_numMoves;

    // make cache
    makeCheck();

    return m;

}



/**
 * Undo the last dropping move, assuming the given move is always legal 
 * @param m move to perform
 */
void Position::remove (const Move::Move &m)
{

    // flip the turn
    _last              = _next;
    _next              = Color::flip(_next);

    // piece and square doropped to
    Piece ::Piece  pc  = Move::from(m);
    Square::Square to  = Move::to  (m);

    // increment hands
    int &d             = _hands[_next][pc];
    _keyHands         += Zobrist::hands[_next][pc];
    _exchg            -= _handsVal[d++][_next][pc];
    _exchg            += _handsVal[d  ][_next][pc];

    // remove
    pc                 = Piece::polar(pc, _next);
    _board[to]         = static_cast<unsigned char>(Piece::EMP);
    _bbord[pc]        ^= Bitboard::Square[to];
    _piece[_next]     ^= Bitboard::Square[to];
    _keyBoard         ^= Zobrist::position[to][pc];
    _exchg            -= _pieceVal[pc];

    // update occupied bitboard
    _ocupd            ^= Bitboard::Square[to];

    // update empty bitboard
    _empty            ^= Bitboard::Square[to];

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // number of moves
    --_numMoves;

    // make cache
    makeCheck();

}



/**
 * Make a move, assuming the given move is always legal 
 * @param m move to perform
 * @return the performed move
 */
Move::Move Position::move (const Move::Move &m)
{

    // save the move
    _lastmove = m;

    // dropping move
    if (m & Move::Drop) {
        return drop(m);
    }

    // suquares
    Square::Square fm  = Move::from(m);
    Square::Square to  = Move::to  (m);

    // moved piece 
    Piece ::Piece  pc  = _board[fm];
    _bbord[pc]        ^= Bitboard::Square[fm];
    _piece[_next]     ^= Bitboard::Square[fm];
    _keyBoard         ^= Zobrist::position[fm][pc];
    _exchg            -= _pieceVal[pc];
    pc                ^= ((m & Move::Promote) >> (Move ::PromotionShift -
                                                   Piece::PromotionShift   ));

    // captured piece including EMP
    Piece ::Piece  cp  = _board[to];
    Move::Move     mv  = Move::setValue(cp, m);
    _bbord[cp]        ^= Bitboard::Square[to];
    _piece[_last]     &= Bitboard::Invert[to];
    _keyBoard         ^= Zobrist::position[to][cp];
    _exchg            -= _pieceVal[cp];
    cp                &= Piece::Mask;

    // square moved from 
    _board[fm]         = Piece::EMP;

    // square moved to  
    _board[to]         = static_cast<unsigned char>(pc);
    _bbord[pc]        ^= Bitboard::Square[to];
    _piece[_next]     ^= Bitboard::Square[to];
    _keyBoard         ^= Zobrist::position[to][pc];
    _exchg            += _pieceVal[pc];

    // hands
    int &d             = _hands[_next][cp];
    _keyHands         += Zobrist::hands[_next][cp];
    _exchg            -= _handsVal[d++][_next][cp];
    d                 &= EMPHandsMask;
    _exchg            += _handsVal[d  ][_next][cp];

    // update occupied bitboard
    _ocupd             = _piece[Color::Black] ^ _piece[Color::White];

    // update empty bitboard
    _empty             = ~_ocupd;

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // flip the turn
    _last              = _next;
    _next              = Color::flip(_next);

    // number of moves
    ++_numMoves;

    // square of OU
    if (_bbord[Piece::BOU]) {
        _kingSB = _bbord[Piece::BOU].lsb();
    }
    if (_bbord[Piece::WOU]) {
        _kingSW = _bbord[Piece::WOU].lsb();
    }

    // make check
    makeCheck();

    return mv;
 
}



/**
 * Und the last move, assuming the given move is always legal 
 * @param m move to perform
 */
void Position::undo (const Move::Move &m)
{

    // dropping move
    if (m & Move::Drop) {
        remove(m);
        return;
    }

    // flip the turn
    _last              = _next;
    _next              = Color::flip(_next);

    // suquares
    Square::Square fm  = Move::from(m);
    Square::Square to  = Move::to  (m);

    // moved piece
    Piece::Piece   pc  = _board[to];
    _bbord[pc]        ^= Bitboard::Square[to];
    _piece[_next]     ^= Bitboard::Square[to];
    _keyBoard         ^= Zobrist::position[to][pc];
    _exchg            -= _pieceVal[pc];
    pc                ^= ((m & Move::Promote) >> (Move ::PromotionShift -
                                                   Piece::PromotionShift   ));

    // captured piece including EMP
    Piece::Piece   cp  = Move::value(m);

    // square moved from
    _board[fm]         = static_cast<unsigned char>(pc);
    _bbord[pc]        ^= Bitboard::Square[fm];
    _piece[_next]     ^= Bitboard::Square[fm];
    _keyBoard         ^= Zobrist::position[fm][pc];
    _exchg            += _pieceVal[pc];

    // square moved to
    _board[to]         = static_cast<unsigned char>(cp);
    _bbord[cp]        ^= Bitboard::Square[to];
    _piece[_last]     ^= (cp == Piece::EMP ? Bitboard::Zero : Bitboard::Square[to]);
    _keyBoard         ^= Zobrist::position[to][cp];
    _exchg            += _pieceVal[cp];
    cp                &= Piece::Mask;

    // hands
    int &d             = _hands[_next][cp];
    _keyHands         -= Zobrist::hands[_next][cp];
    _exchg            -= _handsVal[d--][_next][cp];
    d                 &= EMPHandsMask;
    _exchg            += _handsVal[d  ][_next][cp];


    // update occupied bitboard
    _ocupd             = _piece[Color::Black] ^ _piece[Color::White];

    // update empty bitboard
    _empty             = ~_ocupd;

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // number of moves
    --_numMoves;

    // square of OU
    if (_bbord[Piece::BOU]) {
        _kingSB = _bbord[Piece::BOU].lsb();
    }
    if (_bbord[Piece::WOU]) {
        _kingSW = _bbord[Piece::WOU].lsb();
    }

    // make check
    makeCheck();
 
}



/**
 * Make a move (CSA protocol)
 * @param m move to perform
 * @return the perfomed move
 */
Move::Move Position::move (const CSAMove &mv)
{

    const std::string &     m = mv.move;

    // piece
    Piece::Piece   pc = Piece::csaToPiece(m.substr(5,2));

    // move to
    Square::Square to = Square::fileAll[m[3] - '1'] | Square::rankAll[m[4] - '1'];

    // check if dropping
    if (m.substr(1,2) == "00") {
        _lastmove = Move::drop(pc, to);
        return drop(_lastmove);;
    }

    // move from
    Square::Square fm = Square::fileAll[m[1] - '1'] | Square::rankAll[m[2] - '1'];

    // check if promoting
    if (pc != (_board[fm] & Piece::Neutral)) {
        return move(Move::promote(fm, to));
    }

    return move(Move::move(fm, to));

}



/**
 * Last move
 * @return the last move
 */
Move::Move Position::lastMove (void) const
{

    return _lastmove;

}



/**
 * Uchifuzume check
 * @return true if the last move is drroping FU, false not
 */
bool Position::uchifuzume (void) const
{

    return ((_lastmove & 0xff80) == (Move::Drop | (Piece::FU << 7)));

}



/**
 * Check if OU will be captured
 * @param m move to perform
 * @return true if OU is in the dust, false otherwise.
 */
bool Position::dusty (const Move::Move &m) const
{

    Square::Square to = Move::to(m);
    return ((_last == Color::Black ? _kingSB : _kingSW) == to);

}



/**
 * Check if black will capture WOU
 * @param m move to perform
 * @return true if WOU is in the dust, false otherwise.
 */
bool Position::dustyB (const Move::Move &m) const
{

    Square::Square to = Move::to(m);
    return (_kingSW == to);

}



/**
 * Check if white will captur BOU
 * @param m move to perform
 * @return true if BOU is in the dust, false otherwise.
 */
bool Position::dustyW (const Move::Move &m) const
{

    Square::Square to = Move::to(m);
    return ((_last == Color::Black ? _kingSB : _kingSW) == to);

}



/**
 * Print position of the pieces
 * 
 */
void Position::show (void) const
{

    std::cout << *this << std::endl;

}



/**
 * Print the move out
 * @param m the move to print
 */
void Position::show (Move::Move m) const
{

    std::cout << string(m);

}



/**
 * Make a move string
 * @param m the move
 */
std::string Position::string (Move::Move m) const
{

    Piece::Piece            pc;
    int                     ff, fr, tf, tr;

    if (m & Move::Drop) {
        // dropping move
        ff = 0;
        fr = 0;
        pc = Move::from(m);
        pc = Piece::polar(pc, _next);
    } else {
        // ordinary move
        ff = Square::toFile(Move::from(m)) + 1;
        fr = Square::toRank(Move::from(m)) + 1;
        pc = _board[from(m)];
    }

    tf = Square::toFile(Move::to(m)) + 1;
    tr = Square::toRank(Move::to(m)) + 1;
    if (m & Move::Promote) {
        pc |= Piece::Promoted;
    }

    std::stringstream ss;

    // turn
    if (_next == Color::Black) {
        ss << '+';
    } else {
        ss << '-';
    }

    ss << ff << fr << tf << tr << Piece::vst[pc];

    return ss.str();

}



/**
 * Evaluate current position
 * @return evaluation
 */
Evaluation::Eval Position::eval (void)
{

    return _exchg;

}



/**
 * Conversion to number of moves
 * @return number of moves so far
 */
Position::operator int() const
{

    return (_numMoves);

}



/**
 * Copy operation avoiding copying context cache
 * @param rhs value to assign
 * @return assigned value
 */
Position & Position::operator= (const Position &rhs)
{

    // avoid assign to itself
    if (this == &rhs) {
        return *this;
    }

    _keyBoard   = rhs._keyBoard;
    _keyHands   = rhs._keyHands;
    _key        = rhs._key;
    _kingSB     = rhs._kingSB;
    _kingSW     = rhs._kingSW;
    _ocupd      = rhs._ocupd;
    _empty      = rhs._empty;
    _exchg      = rhs._exchg;
    _last       = rhs._last;
    _next       = rhs._next;
    _numMoves   = rhs._numMoves;


    // copy the board
    memcpy(_board, rhs._board, sizeof(_board));

    // copy pieces in the hands
    for (auto p : Piece::hand) {
        _hands[Color::Black][p] = rhs._hands[Color::Black][p];
        _hands[Color::White][p] = rhs._hands[Color::White][p];
    }
    _hands[Color::Black][Piece::EMP] = 0;
    _hands[Color::White][Piece::EMP] = 0;

    // copy the bitboards
    memcpy(_bbord, rhs._bbord, sizeof(_bbord));

    // copy the occupied square
    _piece[Color::Black] = rhs._piece[Color::Black];
    _piece[Color::White] = rhs._piece[Color::White];

    // copy doesn't affect TLS cache.
    // we don't have to invoke makeCheck() here.
#ifndef USE_THREADLOCALSTORAGE
    makeCheck();
#endif

    return *this;

}



/**
 * Print position of the pieces
 * @param os  output stream
 * @param rhs position to print out
 * @return output stream
 */
std::ostream & operator<< (std::ostream &os, const Position &rhs)
{

    // key and exchange
    os << std::hex;
    os << Terminal::BLU
       << "Hash    : 0x" << rhs._key      << " "
       << "Hand: 0x"     << rhs._keyHands << " "
       << "Board: 0x"    << rhs._keyBoard << std::endl;
    os << std::dec;
    os << "Exchange: "   << rhs._exchg    << std::endl;

    // pieces in white's hands
    auto printHand = [&os, &rhs] (Color::Color c) {
        bool any   = false;
        os << "Hand    : ";
        for (auto p : Piece::hand) {
            int idx  = rhs._hands[c][p];
            if (idx == 0) {
                continue;
            }
            os << Piece::vst[p]
               << std::dec << std::setw(2) << std::setfill('0') << idx << " ";
            any = true;
        }

        return any;

    };

    if (printHand(Color::White)) {
        os << std::endl;
    } else {
        os << "None" << std::endl;
    }

    // pieces on the board
    os << "   9  8  7  6  5  4  3  2  1  " << std::endl
       << "+----------------------------+" << std::endl;
    for (auto y : Square::rankAll) {
        os << "|";
        for (auto x : Square::fileRev) {
            Square::Square s = y | x;
            Piece ::Piece  k = rhs._board[s];
            Color ::Color  c = Piece::color(k);
            if (c == Color::Black) {
                os << Terminal::GRN << " " << Piece::vst[k];
            } else {
                os << Terminal::YLW << " " << Piece::vst[k];
            }
            os << Terminal::BLU;
        }
        os << " | " << std::dec << std::setw(1) << y + 1 << std::endl;
    }
    os << "+----------------------------+" << std::endl;

    // pieces in black's hands
    if (printHand(Color::Black)) {
        os << std::endl;
    } else {
        os << "None" << std::endl;
    }

    os << Terminal::WHT;

    return os;

}



/**
 * Make check
 * 
 */
void Position::makeCheck (void)
{

    // make a bitboard for the pieces making a check
    if (_next == Color::Black) {
        _makeCheckB();
    } else {
        _makeCheckW();
    }

    // clear pin
    _pinnd = Bitboard::Fill;

}



/**
 * Number of checks
 * The function simply returns _nchek that depends on the last move
 * generation.
 */
int Position::nchecks (void) const
{

    return _nchek;

}



/**
 * Number of possible moves
 * @return number of moves for Black
 *
 */
int Position::possible (void)
{

    return ((possibleB() - possibleW()));

}



/**
 * Number of possible moves for black
 * @return number of moves for black
 *
 */
int Position::possibleB (void)
{

    // make pin
    _makePinB();

    // check if OU must get out of the check
    if (_nchek && _next == Color::Black) {
        return _numOutB();
    }

    // number of moves
    int num = 0;

    // move the pinned pieces
    num  = _numPinB();

    num += _numBFU();
    num += _numBKY();
    num += _numBKE();
    num += _numBGI();
    num += _numBKI();
    num += _numBUM();
    num += _numBRY();
    num += _numBKA();
    num += _numBHI();
    num += _numBOU();

    return num;

}



/**
 * Number of possible moves for black
 * @return number of moves for black
 *
 */
int Position::possibleW (void)
{

    // make pin
    _makePinW();

    // check if OU must get out of the check
    if (_nchek && _next == Color::White) {
        return _numOutW();
    }

    // number of moves
    int num = 0;

    // move the pinned pieces
    num  = _numPinW();

    num += _numWFU();
    num += _numWKY();
    num += _numWKE();
    num += _numWGI();
    num += _numWKI();
    num += _numWUM();
    num += _numWRY();
    num += _numWKA();
    num += _numWHI();
    num += _numWOU();

    return num;

}




/**
 * Moves for next player
 * @param m array to store moves
 *
 */
void Position::genMove (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        genMoveB(m);
    } else {
        genMoveW(m);
    }

}



/**
 * Moves for black
 * @param m array to store moves
 *
 */
void Position::genMoveB (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    _m.setsz(0);

    // make pin
    _makePinB();

    // check if OU must get out of the check
    if (_nchek) {
        _getOutB(m);
        return;
    }

    // move the pinned pieces
    _movePinB(m);

    _moveBFU(m);
    _moveBKY(m);
    _moveBKE(m);
    _moveBGI(m);
    _moveBKI(m);
    _moveBUM(m);
    _moveBRY(m);
    _moveBKA(m);
    _moveBHI(m);
    _moveBOU(m);

    _dropBFU(_empty, m);
    _dropBKY(_empty, m);
    _dropBKE(_empty, m);
    _dropBOT(_empty, m);

}



/**
 * Moves for white
 * @param m array to store moves
 *
 */
void Position::genMoveW (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    _m.setsz(0);

    // make pin
    _makePinW();

    // check if OU must get out of the check
    if (_nchek) {
        _getOutW(m);
        return;
    }

    // move the pinned pieces
    _movePinW(m);

    _moveWFU(m);
    _moveWKY(m);
    _moveWKE(m);
    _moveWGI(m);
    _moveWKI(m);
    _moveWUM(m);
    _moveWRY(m);
    _moveWKA(m);
    _moveWHI(m);
    _moveWOU(m);

    _dropWFU(_empty, m);
    _dropWKY(_empty, m);
    _dropWKE(_empty, m);
    _dropWOT(_empty, m);

}



/**
 * Minor moves
 * @param m array to store moves
 */
void Position::minorMove (Array<Move::Move, Move::Max> &m)
{

    // copy array for minor moves
    for (auto move : _m) {
        m.add(move);
    }

}



/**
 * Moves for next player (fast)
 * @param m array to store moves
 */
void Position::genFast (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        genFastB(m);
    } else {
        genFastW(m);
    }

}



/**
 * Moves for black (fast)
 * @param m array to store moves
 */
void Position::genFastB (Array<Move::Move, Move::Max> &m)
{

    // check if OU must get out of the check
    if (_nchek) {
        _fstOutB(m);
        return;
    }

    auto mask  = ~_piece[Color::Black]; 

    _fastBFU(mask, m);
    _fastBKY(mask, m);
    _fastBKE(mask, m);
    _fastBGI(mask, m);
    _fastBKI(mask, m);
    _fastBUM(mask, m);
    _fastBRY(mask, m);
    _fastBKA(mask, m);
    _fastBHI(mask, m);
    _moveBOU(mask, m);

    _dropBFU(_empty, m);
    _dropBKY(_empty, m);
    _dropBKE(_empty, m);
    _dropBOT(_empty, m);

}



/**
 * Moves for white (fast)
 * @param m array to store moves
 */
void Position::genFastW (Array<Move::Move, Move::Max> &m)
{

    // check if OU must get out of the check
    if (_nchek) {
        _fstOutW(m);
        return;
    }

    auto mask  = ~_piece[Color::White]; 

    _fastWFU(mask, m);
    _fastWKY(mask, m);
    _fastWKE(mask, m);
    _fastWGI(mask, m);
    _fastWKI(mask, m);
    _fastWUM(mask, m);
    _fastWRY(mask, m);
    _fastWKA(mask, m);
    _fastWHI(mask, m);
    _moveWOU(mask, m);

    _dropWFU(_empty, m);
    _dropWKY(_empty, m);
    _dropWKE(_empty, m);
    _dropWOT(_empty, m);

}



/**
 * Capturing moves for next player
 * @param m array to store moves
 */
void Position::genCapt (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        genCaptB(m);
    } else {
        genCaptW(m);
    }

}



/**
 * Capturing moves for black
 * @param m array to store moves
 */
void Position::genCaptB (Array<Move::Move, Move::Max> &m)
{

    // check if OU must get out of the check
    if (_nchek) {
        _fstOutB(m);
        return;
    }

    auto mask = _piece[Color::White];

    _fastBFU(mask, m);
    _fastBKY(mask, m);
    _fastBKE(mask, m);
    _fastBGI(mask, m);
    _fastBKI(mask, m);
    _fastBUM(mask, m);
    _fastBRY(mask, m);
    _fastBKA(mask, m);
    _fastBHI(mask, m);
    _moveBOU(mask, m);

}



/**
 * Capturing moves for white
 * @param m array to store moves
 */
void Position::genCaptW (Array<Move::Move, Move::Max> &m)
{

    // check if OU must get out of the check
    if (_nchek) {
        _fstOutW(m);
        return;
    }

    auto mask = _piece[Color::Black];

    _fastWFU(mask, m);
    _fastWKY(mask, m);
    _fastWKE(mask, m);
    _fastWGI(mask, m);
    _fastWKI(mask, m);
    _fastWUM(mask, m);
    _fastWRY(mask, m);
    _fastWKA(mask, m);
    _fastWHI(mask, m);
    _moveWOU(mask, m);

}



/**
 * Moves giving check for next player
 * @param m array to store moves
 *
 */
void Position::genChck (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        genChckB(m);
    } else {
        genChckW(m);
    }

}



/**
 * Moves to give check for black
 * @param m array to store moves
 */
void Position::genChckB (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    _m.setsz(0);

    // make pin
    _makePinB();

    // move the pinned pieces
    _chckPinB(m);

    auto mask = ~_piece[Color::Black];
    _chckBFU(m);
    _chckBKY(_bbord[Piece::BKY] & _pinnd, mask, m);
    _chckBKE(_bbord[Piece::BKE] & _pinnd, mask, m);
    _chckBGI(_bbord[Piece::BGI] & _pinnd, mask, m);
    _chckBKI(mask, m);
    _chckBUM(_bbord[Piece::BUM] & _pinnd, mask, m);
    _chckBRY(_bbord[Piece::BRY] & _pinnd, mask, m);
    _chckBKA(_bbord[Piece::BKA] & _pinnd, mask, m);
    _chckBHI(_bbord[Piece::BHI] & _pinnd, mask, m);

    _dropBFU(Effect::AD(_kingSW, Piece::WFU) & _empty, m);
    _dropBKY(Effect::KW(_kingSW, _ocupd)     & _empty, m);
    _dropBKE(Effect::AD(_kingSW, Piece::WKE) & _empty, m);
    _dchkBOT(m);

    // discovered check
    _discChckB(m);

}



/**
 * Moves to give check for white
 * @param m array to store moves
 */
void Position::genChckW (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    _m.setsz(0);

    // make pin
    _makePinW();

    // move the pinned pieces
    _chckPinW(m);

    auto mask = ~_piece[Color::White];
    _chckWFU(m);
    _chckWKY(_bbord[Piece::WKY] & _pinnd, mask, m);
    _chckWKE(_bbord[Piece::WKE] & _pinnd, mask, m);
    _chckWGI(_bbord[Piece::WGI] & _pinnd, mask, m);
    _chckWKI(mask, m);
    _chckWUM(_bbord[Piece::WUM] & _pinnd, mask, m);
    _chckWRY(_bbord[Piece::WRY] & _pinnd, mask, m);
    _chckWKA(_bbord[Piece::WKA] & _pinnd, mask, m);
    _chckWHI(_bbord[Piece::WHI] & _pinnd, mask, m);

    _dropWFU(Effect::AD(_kingSB, Piece::BFU) & _empty, m);
    _dropWKY(Effect::KB(_kingSB, _ocupd)     & _empty, m);
    _dropWKE(Effect::AD(_kingSB, Piece::BKE) & _empty, m);
    _dchkWOT(m);

    // discovered check
    _discChckW(m);

}



/**
 * Moves giving check fast
 * @param m array to store moves
 *
 */
void Position::genCFst (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        genCFstB(m);
    } else {
        genCFstW(m);
    }

}



/**
 * Moves to give check fast for black
 * @param m array to store moves
 */
void Position::genCFstB (Array<Move::Move, Move::Max> &m)
{

    // check if OU is in check
    if (_nchek) {
        return;
    }

    _cfstBFU(m);
    _chckBKY(_bbord[Piece::BKY], _empty, m);
    _chckBKE(_bbord[Piece::BKE], _empty, m);
    _chckBGI(_bbord[Piece::BGI], _empty, m);
    _chckBKI(_empty, m);
    _chckBUM(_bbord[Piece::BUM], _empty, m);
    _chckBRY(_bbord[Piece::BRY], _empty, m);
    _cfstBKA(m);
    _cfstBHI(m);

    _dropBFU(Effect::AD(_kingSW, Piece::WFU) & _empty, m);
    _dropBKY(Effect::KW(_kingSW, _ocupd)     & _empty, m);
    _dropBKE(Effect::AD(_kingSW, Piece::WKE) & _empty, m);
    _dchkBOT(m);

    // discovered check
    _discCFstB(m);

}



/**
 * Moves to give check fast for white
 * @param m array to store moves
 */
void Position::genCFstW (Array<Move::Move, Move::Max> &m)
{

    // check if OU is in check
    if (_nchek) {
        return;
    }

    _cfstWFU(m);
    _chckWKY(_bbord[Piece::WKY], _empty, m);
    _chckWKE(_bbord[Piece::WKE], _empty, m);
    _chckWGI(_bbord[Piece::WGI], _empty, m);
    _chckWKI(_empty, m);
    _chckWUM(_bbord[Piece::WUM], _empty, m);
    _chckWRY(_bbord[Piece::WRY], _empty, m);
    _cfstWKA(m);
    _cfstWHI(m);

    _dropWFU(Effect::AD(_kingSB, Piece::BFU) & _empty, m);
    _dropWKY(Effect::KB(_kingSB, _ocupd)     & _empty, m);
    _dropWKE(Effect::AD(_kingSB, Piece::BKE) & _empty, m);
    _dchkWOT(m);

    // discovered check
    _discCFstW(m);

}



/**
 * Capture the checking piece
 * @param m array to store moves
 */
void Position::delChck (Array<Move::Move, Move::Max> &m)
{

    if (_next == Color::Black) {
        delChckB(m);
    } else {
        delChckW(m);
    }

}



/**
 * Capture the checking piece for black
 * @param m array to store moves
 */
void Position::delChckB (Array<Move::Move, Move::Max> &m)
{

    // target square
    auto sq   = _chckp.lsb();

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSB];
    _fastToB(mask, sq, m);

    // capture by OU itself
    if ((Effect::OC(_kingSB) & _chckp)) {
        if (! _chkEffectW(sq)) {
            m.add(Move::move(_kingSB, sq));
        }
    }

}



/**
 * Capture the checking piece for white
 * @param m array to store moves
 */
void Position::delChckW (Array<Move::Move, Move::Max> &m)
{

    // target square
    auto sq   = _chckp.lsb();

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSW];
    _fastToW(mask, sq, m);

    // capture by OU itself
    if ((Effect::OC(_kingSW) & _chckp)) {
        if (! _chkEffectB(sq)) {
            m.add(Move::move(_kingSW, sq));
        }
    }

}



/**
 * Fundamental function to generate nomal moves
 * @param to   bitmap for squares moved to
 * @param from square moved from
 * @param m    array to store moves
 */
inline void _normlMove (Bitboard &to, Square::Square sq,
                                            Array<Move::Move, Move::Max> &m)
{
    while (to) {
        auto s = to.pick();
        m.add(Move::move(sq, s));
    }
}



/**
 * Fundamental function to generate promoting moves
 * @param to   bitmap for squares moved to
 * @param from square moved from
 * @param m    array to store moves
 */
inline void _promtMove (Bitboard &to, Square::Square sq,
                                            Array<Move::Move, Move::Max> &m)
{
    while (to) {
        auto s = to.pick();
        m.add(Move::promote(sq, s));
    }
}


/**
 * Fundamental function to generate normal and promoting moves
 * @param to   bitmap for squares moved to
 * @param from square moved from
 * @param m    array to store moves
 */
inline void _pandnMove (Bitboard &to, Square::Square sq,
                                            Array<Move::Move, Move::Max> &m)
{
    while (to) {
        auto s = to.pick();
        m.add(Move::move   (sq, s));
        m.add(Move::promote(sq, s));
    }
}



/**
 * Fundamental function to generate minor moves
 * @param to   bitmap for squares moved to
 * @param from square moved from
 * @param m    array to store moves
 */
inline void Position::_minorMove (Bitboard &to, Square::Square sq,
                                            Array<Move::Move, Move::Max> &m)
{
    while (to) {
        auto s = to.pick();
         m.add(Move::promote(sq, s));
        _m.add(Move::move   (sq, s));
    }
}



/**
 * Cache minor moves
 * @param to   bitmap for squares moved to
 * @param from square moved from
 */
inline void Position::_cacheMove (Bitboard &to, Square::Square sq)
{
    while (to) {
        auto s = to.pick();
        _m.add(Move::move(sq, s));
    }
}



/**
 * undamental function to generate drop moves
 * @param pc   piece to drop
 * @param to   bitmsquare moved from
 * @param m    array to store moves
 */
inline void _dropMove (Piece::Piece pc, Bitboard &to, Array<Move::Move, Move::Max> &m)
{
    while (to) {
        auto s = to.pick();
        m.add(Move::drop(pc, s));
    }
}



/**
 * Number of possible moves for BFU
 * @return number of moves
 */
int Position::_numBFU (void)
{

    return (((_bbord[Piece::BFU] & _pinnd) >> 1) & (~_piece[Color::Black])).popcnt();

}



/**
 * Number of possible moves for WFU
 * @return number of moves
 */
int Position::_numWFU (void)
{

    return (((_bbord[Piece::WFU] & _pinnd) << 1) & (~_piece[Color::White])).popcnt();

}



/**
 * Move BFU
 * @param m array to store moves
 */
void Position::_moveBFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::BFU] & _pinnd) >> 1) & (~_piece[Color::Black]);
    auto en = ef & BlackPNMask;
    auto eo = ef & NFBFU;
    auto ep = ef & BlackPRMask;
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (eo) {
        auto sq = eo.pick();
        _m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::DWARD, sq));
    }

}



/**
 * Move BFU (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto ef = (_bbord[Piece::BFU] >> 1) & mask;
    auto en = ef & BlackPNMask;
    auto ep = ef & BlackPRMask;
    while (en) {
        auto sq = en.pick();
        m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
        m.add(Move::promote(sq + Square::DWARD, sq));
    }

}



/**
 * Move BFU to give check
 * @param m array to store moves
 */
void Position::_chckBFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::BFU] & _pinnd) >> 1) & (~_piece[Color::Black]);
    auto en = ef & BlackPNMask & Effect::AD(_kingSW, Piece::WFU);
    auto eo = ef & NFBFU       & Effect::AD(_kingSW, Piece::WFU);
    auto ep = ef & BlackPRMask & Effect::AD(_kingSW, Piece::WKI);
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (eo) {
        auto sq = eo.pick();
        _m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::DWARD, sq));
    }

}



/**
 * Move BFU to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstBFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = (_bbord[Piece::BFU] >> 1) & _empty;
    auto en = ef & BlackPNMask & Effect::AD(_kingSW, Piece::WFU);
    auto ep = ef & BlackPRMask & Effect::AD(_kingSW, Piece::WKI);
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::DWARD, sq));
    }

}





/**
 * Move WFU
 * @param m array to store moves
 */
void Position::_moveWFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::WFU] & _pinnd) << 1) & (~_piece[Color::White]);
    auto en = ef & WhitePNMask;
    auto eo = ef & NFWFU;
    auto ep = ef & WhitePRMask;
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (eo) {
        auto sq = eo.pick();
        _m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Move WFU (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto ef = (_bbord[Piece::WFU] << 1) & mask;
    auto en = ef & WhitePNMask;
    auto ep = ef & WhitePRMask;
    while (en) {
        auto sq = en.pick();
        m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
        m.add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Move WFU to give check
 * @param m array to store moves
 */
void Position::_chckWFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::WFU] & _pinnd) << 1) & (~_piece[Color::White]);
    auto en = ef & WhitePNMask & Effect::AD(_kingSB, Piece::BFU);
    auto eo = ef & NFWFU       & Effect::AD(_kingSB, Piece::BFU);
    auto ep = ef & WhitePRMask & Effect::AD(_kingSB, Piece::BKI);
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (eo) {
        auto sq = eo.pick();
        _m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Move WFU to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstWFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = (_bbord[Piece::WFU] << 1) & _empty;
    auto en = ef & WhitePNMask & Effect::AD(_kingSB, Piece::BFU);
    auto ep = ef & WhitePRMask & Effect::AD(_kingSB, Piece::BKI);
    while (en) {
        auto sq = en.pick();
         m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (ep) {
        auto sq = ep.pick();
         m.add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Number of possible moves for BKY
 * @return number of moves
 */
int Position::_numBKY (void)
{

    auto pp  = _bbord[Piece::BKY] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::KB(sq, _ocupd) & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WKY
 * @return number of moves
 */
int Position::_numWKY (void)
{

    auto pp  = _bbord[Piece::WKY] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::KW(sq, _ocupd) & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BKY
 * @param m array to store moves
 */
void Position::_moveBKY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KB(sq, _ocupd) & (~_piece[Color::Black]);
        auto en = ef & NFBKY;
        auto ep = ef & BlackPRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKY (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKY];

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KB(sq, _ocupd) & mask;
        auto en = ef & NFBKY;
        auto ep = ef & BlackPRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKY to give check
 * @param p  bitboard of BKY to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBKY (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KB(sq, _ocupd) & em;
        auto en = ef & NFBKY       & Effect::KW(_kingSW, _ocupd);
        auto ep = ef & BlackPRMask & Effect::AD(_kingSW, Piece::WKI);
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKY
 * @param m array to store moves
 */
void Position::_moveWKY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KW(sq, _ocupd) & (~_piece[Color::White]);
        auto en = ef & NFWKY;
        auto ep = ef & WhitePRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKY (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKY];

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KW(sq, _ocupd) & mask;
        auto en = ef & NFWKY;
        auto ep = ef & WhitePRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKY to give check
 * @param p  bitboard of WKY to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWKY (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KW(sq, _ocupd) & em;
        auto en = ef & NFWKY       & Effect::KB(_kingSB, _ocupd);
        auto ep = ef & WhitePRMask & Effect::AD(_kingSB, Piece::BKI);
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Number of possible moves for BKE
 * @return number of moves
 */
int Position::_numBKE (void)
{

    auto pp  = _bbord[Piece::BKE] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::BKE) & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WKE
 * @return number of moves
 */
int Position::_numWKE (void)
{

    auto pp  = _bbord[Piece::WKE] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::WKE) & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BKE
 * @param m array to store moves
 */
void Position::_moveBKE (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKE] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::BKE) & (~_piece[Color::Black]);
        auto en = ef & NFBKE;
        auto ep = ef & BlackPRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKE (fast)
 * @param m array to store moves
 */
void Position::_fastBKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKE];

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::BKE) & mask;
        auto en = ef & NFBKE;
        auto ep = ef & BlackPRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKE to give check
 * @param p  bitboard of BKE to move
 * @param em effect mask
 * @param m array to store moves
 */
void Position::_chckBKE (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::BKE) & em;
        auto en = ef & NFBKE       & Effect::AD(_kingSW, Piece::WKE);
        auto ep = ef & BlackPRMask & Effect::AD(_kingSW, Piece::WKI);
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKE
 * @param m array to store moves
 */
void Position::_moveWKE (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKE] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WKE) & (~_piece[Color::White]);
        auto en = ef & NFWKE;
        auto ep = ef & WhitePRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKE
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKE];

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WKE) & mask;
        auto en = ef & NFWKE;
        auto ep = ef & WhitePRMask;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKE to give check
 * @param p  bitboard of WKE to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWKE (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WKE) & em;
        auto en = ef & NFWKE       & Effect::AD(_kingSB, Piece::BKE);
        auto ep = ef & WhitePRMask & Effect::AD(_kingSB, Piece::BKI);
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Number of possible moves for BGI
 * @return number of moves
 */
int Position::_numBGI (void)
{

    auto pp  = _bbord[Piece::BGI] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::BGI) & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WGI
 * @return number of moves
 */
int Position::_numWGI (void)
{

    auto pp  = _bbord[Piece::WGI] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::WGI) & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BGI
 * @param m array to store moves
 */
void Position::_moveBGI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BGI] & _pinnd;
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBGI(~_piece[Color::Black], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtBGI(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BGI (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBGI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BGI];
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBGI(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtBGI(mask, sq, m);
    }

}



/**
 * Move BGI to give check
 * @param p  bitboard of BGI to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBGI (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p  & BlackPNMask;
    auto pp = p  & BlackPRMask;
    auto mg = em & Effect::AD(_kingSW, Piece::WGI);
    auto mk = em & Effect::AD(_kingSW, Piece::WKI);
    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::AD(sq, Piece::BGI);
        auto en = ef & mg;                     // np -> (np | pr) without promotion
        auto ep = ef & BlackPRMask & mk;       // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::BGI);
        auto en = ef & mg;                      // pr -> (np | pr) without promotion
        auto ep = ef & mk;                      // pr -> (np | pr) with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WGI
 * @param m array to store moves
 */
void Position::_moveWGI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WGI] & _pinnd;
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveWGI(~_piece[Color::White], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtWGI(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WGI (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWGI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WGI];
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveWGI(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtWGI(mask, sq, m);
    }

}



/**
 * Move WGI to give check
 * @param p  bitboard of WGI to move
 * @param em effect mask
 * @param m array to store moves
 */
void Position::_chckWGI (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p  & WhitePNMask;
    auto pp = p  & WhitePRMask;
    auto mg = em & Effect::AD(_kingSB, Piece::BGI);
    auto mk = em & Effect::AD(_kingSB, Piece::BKI);
    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::AD(sq, Piece::WGI);
        auto en = ef & mg;                      // np -> (np | pr) without promotion
        auto ep = ef & WhitePRMask & mk;        // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WGI);
        auto en = ef & mg;
        auto ep = ef & mk;
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BGI with mask
 * @param mask mask of effect
 * @param sq   square of BGI
 * @param m array to store moves
 */
void Position::_moveBGI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::AD(sq, Piece::BGI) & mask;
    auto pr  = ef & BlackPRMask;
         ef &= BlackPNMask;
    _normlMove(ef, sq, m);
    _pandnMove(pr, sq, m);

}



/**
 * Move WGI with mask
 * @param mask mask of effect
 * @param sq   square of WGI
 * @param m array to store moves
 */
void Position::_moveWGI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::AD(sq, Piece::WGI) & mask;
    auto pr  = ef & WhitePRMask;
         ef &= WhitePNMask;
    _normlMove(ef, sq, m);
    _pandnMove(pr, sq, m);

}



/**
 * Move BGI with mask (can promote at any square)
 * @param mask mask of effect
 * @param sq   square of BGI
 * @param m array to store moves
 */
void Position::_prmtBGI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::BGI) & mask;
    _pandnMove(ef, sq, m);

}



/**
 * Move WGI with mask (can promote at any square)
 * @param mask mask of effect
 * @param sq   square of WGI
 * @param m array to store moves
 */
void Position::_prmtWGI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::WGI) & mask;
    _pandnMove(ef, sq, m);

}



/**
 * Number of possible moves for BKI, BTO, BNY, BNK and BNG
 * @return number of moves
 */
int Position::_numBKI (void)
{

    auto pp  = _bbord[Piece::BKI] | _bbord[Piece::BTO] |
               _bbord[Piece::BNY] | _bbord[Piece::BNK] | _bbord[Piece::BNG];
    int  num = 0;

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::BKI) & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WKI, WTO, WNY, WNK and WNG
 * @return number of moves
 */
int Position::_numWKI (void)
{

    auto pp  = _bbord[Piece::WKI] | _bbord[Piece::WTO] |
               _bbord[Piece::WNY] | _bbord[Piece::WNK] | _bbord[Piece::WNG];
    int  num = 0;

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        num += (Effect::AD(sq, Piece::WKI) & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BKI, BTO, BNY, BNK and BNG
 * @param m array to store moves
 */
void Position::_moveBKI (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKI] | _bbord[Piece::BTO] |
              _bbord[Piece::BNY] | _bbord[Piece::BNK] | _bbord[Piece::BNG];

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        _moveBKI(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BKI, BTO, BNY, BNK and BNG (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBKI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKI] | _bbord[Piece::BTO] |
              _bbord[Piece::BNY] | _bbord[Piece::BNK] | _bbord[Piece::BNG];

    while (pp) {
        auto sq = pp.pick();
        _moveBKI(mask, sq, m);
    }

}



/**
 * Move BKI, BTO, BNY, BNK and BNG to give check
 * @param em effect mask
 * @param m array to store moves
 */
void Position::_chckBKI (const Bitboard &em, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKI] | _bbord[Piece::BTO] |
              _bbord[Piece::BNY] | _bbord[Piece::BNK] | _bbord[Piece::BNG];

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        auto mk = em & Effect::AD(_kingSW, Piece::WKI);
        _moveBKI(mk, sq, m);
    }

}



/**
 * Move WKI, WTO, WNY, WNK and WNG
 * @param m array to store moves
 */
void Position::_moveWKI (Array<Move::Move, Move::Max> &m)
{

    auto pp  = _bbord[Piece::WKI] | _bbord[Piece::WTO] |
               _bbord[Piece::WNY] | _bbord[Piece::WNK] | _bbord[Piece::WNG];

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        _moveWKI(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WKI, WTO, WNY, WNK and WNG (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp  = _bbord[Piece::WKI] | _bbord[Piece::WTO] |
               _bbord[Piece::WNY] | _bbord[Piece::WNK] | _bbord[Piece::WNG];

    while (pp) {
        auto sq = pp.pick();
        _moveWKI(mask, sq, m);
    }

}



/**
 * Move WKI, WTO, WNY, WNK and WNG to give check
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWKI (const Bitboard &em, Array<Move::Move, Move::Max> &m)
{

    auto pp  = _bbord[Piece::WKI] | _bbord[Piece::WTO] |
               _bbord[Piece::WNY] | _bbord[Piece::WNK] | _bbord[Piece::WNG];

    pp &= _pinnd;
    while (pp) {
        auto sq = pp.pick();
        auto mk = em & Effect::AD(_kingSB, Piece::BKI);
        _moveWKI(mk, sq, m);
    }

}



/**
 * Move BKI with mask
 * @param mask mask of effect
 * @param sq   square of BKI
 * @param m array to store moves
 */
void Position::_moveBKI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::BKI) & mask;
    _normlMove(ef, sq, m);

}



/**
 * Move WKI with mask
 * @param mask mask of effect
 * @param sq   square of WKI
 * @param m array to store moves
 */
void Position::_moveWKI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::WKI) & mask;
    _normlMove(ef, sq, m);

}



/**
 * Number of possible moves for BUM
 * @return number of moves
 */
int Position::_numBUM (void)
{

    auto pp  = _bbord[Piece::BUM] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += ((Effect::OC(sq) | Effect::KA(sq, _ocupd))
                                & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WUM
 * @return number of moves
 */
int Position::_numWUM (void)
{

    auto pp  = _bbord[Piece::WUM] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += ((Effect::OC(sq) | Effect::KA(sq, _ocupd))
                                & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BUM
 * @param m array to store moves
 */
void Position::_moveBUM (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BUM] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        _moveUM(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BUM (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBUM (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BUM];

    while (pp) {
        auto sq = pp.pick();
        _moveUM(mask, sq, m);
    }

}



/**
 * Move BUM to give check
 * @param p  bitboard of BUM to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBUM (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto mk = em & (Effect::KA(_kingSW, _ocupd) | Effect::OC(_kingSW));
        _moveUM(mk, sq, m);
    }

}



/**
 * Move WUM
 * @param m array to store moves
 */
void Position::_moveWUM (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WUM] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        _moveUM(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WUM (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWUM (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WUM];

    while (pp) {
        auto sq = pp.pick();
        _moveUM(mask, sq, m);
    }

}



/**
 * Move WUM to give check
 * @param p  bitboard of WUM to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWUM (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto mk = em & (Effect::KA(_kingSB, _ocupd) | Effect::OC(_kingSB));
        _moveUM(mk, sq, m);
    }

}



/**
 * Move UM with mask
 * @param mask mask of effect
 * @param sq   square of UM
 * @param m array to store moves
 */
void Position::_moveUM (const Bitboard &mask, Square::Square sq,
                        Array<Move::Move, Move::Max> &m          )
{

    auto ef = (Effect::OC(sq) | Effect::KA(sq, _ocupd)) & mask;
    _normlMove(ef, sq, m);

}



/**
 * Number of possible moves for BRY
 * @return number of moves
 */
int Position::_numBRY (void)
{

    auto pp  = _bbord[Piece::BRY] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += ((Effect::OC(sq) | Effect::HI(sq, _ocupd))
                                & (~_piece[Color::Black])).popcnt();
    }

    return num;

}



/**
 * Number of possible moves for WRY
 * @return number of moves
 */
int Position::_numWRY (void)
{

    auto pp  = _bbord[Piece::WRY] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += ((Effect::OC(sq) | Effect::HI(sq, _ocupd))
                                & (~_piece[Color::White])).popcnt();
    }

    return num;

}



/**
 * Move BRY
 * @param m array to store moves
 */
void Position::_moveBRY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BRY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        _moveRY(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BRY (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBRY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BRY];

    while (pp) {
        auto sq = pp.pick();
        _moveRY(mask, sq, m);
    }

}



/**
 * Move BRY to give check
 * @param p  bitboard of BRY to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBRY (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto mk = em & (Effect::HI(_kingSW, _ocupd) | Effect::OC(_kingSW));
        _moveRY(mk, sq, m);
    }

}



/**
 * Move WRY
 * @param m array to store moves
 */
void Position::_moveWRY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WRY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        _moveRY(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WRY (fast)
 * @param mask mask of effect
 * @param m    array to store moves
 */
void Position::_fastWRY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WRY];

    while (pp) {
        auto sq = pp.pick();
        _moveRY(mask, sq, m);
    }

}



/**
 * Move WRY to give check
 * @param p  bitboard of WRY to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWRY (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    Bitboard pp(p);

    while (pp) {
        auto sq = pp.pick();
        auto mk = em & (Effect::HI(_kingSB, _ocupd) | Effect::OC(_kingSB));
        _moveRY(mk, sq, m);
    }

}



/**
 * Move RY with mask
 * @param mask mask of effect
 * @param sq   square of RY
 * @param m array to store moves
 */
void Position::_moveRY (const Bitboard &mask, Square::Square sq,
                        Array<Move::Move, Move::Max> &m          )
{

    auto ef = (Effect::OC(sq) | Effect::HI(sq, _ocupd)) & mask;
    _normlMove(ef, sq, m);

}



/**
 * Number of possible moves for BKA
 * @return number of moves
 */
int Position::_numBKA (void)
{

    auto pp  = _bbord[Piece::BKA] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::KA(sq, _ocupd) & (~_piece[Color::Black])).popcnt();
    }

    return num;
}



/**
 * Number of possible moves for WKA
 * @return number of moves
 */
int Position::_numWKA (void)
{

    auto pp  = _bbord[Piece::WKA] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::KA(sq, _ocupd) & (~_piece[Color::White])).popcnt();
    }

    return num;
}



/**
 * Move BKA
 * @param m array to store moves
 */
void Position::_moveBKA (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BKA] & _pinnd;
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBKA(~_piece[Color::Black], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtKA(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BKA (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBKA (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BKA];
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _fastBKA(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _pfastKA(mask, sq, m);
    }

}



/**
 * Move BKA to give check
 * @param p  bitboard of BKA to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBKA (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p  & BlackPNMask;
    auto pp = p  & BlackPRMask;
    auto mk = em & Effect::KA(_kingSW, _ocupd);
    auto mu = mk | (Effect::OC(_kingSW) & em );

    while (pc) {
        auto sq  = pc.pick();
        auto ef  = Effect::KA(sq, _ocupd);
        auto en  = ef & BlackPNMask & mk;   // np -> np
             ef &= BlackPRMask;
        auto eo  = ef & mk;                 // np -> pr without promotion
        auto ep  = ef & mu;                 // np -> pr with promotion
        _normlMove(en, sq, m);
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq  = pp.pick();
        auto ef  = Effect::KA(sq, _ocupd);
        auto eo  = ef & mk;                 // pr -> (np | pr) without promotion
        auto ep  = ef & mu;                 // pr -> (np | pr) with promotion
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKA to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstBKA (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BKA];

    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;
    auto mk = Effect::KA(_kingSW, _ocupd) & _empty;
    auto mu = mk | (Effect::OC(_kingSW)   & _empty);

    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::KA(sq, _ocupd);
        auto en = ef & BlackPNMask & mk;    // np -> np
        auto ep = ef & BlackPRMask & mu;    // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ep = Effect::KA(sq, _ocupd) & mu;
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKA
 * @param m array to store moves
 */
void Position::_moveWKA (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WKA] & _pinnd;
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveWKA(~_piece[Color::White], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtKA(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WKA (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKA (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WKA];
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _fastWKA(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _pfastKA(mask, sq, m);
    }

}



/**
 * Move WKA to give check
 * @param p  bitboard of WKA to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWKA (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p  & WhitePNMask;
    auto pp = p  & WhitePRMask;
    auto mk = em & Effect::KA(_kingSB, _ocupd);
    auto mu = mk | (Effect::OC(_kingSB) & em );

    while (pc) {
        auto sq  = pc.pick();
        auto ef  = Effect::KA(sq, _ocupd);
        auto en  = ef & WhitePNMask & mk;   // np -> np
             ef &= WhitePRMask;
        auto eo  = ef & mk;                 // np -> pr without promotion
        auto ep  = ef & mu;                 // np -> pr with promotion
        _normlMove(en, sq, m);
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq  = pp.pick();
        auto ef  = Effect::KA(sq, _ocupd);
        auto eo  = ef & mk;                 // pr -> (np | pr) without promotion
        auto ep  = ef & mu;                 // pr -> (np | pr) with promotion
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WKA to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstWKA (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WKA];

    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;
    auto mk = Effect::KA(_kingSB, _ocupd) & _empty;
    auto mu = mk | (Effect::OC(_kingSB)   & _empty);

    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::KA(sq, _ocupd);
        auto en = ef & WhitePNMask & mk;    // np -> np
        auto ep = ef & WhitePRMask & mu;    // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ep = Effect::KA(sq, _ocupd) & mu;
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BKA with mask 
 * @param mask mask of effect
 * @param sq   square of BKA
 * @param m array to store moves
 */
void Position::_moveBKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::KA(sq, _ocupd) & mask;
    auto pr  = ef & BlackPRMask;
         ef &= BlackPNMask;
    _normlMove(ef, sq, m);
    _minorMove(pr, sq, m);

}



/**
 * Move BKA with mask (fast)
 * @param mask mask of effect
 * @param sq   square of BKA
 * @param m array to store moves
 */
void Position::_fastBKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::KA(sq, _ocupd) & mask;
    auto pr  = ef & BlackPRMask;
         ef &= BlackPNMask;
    _normlMove(ef, sq, m);
    _promtMove(pr, sq, m);

}



/**
 * Move WKA with mask
 * @param mask mask of effect
 * @param sq   square of WKA
 * @param m array to store moves
 */
void Position::_moveWKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::KA(sq, _ocupd) & mask;
    auto pr  = ef & WhitePRMask;
         ef &= WhitePNMask;
    _normlMove(ef, sq, m);
    _minorMove(pr, sq, m);

}



/**
 * Move WKA with mask (fast)
 * @param mask mask of effect
 * @param sq   square of WKA
 * @param m array to store moves
 */
void Position::_fastWKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::KA(sq, _ocupd) & mask;
    auto pr  = ef & WhitePRMask;
         ef &= WhitePNMask;
    _normlMove(ef, sq, m);
    _promtMove(pr, sq, m);

}



/**
 * Move KA with mask (can promote at any square)
 * @param mask mask of effect
 * @param sq   square of KA
 * @param m array to store moves
 */
void Position::_promtKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::KA(sq, _ocupd) & mask;
    _minorMove(ef, sq, m);

}



/**
 * Move KA with mask (can promote at any square, fast)
 * @param mask mask of effect
 * @param sq   square of KA
 * @param m array to store moves
 */
void Position::_pfastKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::KA(sq, _ocupd) & mask;
    _promtMove(ef, sq, m);

}



/**
 * Number of possible moves for BHI
 * @return number of moves
 */
int Position::_numBHI (void)
{

    auto pp  = _bbord[Piece::BHI] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::HI(sq, _ocupd) & (~_piece[Color::Black])).popcnt();
    }

    return num;
}



/**
 * Number of possible moves for WHI
 * @return number of moves
 */
int Position::_numWHI (void)
{

    auto pp  = _bbord[Piece::WHI] & _pinnd;
    int  num = 0;

    while (pp) {
        auto sq = pp.pick();
        num += (Effect::HI(sq, _ocupd) & (~_piece[Color::White])).popcnt();
    }

    return num;
}



/**
 * Move BHI
 * @param m array to store moves
 */
void Position::_moveBHI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BHI] & _pinnd;
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBHI(~_piece[Color::Black], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtHI(~_piece[Color::Black], sq, m);
    }

}



/**
 * Move BHI (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBHI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BHI];
    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _fastBHI(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _pfastHI(mask, sq, m);
    }

}



/**
 * Move BHI to give check
 * @param p  bitboard of BHI to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckBHI (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;
    auto mh = em & Effect::HI(_kingSW, _ocupd);
    auto mr = mh | (Effect::OC(_kingSW) & em );

    while (pc) {
        auto sq  = pc.pick();
        auto ef  = Effect::HI(sq, _ocupd);
        auto en  = ef & BlackPNMask & mh;   // np -> np
             ef &= BlackPRMask;
        auto eo  = ef & mh;                 // np -> pr without promotion
        auto ep  = ef & mr;                 // np -> pr with promotion
        _normlMove(en, sq, m);
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::HI(sq, _ocupd);
        auto eo = ef & mh;                  // pr -> (np | pr) without promotion
        auto ep = ef & mr;                  // pr -> (np | pr) with promotion
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BHI to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstBHI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BHI];

    auto pc = p & BlackPNMask;
    auto pp = p & BlackPRMask;
    auto mh = Effect::HI(_kingSW, _ocupd) & _empty;
    auto mr = mh | (Effect::OC(_kingSW)   & _empty);

    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::HI(sq, _ocupd);
        auto en = ef & BlackPNMask & mh;    // np -> np
        auto ep = ef & BlackPRMask & mr;    // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ep = Effect::HI(sq, _ocupd) & mr;
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WHI
 * @param m array to store moves
 */
void Position::_moveWHI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WHI] & _pinnd;
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveWHI(~_piece[Color::White], sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtHI(~_piece[Color::White], sq, m);
    }

}



/**
 * Move WHI (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWHI (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WHI];
    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;

    while (pc) {
        auto sq = pc.pick();
        _fastWHI(mask, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _pfastHI(mask, sq, m);
    }

}



/**
 * Move WHI to give check
 * @param p  bitboard of WHI to move
 * @param em effect mask
 * @param m  array to store moves
 */
void Position::_chckWHI (const Bitboard &p, const Bitboard &em,
                                            Array<Move::Move, Move::Max> &m)
{

    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;
    auto mh = em & Effect::HI(_kingSB, _ocupd);
    auto mr = mh | (Effect::OC(_kingSB) & em );

    while (pc) {
        auto sq  = pc.pick();
        auto ef  = Effect::HI(sq, _ocupd); 
        auto en  = ef & WhitePNMask & mh;   // np -> np
             ef &= WhitePRMask;
        auto eo  = ef & mh;                 // np -> pr without promotion
        auto ep  = ef & mr;                 // np -> pr with promotion
        _normlMove(en, sq, m);
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq  = pp.pick();
        auto ef  = Effect::HI(sq, _ocupd); 
        auto eo  = ef & mh;                 // pr -> (np | pr) without promotion
        auto ep  = ef & mr;                 // pr -> (np | pr) with promotion
        _cacheMove(eo, sq);
        _promtMove(ep, sq, m);
    }

}



/**
 * Move WHI to give check (fast)
 * @param m array to store moves
 */
void Position::_cfstWHI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::WHI];

    auto pc = p & WhitePNMask;
    auto pp = p & WhitePRMask;
    auto mh = Effect::HI(_kingSB, _ocupd) & _empty;
    auto mr = mh | (Effect::OC(_kingSB)   & _empty);

    while (pc) {
        auto sq = pc.pick();
        auto ef = Effect::HI(sq, _ocupd); 
        auto en = ef & WhitePNMask & mh;    // np -> np
        auto ep = ef & WhitePRMask & mr;    // np -> pr with promotion
        _normlMove(en, sq, m);
        _promtMove(ep, sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        auto ep = Effect::HI(sq, _ocupd) & mr; 
        _promtMove(ep, sq, m);
    }

}



/**
 * Move BHI with mask 
 * @param mask mask of effect
 * @param sq   square of BHI
 * @param m array to store moves
 */
void Position::_moveBHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::HI(sq, _ocupd) & mask;
    auto pr  = ef & BlackPRMask;
         ef &= BlackPNMask;
    _normlMove(ef, sq, m);
    _minorMove(pr, sq, m);

}



/**
 * Move BHI with mask (fast)
 * @param mask mask of effect
 * @param sq   square of BHI
 * @param m array to store moves
 */
void Position::_fastBHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::HI(sq, _ocupd) & mask;
    auto pr  = ef & BlackPRMask;
         ef &= BlackPNMask;
    _normlMove(ef, sq, m);
    _promtMove(pr, sq, m);

}



/**
 * Move WHI with mask
 * @param mask mask of effect
 * @param sq   square of BHI
 * @param m array to store moves
 */
void Position::_moveWHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::HI(sq, _ocupd) & mask;
    auto pr  = ef & WhitePRMask;
         ef &= WhitePNMask;
    _normlMove(ef, sq, m);
    _minorMove(pr, sq, m);

}



/**
 * Move WHI with mask (fast)
 * @param mask mask of effect
 * @param sq   square of WHI
 * @param m array to store moves
 */
void Position::_fastWHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef  = Effect::HI(sq, _ocupd) & mask;
    auto pr  = ef & WhitePRMask;
         ef &= WhitePNMask;
    _normlMove(ef, sq, m);
    _promtMove(pr, sq, m);

}



/**
 * Move HI with mask (can promote at any square)
 * @param mask mask of effect
 * @param sq   square of HI
 * @param m array to store moves
 */
void Position::_promtHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::HI(sq, _ocupd) & mask;
    _minorMove(ef, sq, m);

}



/**
 * Move HI with mask (can promote at any square, fast)
 * @param mask mask of effect
 * @param sq   square of HI
 * @param m array to store moves
 */
void Position::_pfastHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::HI(sq, _ocupd) & mask;
    _promtMove(ef, sq, m);

}



/**
 * Discovered check for black
 * @param m array to store moves
 */
void Position::_discChckB (Array<Move::Move, Move::Max> &m)
{

    // if the opponent has properly responded, his/her king shoudn't
    // be in check here.

    // pin mask
    auto pin = ~_pinnd;

    // attacking pieces
    auto atk = _piece[Color::Black];

    // BKY
    auto bky = _bbord[Piece::BKY];
    while (bky) {
        auto sq = bky.pick();
        auto ef = Effect::KB(sq, _ocupd);
        auto pc = ef & Effect::KW(_kingSW, _ocupd) & atk;
        if (! pc) {
            continue;
        }
        auto s  = pc.lsb();
        auto mk = ~(Effect::HV(s) | _piece[Color::Black]);
        if (_board[s] == Piece::BOU) {
            _moveBOU(mk, m);
            continue;
        } else
        if (pc & pin) {
            auto dr  = Direction::distantDirection(s, _kingSB);
                 mk &= DirectionMap[dr](s);
        }
        _chckFromB(mk, s, m);
    }

    // almost same operation for KA and HI here
    auto mv  = [&] (const Bitboard & (*func)(Square::Square, const Bitboard&),
                    const Bitboard & (*mask)(Square::Square),
                          Bitboard bmp                                         ) {
        while (bmp) {
            auto sq = bmp.pick();
            auto ef = func(sq, _ocupd) & mask(sq);
            auto pc = ef & (func(_kingSW, _ocupd) & mask(_kingSW)) & atk;
            if (! pc) {
                continue;
            }
            auto s  = pc.lsb();
            auto dr = Direction::distantDirection(sq, s);
            auto mk = ~(DirectionMap[dr](s) | _piece[Color::Black]);
            if (_board[s] == Piece::BOU) {
                _moveBOU(mk, m);
                continue;
            } else
            if (pc & pin) {
                dr  = Direction::distantDirection(s, _kingSB);
                mk &= DirectionMap[dr](s);
            }
            _chckFromB(mk, s, m);
        }
    };

    // BKA and BUM
    auto bka = _bbord[Piece::BKA] | _bbord[Piece::BUM];
    mv(Effect::KA, Effect::RS, bka);
    mv(Effect::KA, Effect::FR, bka);

    // BHI and BRY
    auto bhi = _bbord[Piece::BHI] | _bbord[Piece::BRY];
    mv(Effect::HI, Effect::HH, bhi);
    mv(Effect::HI, Effect::HV, bhi);

}



/**
 * Discovered check for black
 * @param m array to store moves
 */
void Position::_discChckW (Array<Move::Move, Move::Max> &m)
{

    // if the opponent has properly responded, his/her king shoudn't
    // be in check here.

    // pin mask
    auto pin = ~_pinnd;

    // attacking pieces
    auto atk = _piece[Color::White];

    // WKY
    auto wky = _bbord[Piece::WKY];
    while (wky) {
        auto sq = wky.pick();
        auto ef = Effect::KW(sq, _ocupd);
        auto pc = ef & Effect::KB(_kingSB, _ocupd) & atk;
        if (! pc) {
            continue;
        }
        auto s  = pc.lsb();
        auto mk = ~(Effect::HV(s) | _piece[Color::White]);
        if (_board[s] == Piece::WOU) {
            _moveWOU(mk, m);
            continue;
        } else
        if (pc & pin) {
            auto dr  = Direction::distantDirection(s, _kingSW);
                 mk &= DirectionMap[dr](s);
        }
        _chckFromW(mk, s, m);
    }

    // almost same operation for KA and HI here
    auto mv  = [&] (const Bitboard & (*func)(Square::Square, const Bitboard&),
                    const Bitboard & (*mask)(Square::Square),
                          Bitboard bmp                                         ) {
        while (bmp) {
            auto sq = bmp.pick();
            auto ef = func(sq, _ocupd) & mask(sq);
            auto pc = ef & (func(_kingSB, _ocupd) & mask(_kingSB)) & atk;
            if (! pc) {
                continue;
            }
            auto s  = pc.lsb();
            auto dr = Direction::distantDirection(sq, s);
            auto mk = ~(DirectionMap[dr](s) | _piece[Color::White]);
            if (_board[s] == Piece::WOU) {
                _moveWOU(mk, m);
                continue;
            } else
            if (pc & pin) {
                dr  = Direction::distantDirection(s, _kingSW);
                mk &= DirectionMap[dr](s);
            }
            _chckFromW(mk, s, m);
        }
    };

    // WKA and WUM
    auto wka = _bbord[Piece::WKA] | _bbord[Piece::WUM];
    mv(Effect::KA, Effect::RS, wka);
    mv(Effect::KA, Effect::FR, wka);

    // WHI and WRY
    auto whi = _bbord[Piece::WHI] | _bbord[Piece::WRY];
    mv(Effect::HI, Effect::HH, whi);
    mv(Effect::HI, Effect::HV, whi);

}



/**
 * Discovered check fast for black
 * @param m array to store moves
 */
void Position::_discCFstB (Array<Move::Move, Move::Max> &m)
{

    // if the opponent has properly responded, his/her king shoudn't
    // be in check here.

    // attacking pieces
    auto atk = _piece[Color::Black];

    // BKY
    auto bky = _bbord[Piece::BKY];
    while (bky) {
        auto sq = bky.pick();
        auto ef = Effect::KB(sq, _ocupd);
        auto pc = ef & Effect::KW(_kingSW, _ocupd) & atk;
        if (! pc) {
            continue;
        }
        auto s  = pc.lsb();
        auto mk = (~Effect::HV(s)) & _empty;
        if (_board[s] == Piece::BOU) {
            _moveBOU(mk, m);
            continue;
        }
        _cfstFromB(mk, s, m);
    }

    // almost same operation for KA and HI here
    auto mv  = [&] (const Bitboard & (*func)(Square::Square, const Bitboard&),
                    const Bitboard & (*mask)(Square::Square),
                          Bitboard bmp                                         ) {
        while (bmp) {
            auto sq = bmp.pick();
            auto ef = func(sq, _ocupd) & mask(sq);
            auto pc = ef & (func(_kingSW, _ocupd) & mask(_kingSW)) & atk;
            if (! pc) {
                continue;
            }
            auto s  = pc.lsb();
            auto dr = Direction::distantDirection(sq, s);
            auto mk = (~DirectionMap[dr](s)) & _empty;
            if (_board[s] == Piece::BOU) {
                _moveBOU(mk, m);
                continue;
            }
            _cfstFromB(mk, s, m);
        }
    };

    // BKA and BUM
    auto bka = _bbord[Piece::BKA] | _bbord[Piece::BUM];
    mv(Effect::KA, Effect::RS, bka);
    mv(Effect::KA, Effect::FR, bka);

    // BHI and BRY
    auto bhi = _bbord[Piece::BHI] | _bbord[Piece::BRY];
    mv(Effect::HI, Effect::HH, bhi);
    mv(Effect::HI, Effect::HV, bhi);

}



/**
 * Discovered check fast for black
 * @param m array to store moves
 */
void Position::_discCFstW (Array<Move::Move, Move::Max> &m)
{

    // if the opponent has properly responded, his/her king shoudn't
    // be in check here.

    // attacking pieces
    auto atk = _piece[Color::White];

    // WKY
    auto wky = _bbord[Piece::WKY];
    while (wky) {
        auto sq = wky.pick();
        auto ef = Effect::KW(sq, _ocupd);
        auto pc = ef & Effect::KB(_kingSB, _ocupd) & atk;
        if (! pc) {
            continue;
        }
        auto s  = pc.lsb();
        auto mk = (~Effect::HV(s)) & _empty;
        if (_board[s] == Piece::WOU) {
            _moveWOU(mk, m);
            continue;
        }
        _cfstFromW(mk, s, m);
    }

    // almost same operation for KA and HI here
    auto mv  = [&] (const Bitboard & (*func)(Square::Square, const Bitboard&),
                    const Bitboard & (*mask)(Square::Square),
                          Bitboard bmp                                         ) {
        while (bmp) {
            auto sq = bmp.pick();
            auto ef = func(sq, _ocupd) & mask(sq);
            auto pc = ef & (func(_kingSB, _ocupd) & mask(_kingSB)) & atk;
            if (! pc) {
                continue;
            }
            auto s  = pc.lsb();
            auto dr = Direction::distantDirection(sq, s);
            auto mk = (~DirectionMap[dr](s)) & _empty;
            if (_board[s] == Piece::WOU) {
                _moveWOU(mk, m);
                continue;
            }
            _cfstFromW(mk, s, m);
        }
    };

    // WKA and WUM
    auto wka = _bbord[Piece::WKA] | _bbord[Piece::WUM];
    mv(Effect::KA, Effect::RS, wka);
    mv(Effect::KA, Effect::FR, wka);

    // WHI and WRY
    auto whi = _bbord[Piece::WHI] | _bbord[Piece::WRY];
    mv(Effect::HI, Effect::HH, whi);
    mv(Effect::HI, Effect::HV, whi);

}



/**
 * Number of possible moves for black pinned piece
 * @return number of moves
 */
int Position::_numPinB (void)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;
    if (! (pinned)) {
        return 0;
    }

    // number of moves
    int  num    = 0;

    while (pinned) {
        auto sq = pinned.pick();
        auto dr = Direction::distantDirection(sq, _kingSB);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::Black]);
        auto pc = _board[sq];
        switch (pc) {
        case BFU: case BKY: case BKE:
            break;
        case BGI:
            num += (Effect::AD(sq, Piece::BGI) & mk).popcnt();
            break;
        case BKI: case BTO: case BNY: case BNK: case BNG:
            num += (Effect::AD(sq, Piece::BKI) & mk).popcnt();
            break;
        case BUM:
            num += ((Effect::OC(sq) | Effect::KA(sq, _ocupd)) & mk).popcnt();
            break;
        case BRY:
            num += ((Effect::OC(sq) | Effect::HI(sq, _ocupd)) & mk).popcnt();
            break;
        case BKA:
            num += (Effect::KA(sq, _ocupd) & mk).popcnt();
            break;
        case BHI:
            num += (Effect::HI(sq, _ocupd) & mk).popcnt();
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

    return num;

}



/**
 * Number of possible moves for black pinned piece
 * @return number of moves
 */
int Position::_numPinW (void)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;
    if (! (pinned)) {
        return 0;
    }

    // number of moves
    int  num    = 0;

    while (pinned) {
        auto sq = pinned.pick();
        auto dr = Direction::distantDirection(sq, _kingSW);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::White]);
        auto pc = _board[sq];
        switch (pc) {
        case WFU: case WKY: case WKE:
            break;
        case WGI:
            num += (Effect::AD(sq, Piece::WGI) & mk).popcnt();
            break;
        case WKI: case WTO: case WNY: case WNK: case WNG:
            num += (Effect::AD(sq, Piece::WKI) & mk).popcnt();
            break;
        case WUM:
            num += ((Effect::OC(sq) | Effect::KA(sq, _ocupd)) & mk).popcnt();
            break;
        case WRY:
            num += ((Effect::OC(sq) | Effect::HI(sq, _ocupd)) & mk).popcnt();
            break;
        case WKA:
            num += (Effect::KA(sq, _ocupd) & mk).popcnt();
            break;
        case WHI:
            num += (Effect::HI(sq, _ocupd) & mk).popcnt();
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

    return num;

}



/**
 * Move pinned piece for black
 * @param m array to store moves
 */
void Position::_movePinB (Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;
    if (! (pinned)) {
        return;
    }

    // the pinned pieces in opponent area can promote when moving
    auto target = _bbord[BGI] | _bbord[BKA] | _bbord[BHI];
    auto pinnds = pinned & BlackPRMask & target;
    auto pinndn = pinned & (~pinnds);

    while (pinndn) {
        auto sq = pinndn.pick();
        auto dr = Direction::distantDirection(sq, _kingSB);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::Black]);
        auto pc = _board[sq];
        switch (pc) {
        case BFU: case BKY: case BKE:
            break;
        case BGI:
            _moveBGI(mk, sq, m);
            break;
        case BKI: case BTO: case BNY: case BNK: case BNG:
            _moveBKI(mk, sq, m);
            break;
        case BUM:
            _moveUM (mk, sq, m);
            break;
        case BRY:
            _moveRY (mk, sq, m);
            break;
        case BKA:
            _moveBKA(mk, sq, m);
            break;
        case BHI:
            _moveBHI(mk, sq, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

    while (pinnds) {
        auto sq = pinnds.pick();
        auto dr = Direction::distantDirection(sq, _kingSB);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::Black]);
        auto pc = _board[sq];
        switch (pc) {
        case BFU: case BKY: case BKE: case BKI: case BTO:
        case BNY: case BNK: case BNG: case BUM: case BRY:
            break;
        case BGI:
            _prmtBGI(mk, sq, m);
            break;
        case BKA:
            _promtKA(mk, sq, m);
            break;
        case BHI:
            _promtHI(mk, sq, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

}



/**
 * Move pinned piece for white
 * @param m array to store moves
 */
void Position::_movePinW (Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;
    if (! (pinned)) {
        return;
    }

    // the pinned pieces in opponent area can promote when moving
    auto target = _bbord[WGI] | _bbord[WKA] | _bbord[WHI];
    auto pinnds = pinned & WhitePRMask & target;
    auto pinndn = pinned & (~pinnds);

    while (pinndn) {
        auto sq = pinndn.pick();
        auto dr = Direction::distantDirection(sq, _kingSW);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::White]);
        auto pc = _board[sq];
        switch (pc) {
        case WFU: case WKY: case WKE:
            break;
        case WGI:
            _moveWGI(mk, sq, m);
            break;
        case WKI: case WTO: case WNY: case WNK: case WNG:
            _moveWKI(mk, sq, m);
            break;
        case WUM:
            _moveUM (mk, sq, m);
            break;
        case WRY:
            _moveRY (mk, sq, m);
            break;
        case WKA:
            _moveWKA(mk, sq, m);
            break;
        case WHI:
            _moveWHI(mk, sq, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

    while (pinnds) {
        auto sq = pinnds.pick();
        auto dr = Direction::distantDirection(sq, _kingSW);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::White]);
        auto pc = _board[sq];
        switch (pc) {
        case WFU: case WKY: case WKE: case WKI: case WTO:
        case WNY: case WNK: case WNG: case WUM: case WRY:
            break;
        case WGI:
            _prmtWGI(mk, sq, m);
            break;
        case WKA:
            _promtKA(mk, sq, m);
            break;
        case WHI:
            _promtHI(mk, sq, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

}



/**
 * Move black pinned piece to give check
 * @param m array to store moves
 */
void Position::_chckPinB (Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;

    while (pinned) {
        auto sq = pinned.pick();
        auto dr = Direction::distantDirection(sq, _kingSB);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::Black]);
        auto pc = _board[sq];
        switch (pc) {
        case BFU: case BKY: case BKE:
            break;
        case BGI:
            _chckBGI(Bitboard::Square[sq], mk, m);
            break;
        case BKI: case BTO: case BNY: case BNK: case BNG:
            mk &= Effect::AD(_kingSW, Piece::WKI);
            _moveBKI(mk, sq, m);
            break;
        case BUM:
            _chckBUM(Bitboard::Square[sq], mk, m);
            break;
        case BRY:
            _chckBRY(Bitboard::Square[sq], mk, m);
            break;
        case BKA:
            _chckBKA(Bitboard::Square[sq], mk, m);
            break;
        case BHI:
            _chckBHI(Bitboard::Square[sq], mk, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

}



/**
 * Move white pinned piece to give check
 * @param m array to store moves
 */
void Position::_chckPinW (Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = ~_pinnd;

    while (pinned) {
        auto sq = pinned.pick();
        auto dr = Direction::distantDirection(sq, _kingSW);
        auto mk = DirectionMap[dr](sq) & (~_piece[Color::White]);
        auto pc = _board[sq];
        switch (pc) {
        case WFU: case WKY: case WKE:
            break;
        case WGI:
            _chckWGI(Bitboard::Square[sq], mk, m);
            break;
        case WKI: case WTO: case WNY: case WNK: case WNG:
            mk &= Effect::AD(_kingSB, Piece::BKI);
            _moveWKI(mk, sq, m);
            break;
        case WUM:
            _chckWUM(Bitboard::Square[sq], mk, m);
            break;
        case WRY:
            _chckWRY(Bitboard::Square[sq], mk, m);
            break;
        case WKA:
            _chckWKA(Bitboard::Square[sq], mk, m);
            break;
        case WHI:
            _chckWHI(Bitboard::Square[sq], mk, m);
            break;
        default:
            _GAME_POSITION_CHECK(0);
            break;
        }
    }

}



/**
 * Number of possible moves for BOU
 * @return number of moves
 */
int Position::_numBOU (void)
{

    int  num      = 0;
    auto surround = Effect::OC(_kingSB) & (~_piece[Color::Black]);
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectW(sq)) {
            continue;
        }
        ++num;
    }

    return num;

}



/**
 * Number of possible moves for BOU with mask
 * @return number of moves
 */
int Position::_numBOU (const Bitboard &mask)
{

    int  num      = 0;
    auto surround = Effect::OC(_kingSB) & mask;
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectW(sq)) {
            continue;
        }
        ++num;
    }

    return num;

}



/**
 * Number of possible moves for WOU
 * @return number of moves
 */
int Position::_numWOU (void)
{

    int  num      = 0;
    auto surround = Effect::OC(_kingSW) & (~_piece[Color::White]);
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectB(sq)) {
            continue;
        }
        ++num;
    }

    return num;

}



/**
 * Number of possible moves for WOU with mask
 * @return number of moves
 */
int Position::_numWOU (const Bitboard &mask)
{

    int  num      = 0;
    auto surround = Effect::OC(_kingSW) & mask;
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectB(sq)) {
            continue;
        }
        ++num;
    }

    return num;

}



/**
 * Moves of BOU
 * 
 */
void Position::_moveBOU (Array<Move::Move, Move::Max> &m)
{

    auto surround = Effect::OC(_kingSB) & (~_piece[Color::Black]);
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectW(sq)) {
            continue;
        }
        m.add(Move::move(_kingSB, sq));
    }

}



/**
 * Moves of BOU with mask
 * 
 */
void Position::_moveBOU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto surround = Effect::OC(_kingSB) & mask;
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectW(sq)) {
            continue;
        }
        m.add(Move::move(_kingSB, sq));
    }

}



/**
 * Moves of WOU
 * 
 */
void Position::_moveWOU (Array<Move::Move, Move::Max> &m)
{

    auto surround = Effect::OC(_kingSW) & (~_piece[Color::White]);
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectB(sq)) {
            continue;
        }
        m.add(Move::move(_kingSW, sq));
    }

}



/**
 * Moves of WOU with mask
 * 
 */
void Position::_moveWOU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto surround = Effect::OC(_kingSW) & mask;
    while (surround) {
        auto sq = surround.pick();
        if (_chkEffectB(sq)) {
            continue;
        }
        m.add(Move::move(_kingSW, sq));
    }

}



/**
 * Number of possible moves to get out of check for BOU
 * @return number of moves
 */
int Position::_numOutB (void)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        return _numEscapeB();
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSB] & _pinnd;
    int  num  = _numToB(mask, _chckp.lsb());

    // drop a piece to block
    num += _ndropBFU(_chkDE);
    num += _ndropBKY(_chkDE);
    num += _ndropBKE(_chkDE);
    num += _ndropBOT(_chkDE);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        num += _numToB(mask, s);
    }

    // move OU to get out of check
    num += _numEscapeB();

    return num;

}



/**
 * Number of possible moves to get out of check for WOU
 * @return number of moves
 */
int Position::_numOutW (void)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        return _numEscapeW();
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSW] & _pinnd;
    int  num  = _numToW(mask, _chckp.lsb());

    // drop a piece to block
    num += _ndropWFU(_chkDE);
    num += _ndropWKY(_chkDE);
    num += _ndropWKE(_chkDE);
    num += _ndropWOT(_chkDE);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        num += _numToW(mask, s);
    }

    // move OU to get out of check
    num += _numEscapeW();

    return num;

}



/**
 * Get out of check for BOU
 * @param m array to store moves
 */
void Position::_getOutB (Array<Move::Move, Move::Max> &m)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        _escapeB(m);
        return;
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSB] & _pinnd;
    _moveToB(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropBFU(_chkDE, m);
    _dropBKY(_chkDE, m);
    _dropBKE(_chkDE, m);
    _dropBOT(_chkDE, m);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _moveToB(mask, s, m);
    }

    // move OU to get out of check
    _escapeB(m);

}



/**
 * Get out of check for BOU (fast)
 * @param m array to store moves
 */
void Position::_fstOutB (Array<Move::Move, Move::Max> &m)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        _escapeB(m);
        return;
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSB];
    _fastToB(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropBFU(_chkDE, m);
    _dropBKY(_chkDE, m);
    _dropBKE(_chkDE, m);
    _dropBOT(_chkDE, m);

    // move the piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _fastToB(mask, s, m);
    }

    // move OU to get out of check
    _escapeB(m);

}



/**
 * Get out of check for WOU
 * @param m array to store moves
 * 
 */
void Position::_getOutW (Array<Move::Move, Move::Max> &m)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        _escapeW(m);
        return;
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSW] & _pinnd;
    _moveToW(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropWFU(_chkDE, m);
    _dropWKY(_chkDE, m);
    _dropWKE(_chkDE, m);
    _dropWOT(_chkDE, m);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _moveToW(mask, s, m);
    }

    // move OU to get out of check
    _escapeW(m);

}



/**
 * Get out of check for WOU (fast)
 * @param m array to store moves
 */
void Position::_fstOutW (Array<Move::Move, Move::Max> &m)
{

    // double check (OU can only run)
    if (_nchek == 2) {
        _escapeW(m);
        return;
    }

    // capture the piece making a check (except for a move to caputure by OU)
    auto mask = Bitboard::Invert[_kingSW];
    _fastToW(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropWFU(_chkDE, m);
    _dropWKY(_chkDE, m);
    _dropWKE(_chkDE, m);
    _dropWOT(_chkDE, m);

    // move the piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _fastToW(mask, s, m);
    }

    // move OU to get out of check
    _escapeW(m);

}



/**
 * Make direction mask for escaping OU
 * @param sq square of OU
 * @return direction mask
 */
Bitboard Position::_escape (Square::Square current)
{

    // make direction mask
    auto dirmask  = Bitboard::Fill;

    // OU can't escape toward the distant effects. This mask is to
    // eliminate moves to those direction.
    auto mask     = [&dirmask, &current] (Bitboard e) {
        if (e) {
            auto sq  = e.lsb();
            auto dr  = Direction::distantDirection(current, sq);
            dirmask &= Effect::ED(current, dr);
            // exceptionally if the effecting piece is adjacent,
            // capturing it is possible.
            dirmask |= (e & Effect::OC(current));
        }
    };
    mask(_chkKY);
    mask(_chkKA);
    mask(_chkHI);

    return dirmask;

}



/**
 * Number of possible moves to escape from checks for BOU
 * @return number of moves
 */
int Position::_numEscapeB (void)
{

    // make direction mask
    auto dirmask = (~_piece[Color::Black]) & _escape(_kingSB);

    // move
    return _numBOU(dirmask);

}



/**
 * Number of possible moves to escape from checks for WOU
 * @return number of moves
 */
int Position::_numEscapeW (void)
{

    // make direction mask
    auto dirmask = (~_piece[Color::White]) & _escape(_kingSW);

    // move
    return _numWOU(dirmask);

}



/**
 * Let BOU escape from checks
 * @param m array to store moves
 */
void Position::_escapeB (Array<Move::Move, Move::Max> &m)
{

    // make direction mask
    auto dirmask = (~_piece[Color::Black]) & _escape(_kingSB);

    // move
    _moveBOU(dirmask, m);

}



/**
 * Let WOU escape from checks
 * @param m array to store moves
 */
void Position::_escapeW (Array<Move::Move, Move::Max> &m)
{

    // make direction mask
    auto dirmask = (~_piece[Color::White]) & _escape(_kingSW);

    // move
    _moveWOU(dirmask, m);

}



/**
 * Move the black piece from square to square
 * @param from square move from
 * @param to   square move to
 * @param m    array to store moves
 */
void Position::_moveBlack (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::BFU:
        if (BFUMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            _m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKY:
        if (BFUMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKE:
        if (BKEMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BGI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKA:
    case Piece::BHI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
            _m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Move the black piece from square to square (fast)
 * @param from square move from
 * @param to   square move to
 * @param m array to store moves
 */
void Position::_fastBlack (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::BFU:
        if (BlackCanPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKY:
        if (BFUMPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKE:
        if (BKEMPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BGI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKA:
    case Piece::BHI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Move the white piece from square to square
 * @param from square move from
 * @param to   square move to
 * @param m array to store moves
 */
void Position::_moveWhite (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::WFU:
        if (WFUMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            _m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKY:
        if (WFUMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKE:
        if (WKEMPromote[to]) {
             m.add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WGI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
             m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKA:
    case Piece::WHI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
            _m.add(Move::move   (from, to)); 
             m.add(Move::promote(from, to)); 
        } else {
             m.add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Move the white piece from square to square (fast)
 * @param from square move from
 * @param to   square move to
 * @param m    array to store moves
 */
void Position::_fastWhite (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::WFU:
        if (WhiteCanPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKY:
        if (WFUMPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKE:
        if (WKEMPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WGI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
            m.add(Move::move   (from, to)); 
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKA:
    case Piece::WHI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
            m.add(Move::promote(from, to)); 
        } else {
            m.add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Number of possible moves to drop BFU
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropBFU (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::Black][Piece::FU] == 0) {
        return 0;
    }

    // Nifu check
    return (_bbord[Piece::BFU].column() & mask & CanDropBFU).popcnt();

}



/**
 * Number of possible moves to drop WFU
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropWFU (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::White][Piece::FU] == 0) {
        return 0;
    }

    // Nifu check and return the number
    return (_bbord[Piece::WFU].column() & mask & CanDropWFU).popcnt();

}


/**
 * Number of possible moves to drop BKY
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropBKY (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::Black][Piece::KY] == 0) {
        return 0;
    }

    return (mask & CanDropBFU).popcnt();

}



/**
 * Number of possible moves to drop WKY
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropWKY (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::White][Piece::KY] == 0) {
        return 0;
    }

    return (mask & CanDropWFU).popcnt();

}



/**
 * Number of possible moves to drop BKE
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropBKE (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::Black][Piece::KE] == 0) {
        return 0;
    }

    return (mask & CanDropBKE).popcnt();

}



/**
 * Number of possible moves to drop WKE
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropWKE (const Bitboard &mask)
{

    // check if the piece is available
    if (_hands[Color::White][Piece::KE] == 0) {
        return 0;
    }

    return (mask & CanDropWKE).popcnt();

}



/**
 * Number of possible moves to drop other black pieces
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropBOT (const Bitboard &mask)
{

    Piece::Piece            list[] = {Piece::GI, Piece::KI, Piece::KA, Piece::HI};

    int  num = 0;
    for (auto pc : list) {
        if (_hands[Color::Black][pc] > 0) {
            ++num;
        }
    }

    return (num * mask.popcnt());

}



/**
 * Number of possible moves to drop other white pieces
 * @param mask potential squares
 * @return number of moves
 */
int Position::_ndropWOT (const Bitboard &mask)
{

    Piece::Piece            list[] = {Piece::GI, Piece::KI, Piece::KA, Piece::HI};

    int  num = 0;
    for (auto pc : list) {
        if (_hands[Color::White][pc] > 0) {
            ++num;
        }
    }

    return (num * mask.popcnt());

}



/**
 * Drop BFU
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropBFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::FU;

    // check if the piece is available
    if (_hands[Color::Black][h] == 0) {
        return;
    }

    // Nifu check
    auto sq = _bbord[Piece::BFU].column() & mask & CanDropBFU;
    _dropMove(h, sq, m);

}



/**
 * Drop WFU
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropWFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::FU;

    // check if the piece is available
    if (_hands[Color::White][h] == 0) {
        return;
    }

    // Nifu check
    auto sq = _bbord[Piece::WFU].column() & mask & CanDropWFU;
    _dropMove(h, sq, m);

}



/**
 * Drop BKY
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropBKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::KY;

    // check if the piece is available
    if (_hands[Color::Black][h] == 0) {
        return;
    }

    auto sq = mask & CanDropBFU;
    _dropMove(h, sq, m);

}



/**
 * Drop WKY
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropWKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::KY;

    // check if the piece is available
    if (_hands[Color::White][h] == 0) {
        return;
    }

    auto sq = mask & CanDropWFU;
    _dropMove(h, sq, m);

}



/**
 * Drop BKE
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropBKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::KE;

    // check if the piece is available
    if (_hands[Color::Black][h] == 0) {
        return;
    }

    auto sq = mask & CanDropBKE;
    _dropMove(h, sq, m);

}



/**
 * Drop WKE
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropWKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    const Piece::Piece      h = Piece::KE;

    // check if the piece is available
    if (_hands[Color::White][h] == 0) {
        return;
    }

    auto sq = mask & CanDropWKE;
    _dropMove(h, sq, m);

}



/**
 * Drop other black pieces
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropBOT (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    Piece::Piece            list[] = {Piece::GI, Piece::KI, Piece::KA, Piece::HI};

    auto sq = mask;
    while (sq) {
        auto s = sq.pick();
        for (auto pc : list) {
            if (_hands[Color::Black][pc] > 0) {
                m.add(Move::drop(pc, s));
            }
        }
    }

}



/**
 * Drop other black pieces to give check
 * @param m array to store moves
 *
 */
void Position::_dchkBOT (Array<Move::Move, Move::Max> &m)
{

    Bitboard                sq;

    // GI
    sq = Effect::AD(_kingSW, Piece::WGI) & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::Black][Piece::GI] > 0) {
            m.add(Move::drop(Piece::GI, s));
        }
    }

    // KI
    sq = Effect::AD(_kingSW, Piece::WKI) & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::Black][Piece::KI] > 0) {
            m.add(Move::drop(Piece::KI, s));
        }
    }

    // KA
    sq = Effect::KA(_kingSW, _ocupd)     & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::Black][Piece::KA] > 0) {
            m.add(Move::drop(Piece::KA, s));
        }
    }

    // KA
    sq = Effect::HI(_kingSW, _ocupd)     & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::Black][Piece::HI] > 0) {
            m.add(Move::drop(Piece::HI, s));
        }
    }

}



/**
 * Drop other white pieces
 * @param mask potential squares
 * @param m    array to store moves
 */
void Position::_dropWOT (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    Piece::Piece            list[] = {Piece::GI, Piece::KI, Piece::KA, Piece::HI};

    auto sq = mask;
    while (sq) {
        auto s = sq.pick();
        for (auto pc : list) {
            if (_hands[Color::White][pc] > 0) {
                m.add(Move::drop(pc, s));
            }
        }
    }

}



/**
 * Drop other white pieces to give check
 * @param m array to store moves
 *
 */
void Position::_dchkWOT (Array<Move::Move, Move::Max> &m)
{

    Bitboard                sq;

    // GI
    sq = Effect::AD(_kingSB, Piece::BGI) & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::White][Piece::GI] > 0) {
            m.add(Move::drop(Piece::GI, s));
        }
    }

    // KI
    sq = Effect::AD(_kingSB, Piece::BKI) & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::White][Piece::KI] > 0) {
            m.add(Move::drop(Piece::KI, s));
        }
    }

    // KA
    sq = Effect::KA(_kingSB, _ocupd)     & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::White][Piece::KA] > 0) {
            m.add(Move::drop(Piece::KA, s));
        }
    }

    // KA
    sq = Effect::HI(_kingSB, _ocupd)     & _empty;
    while (sq) {
        auto s = sq.pick();
        if (_hands[Color::White][Piece::HI] > 0) {
            m.add(Move::drop(Piece::HI, s));
        }
    }

}



/**
 * Number of the moves to certain square for black
 * @param mask mask
 * @param dst  destination
 * @return number of moves
 */
int Position::_numToB (const Bitboard &mask, Square::Square dst)
{

    _allEffectB(dst);
    return (_effect & mask).popcnt();

}



/**
 * Number of the moves to certain square for white
 * @param mask mask
 * @param dst  destination
 * @return number of moves
 */
int Position::_numToW (const Bitboard &mask, Square::Square dst)
{

    _allEffectW(dst);
    return (_effect & mask).popcnt();

}



/**
 * All the moves to certain square
 * @param mask mask
 * @param dst  destination
 * @param m    array to store moves
 */
void Position::_moveToB (const Bitboard &mask, Square::Square dst,
                                    Array<Move::Move, Move::Max> &m)
{

    _allEffectB(dst);
    auto sq = _effect & mask;
    while (sq) {
        auto s = sq.pick();
        _moveBlack(s, dst, m);
    }

}



/**
 * All the moves to certain square (fast)
 * @param mask mask
 * @param dst  destination
 * @param m    array to store moves
 */
void Position::_fastToB (const Bitboard &mask, Square::Square dst,
                                    Array<Move::Move, Move::Max> &m)
{

    _allEffectB(dst);
    auto sq = _effect & mask;
    while (sq) {
        auto s = sq.pick();
        _fastBlack(s, dst, m);
    }

}



/**
 * All the moves to certain square
 * @param mask mask
 * @param dst  destination
 * @param m    array to store moves
 */
void Position::_moveToW (const Bitboard &mask, Square::Square dst,
                                    Array<Move::Move, Move::Max> &m)
{

    _allEffectW(dst);
    auto sq = _effect & mask;
    while (sq) {
        auto s = sq.pick();
        _moveWhite(s, dst, m);
    }


}



/**
 * All the moves to certain square (fast)
 * @param mask mask
 * @param dst  destination
 * @param m    array to store moves
 */
void Position::_fastToW (const Bitboard &mask, Square::Square dst,
                                    Array<Move::Move, Move::Max> &m)
{

    _allEffectW(dst);
    auto sq = _effect & mask;
    while (sq) {
        auto s = sq.pick();
        _fastWhite(s, dst, m);
    }


}



/**
 * Move the black piece from square to square this function is specialized
 * to discovering check
 * @param mask mask
 * @param from square move from
 * @param m    array to store moves
 */
void Position::_chckFromB (const Bitboard &mask, Square::Square from,
                                      Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    switch (_board[from]) {
    case BFU:
        { auto ef = Bitboard::Square[from + Square::UWARD] & mask;
            while (ef) {
                auto to = ef.pick();
                if (BFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (BlackCanPromote[to]) {
                    _m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BKY:
        { auto ef = Effect::KB(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (BlackCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move   (from, to)); 
                }
            }
        }
        break;
    case BKE:
        { auto ef = Effect::AD(from, BKE) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BKEMPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (BlackCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::BKE) & _bbord[Piece::WOU])) {
                        m.add(Move::move(from, to)); 
                    }
                }
            }
        }
        break;
    case BGI:
        { auto ef = Effect::AD(from, BGI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::BGI) & _bbord[Piece::WOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::BGI) & _bbord[Piece::WOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case BKI: case BTO: case BNY: case BNK: case BNG:
        { auto ef = Effect::AD(from, BKI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BUM:
        { auto ef = (Effect::AD(from, BUM) | Effect::KA(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                            & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BRY:
        { auto ef = (Effect::AD(from, BRY) | Effect::HI(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                           & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BKA:
        { auto ef = Effect::KA(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::WOU])) {
                        _m.add(Move::move   (from, to)); 
                    }
                    if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                & _bbord[Piece::WOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::WOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case BHI:
        { auto ef = Effect::HI(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::WOU])) {
                        _m.add(Move::move   (from, to)); 
                    }
                    if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                                & _bbord[Piece::WOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::WOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    default:
        _GAME_POSITION_CHECK(0);
        break;
    }

}




/**
 * Move the black piece from square to square (fast). this function is
 * specialized to discovering check 
 * @param mask mask
 * @param from square move from
 * @param m    array to store moves
 */
void Position::_cfstFromB (const Bitboard &mask, Square::Square from,
                                      Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    switch (_board[from]) {
    case BFU:
        { auto ef = Bitboard::Square[from + Square::UWARD] & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BKY:
        { auto ef = Effect::KB(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (BlackCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move   (from, to)); 
                }
            }
        }
        break;
    case BKE:
        { auto ef = Effect::AD(from, BKE) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BKEMPromote[to]) {
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (BlackCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::BKE) & _bbord[Piece::WOU])) {
                        m.add(Move::move(from, to)); 
                    }
                }
            }
        }
        break;
    case BGI:
        { auto ef = Effect::AD(from, BGI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::BGI) & _bbord[Piece::WOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                    if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::BGI) & _bbord[Piece::WOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case BKI: case BTO: case BNY: case BNK: case BNG:
        { auto ef = Effect::AD(from, BKI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! (Effect::AD(to, Piece::BKI) & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BUM:
        { auto ef = (Effect::AD(from, BUM) | Effect::KA(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                            & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BRY:
        { auto ef = (Effect::AD(from, BRY) | Effect::HI(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                           & _bbord[Piece::WOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case BKA:
        { auto ef = Effect::KA(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                  & _bbord[Piece::WOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::WOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case BHI:
        { auto ef = Effect::HI(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (BlackCanPromote[from] || BlackCanPromote[to]) {
                    if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                                  & _bbord[Piece::WOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::WOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    default:
        _GAME_POSITION_CHECK(0);
        break;
    }

}



/**
 * Move the white piece from square to square this function is specialized
 * to discovering check
 * @param mask mask
 * @param from square move from
 * @param m    array to store moves
 */
void Position::_chckFromW (const Bitboard &mask, Square::Square from,
                                      Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    switch (_board[from]) {
    case WFU:
        { auto ef = Bitboard::Square[from + Square::DWARD] & mask;
            while (ef) {
                auto to = ef.pick();
                if (WFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (WhiteCanPromote[to]) {
                    _m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKY:
        { auto ef = Effect::KW(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (WhiteCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKE:
        { auto ef = Effect::AD(from, WKE) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WKEMPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (WhiteCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::WKE) & _bbord[Piece::BOU])) {
                        m.add(Move::move(from, to)); 
                    }
                }
            }
        }
        break;
    case WGI:
        { auto ef = Effect::AD(from, WGI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::WGI) & _bbord[Piece::BOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::WGI) & _bbord[Piece::BOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case WKI: case WTO: case WNY: case WNK: case WNG:
        { auto ef = Effect::AD(from, WKI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WUM:
        { auto ef = (Effect::AD(from, WUM) | Effect::KA(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                            & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WRY:
        { auto ef = (Effect::AD(from, WRY) | Effect::HI(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                            & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKA:
        { auto ef = Effect::KA(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::BOU])) {
                        _m.add(Move::move   (from, to)); 
                    }
                    if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                  & _bbord[Piece::BOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::BOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case WHI:
        { auto ef = Effect::HI(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::BOU])) {
                        _m.add(Move::move   (from, to)); 
                    }
                    if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                                  & _bbord[Piece::BOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::BOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    default:
        _GAME_POSITION_CHECK(0);
        break;
    }

}



/**
 * Move the white piece from square to square (fast). this function is
 * specialized to discovering check
 * @param mask mask
 * @param from square move from
 * @param m    array to store moves
 */
void Position::_cfstFromW (const Bitboard &mask, Square::Square from,
                                      Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    switch (_board[from]) {
    case WFU:
        { auto ef = Bitboard::Square[from + Square::DWARD] & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKY:
        { auto ef = Effect::KW(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WFUMPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (WhiteCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                     m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKE:
        { auto ef = Effect::AD(from, WKE) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WKEMPromote[to]) {
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else
                if (WhiteCanPromote[to]) {
                     m.add(Move::move(from, to)); 
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::WKE) & _bbord[Piece::BOU])) {
                        m.add(Move::move(from, to)); 
                    }
                }
            }
        }
        break;
    case WGI:
        { auto ef = Effect::AD(from, WGI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! (Effect::AD(to, Piece::WGI) & _bbord[Piece::BOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                    if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                        m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::AD(to, Piece::WGI) & _bbord[Piece::BOU])) {
                        m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case WKI: case WTO: case WNY: case WNK: case WNG:
        { auto ef = Effect::AD(from, WKI) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! (Effect::AD(to, Piece::WKI) & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WUM:
        { auto ef = (Effect::AD(from, WUM) | Effect::KA(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                            & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WRY:
        { auto ef = (Effect::AD(from, WRY) | Effect::HI(from, _ocupd)) & mask;
            while (ef) {
                auto to = ef.pick();
                if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                            & _bbord[Piece::BOU])) {
                    m.add(Move::move(from, to)); 
                }
            }
        }
        break;
    case WKA:
        { auto ef = Effect::KA(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! ((Effect::OC(to) | Effect::KA(to, _ocupd))
                                                  & _bbord[Piece::BOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::KA(to, _ocupd) & _bbord[Piece::BOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    case WHI:
        { auto ef = Effect::HI(from, _ocupd) & mask;
            while (ef) {
                auto to = ef.pick();
                if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
                    if (! ((Effect::OC(to) | Effect::HI(to, _ocupd))
                                                  & _bbord[Piece::BOU])) {
                         m.add(Move::promote(from, to)); 
                    }
                } else {
                    if (! (Effect::HI(to, _ocupd) & _bbord[Piece::BOU])) {
                         m.add(Move::move   (from, to)); 
                    }
                }
            }
        }
        break;
    default:
        _GAME_POSITION_CHECK(0);
        break;
    }

}



/**
 * Check if any of the black piece reaches to the square
 * @param c  color of pieces to check
 * @param sq square to check
 * @return false for no effect, true otherwise.
 */
bool Position::_chkEffectB (Square::Square sq)
{

    // check adjacent pieces
    auto ad = Effect::DU(sq) & _piece[Color::Black];
    while (ad) {
        auto s = ad.pick();
        if ((Effect::AD(s, _board[s]) & Bitboard::Square[sq])) {
            return true;
        }
    }

    // pieces with distant effect
    auto aky = Piece::BKY;
    auto aka = Piece::BKA;
    auto ahi = Piece::BHI;
    auto aum = Piece::BUM;
    auto ary = Piece::BRY;

    // check HI and RY
    if ((Effect::HI(sq, _ocupd) & (_bbord[ahi] | _bbord[ary]))) {
        return true;
    }

    // check KA and UM
    if ((Effect::KA(sq, _ocupd) & (_bbord[aka] | _bbord[aum]))) {
        return true;
   }

    // check KY
    if ((Effect::KW(sq, _ocupd) & _bbord[aky]))   {
        return true;
    }

    return false;

}



/**
 * Check if any of the white piece reaches to the square
 * @param c  color of pieces to check
 * @param sq square to check
 * @return false for no effect, true otherwise.
 */
bool Position::_chkEffectW (Square::Square sq)
{

    // check adjacent pieces
    auto ad = Effect::DU(sq) & _piece[Color::White];
    while (ad) {
        auto s = ad.pick();
        if ((Effect::AD(s, _board[s]) & Bitboard::Square[sq])) {
            return true;
        }
    }

    // pieces with distant effect
    auto aky = Piece::WKY;
    auto aka = Piece::WKA;
    auto ahi = Piece::WHI;
    auto aum = Piece::WUM;
    auto ary = Piece::WRY;

    // check HI and RY
    if ((Effect::HI(sq, _ocupd) & (_bbord[ahi] | _bbord[ary]))) {
        return true;
    }

    // check KA and UM
    if ((Effect::KA(sq, _ocupd) & (_bbord[aka] | _bbord[aum]))) {
        return true;
    }

    // check KY
    if ((Effect::KB(sq, _ocupd) & _bbord[aky]))   {
        return true;
    }

    return false;

}



/**
 * Check all the black pieces reach to the square
 * @param sq square to check
 */
void Position::_allEffectB (Square::Square sq)
{

    _effect = Bitboard::Zero;

    // check adjacent pieces
    auto ad = Effect::DU(sq) & _piece[Color::Black];
    while (ad) {
        auto s   = ad.pick();
        if ((Effect::AD(s, _board[s]) & Bitboard::Square[sq])) {
            _effect |= Bitboard::Square[s];
        }
    }

    // pieces with distant effect
    auto aky = Piece::BKY;
    auto aka = Piece::BKA;
    auto ahi = Piece::BHI;
    auto aum = Piece::BUM;
    auto ary = Piece::BRY;

    // check HI and RY
    auto hi = Effect::HI(sq, _ocupd) & (_bbord[ahi] | _bbord[ary]);
    _effect |= hi;

    // check KA and UM
    auto ka = Effect::KA(sq, _ocupd) & (_bbord[aka] | _bbord[aum]);
    _effect |= ka;

    // check KY
    auto ky = Effect::KW(sq, _ocupd) & _bbord[aky];
    _effect |= ky;

}



/**
 * Check all the white pieces reach to the square
 * @param sq square to check
 */
void Position::_allEffectW (Square::Square sq)
{

    _effect = Bitboard::Zero;

    // check adjacent pieces
    auto ad = Effect::DU(sq) & _piece[Color::White];
    while (ad) {
        auto s   = ad.pick();
        if ((Effect::AD(s, _board[s]) & Bitboard::Square[sq])) {
            _effect |= Bitboard::Square[s];
        }
    }

    // pieces with distant effect
    auto aky = Piece::WKY;
    auto aka = Piece::WKA;
    auto ahi = Piece::WHI;
    auto aum = Piece::WUM;
    auto ary = Piece::WRY;

    // check HI and RY
    auto hi = Effect::HI(sq, _ocupd) & (_bbord[ahi] | _bbord[ary]);
    _effect |= hi;

    // check KA and UM
    auto ka = Effect::KA(sq, _ocupd) & (_bbord[aka] | _bbord[aum]);
    _effect |= ka;

    // check KY
    auto ky = Effect::KB(sq, _ocupd) & _bbord[aky];
    _effect |= ky;

}



/**
 * Check the black pinned pieces
 * depending on makeCache() must have been invoked beforehand.
 */
void Position::_makePinB (void)
{

    // clear
    _pinnd     = Bitboard::Fill;

    // if BOU doesn't exist, do nothing here
    if (_kingSB == Square::SQVD) {
        return;
    }

    // square of guarding OU
    auto guard = _kingSB;

    // gurading FU and KY
    // pinning  FU and KY against vertical effect can be masked
    auto gfu   = Piece::BFU;
    auto gky   = Piece::BKY;
    auto ify   = (~_bbord[gfu]) & (~_bbord[gky]);

    // pieces with distant effect
    auto aky   = Piece::WKY;
    auto aka   = Piece::WKA;
    auto ahi   = Piece::WHI;
    auto aum   = Piece::WUM;
    auto ary   = Piece::WRY;

    // for attacking KY
    auto mky = _bbord[aky] & (~_chckp);
    while (mky) {
        auto sq  = mky.pick();
        auto yk  = Effect::KW(sq, _ocupd);
        auto yo  = Effect::KB(guard, _ocupd);
        _pinnd  ^= (yk & yo & ify);
    }

    // for attacking KA and UM
    auto mka = (_bbord[aka] | _bbord[aum]) & (~_chckp);
    while (mka) {
        auto sq  = mka.pick();
        auto kk  = Effect::KA(sq,    _ocupd);
        auto ko  = Effect::KA(guard, _ocupd);
        _pinnd  ^= ((kk & Effect::RS(sq)) & (ko & Effect::RS(guard)));
        _pinnd  ^= ((kk & Effect::FR(sq)) & (ko & Effect::FR(guard)));
    }

    // for attacking HI and RY
    auto mhi = (_bbord[ahi] | _bbord[ary]) & (~_chckp);
    while (mhi) {
        auto sq  = mhi.pick();
        auto hh  = Effect::HI(sq,    _ocupd);
        auto ho  = Effect::HI(guard, _ocupd);
        _pinnd  ^= ((hh & Effect::HH(sq)) & (ho & Effect::HH(guard)));
        _pinnd  ^= ((hh & Effect::HV(sq)) & (ho & Effect::HV(guard)) & ify);
    }

}



/**
 * Check the white pinned pieces
 * depending on makeCache() must have been invoked beforehand.
 * 
 */
void Position::_makePinW (void)
{

    // clear
    _pinnd     = Bitboard::Fill;

    // if WOU doesn't exist, do nothing here
    if (_kingSW == Square::SQVD) {
        return;
    }

    // square of guarding OU
    auto guard = _kingSW;

    // gurading FU and KY
    // pinning  FU and KY against vertical effect can be masked
    auto gfu   = Piece::WFU;
    auto gky   = Piece::WKY;
    auto ify   = (~_bbord[gfu]) & (~_bbord[gky]);

    // pieces with distant effect
    auto aky   = Piece::BKY;
    auto aka   = Piece::BKA;
    auto ahi   = Piece::BHI;
    auto aum   = Piece::BUM;
    auto ary   = Piece::BRY;

    // for attacking KY
    auto mky = _bbord[aky] & (~_chckp);
    while (mky) {
        auto sq  = mky.pick();
        auto yk  = Effect::KB(sq, _ocupd);
        auto yo  = Effect::KW(guard, _ocupd);
        _pinnd  ^= (yk & yo & ify);
    }

    // for attacking KA and UM
    auto mka = (_bbord[aka] | _bbord[aum]) & (~_chckp);
    while (mka) {
        auto sq  = mka.pick();
        auto kk  = Effect::KA(sq,    _ocupd);
        auto ko  = Effect::KA(guard, _ocupd);
        _pinnd  ^= ((kk & Effect::RS(sq)) & (ko & Effect::RS(guard)));
        _pinnd  ^= ((kk & Effect::FR(sq)) & (ko & Effect::FR(guard)));
    }

    // for attacking HI and RY
    auto mhi = (_bbord[ahi] | _bbord[ary]) & (~_chckp);
    while (mhi) {
        auto sq  = mhi.pick();
        auto hh  = Effect::HI(sq,    _ocupd);
        auto ho  = Effect::HI(guard, _ocupd);
        _pinnd  ^= ((hh & Effect::HH(sq)) & (ho & Effect::HH(guard)));
        _pinnd  ^= ((hh & Effect::HV(sq)) & (ho & Effect::HV(guard)) & ify);
    }

}



/**
 * Make a bitboard for the pieces making a check (black)
 *
 */
void Position::_makeCheckB (void)
{

    // clear number of checks
    _nchek        = 0;

    // if BOU doesn't exist, do nothing here
    if (_kingSB == Square::SQVD) {
        return;
    }

    // clear the bitboard of checking condition
    _chckp        = Bitboard::Zero;
    _chkAD        = Bitboard::Zero;
    _chkKY        = Bitboard::Zero;
    _chkKA        = Bitboard::Zero;
    _chkHI        = Bitboard::Zero;
    _chkDE        = Bitboard::Zero;

    // square of guarding OU
    auto guard    = _kingSB;

    // adjacent pieces
    auto adjacent = Effect::DU(guard) & _piece[Color::White];

    // check by adjacent pieces
    while (adjacent) {
        auto sq = adjacent.pick();
        if (Effect::AD(sq, _board[sq]) & Bitboard::Square[guard]) {
            _chckp ^= Bitboard::Square[sq];
            _chkAD ^= Bitboard::Square[sq];
            ++_nchek;
            break;
        }
    }

    // pieces with distant effect
    auto aky = Piece::WKY;
    auto aka = Piece::WKA;
    auto ahi = Piece::WHI;
    auto aum = Piece::WUM;
    auto ary = Piece::WRY;

    // general purpose variable of bitboard
    Bitboard check;

    // check by KA and UM
    // put KA at the square of OU and check if it effects KA or UM on the board
    check = Effect::KA(guard, _ocupd) & (_bbord[aka] | _bbord[aum]);
    if (check) {
        // checking KA or UM must be unique
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkKA ^= Bitboard::Square[sq];
        _chkDE  = Effect::KA(guard, _ocupd) & Effect::KA(sq, _ocupd);
        ++_nchek;
    }

    // check by HI and RY
    // put HI at the square of OU and check if it effects HI or RY on the board
    check = Effect::HI(guard, _ocupd) & (_bbord[ahi] | _bbord[ary]);
    if (check) {
        // checking HI, RY, KY must be unique
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkHI ^= Bitboard::Square[sq];
        _chkDE  = Effect::HI(guard, _ocupd) & Effect::HI(sq, _ocupd);
        ++_nchek;
        return;
    }

    // check by KY
    // put KY at the square of OU and check if it effects KY on the board
    check = Effect::KB(guard, _ocupd) & _bbord[aky];
    if (check) {
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkKY ^= Bitboard::Square[sq];
        _chkDE  = Effect::KB(guard, _ocupd) & Effect::KW(sq, _ocupd);
        ++_nchek;
    }

}



/**
 * Make a bitboard for the pieces making a check (white)
 *
 */
void Position::_makeCheckW (void)
{

    // clear number of checks
    _nchek        = 0;

    // if WOU doesn't exist, do nothing here
    if (_kingSW == Square::SQVD) {
        return;
    }

    // clear the bitboard of checking condition
    _chckp        = Bitboard::Zero;
    _chkAD        = Bitboard::Zero;
    _chkKY        = Bitboard::Zero;
    _chkKA        = Bitboard::Zero;
    _chkHI        = Bitboard::Zero;
    _chkDE        = Bitboard::Zero;

    // square of guarding OU
    auto guard    = _kingSW;

    // adjacent pieces
    auto adjacent = Effect::DU(guard) & _piece[Color::Black];

    // check by adjacent pieces
    while (adjacent) {
        auto sq = adjacent.pick();
        if (Effect::AD(sq, _board[sq]) & Bitboard::Square[guard]) {
            _chckp ^= Bitboard::Square[sq];
            _chkAD ^= Bitboard::Square[sq];
            ++_nchek;
            break;
        }
    }

    // pieces with distant effect
    auto aky = Piece::BKY;
    auto aka = Piece::BKA;
    auto ahi = Piece::BHI;
    auto aum = Piece::BUM;
    auto ary = Piece::BRY;

    // general purpose variable of bitboard
    Bitboard check;

    // check by KA and UM
    // put KA at the square of OU and check if it effects KA or UM on the board
    check = Effect::KA(guard, _ocupd) & (_bbord[aka] | _bbord[aum]);
    if (check) {
        // checking KA or UM must be unique
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkKA ^= Bitboard::Square[sq];
        _chkDE  = Effect::KA(guard, _ocupd) & Effect::KA(sq, _ocupd);
        ++_nchek;
    }

    // check by HI and RY
    // put HI at the square of OU and check if it effects HI or RY on the board
    check = Effect::HI(guard, _ocupd) & (_bbord[ahi] | _bbord[ary]);
    if (check) {
        // checking HI, RY, KY must be unique
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkHI ^= Bitboard::Square[sq];
        _chkDE  = Effect::HI(guard, _ocupd) & Effect::HI(sq, _ocupd);
        ++_nchek;
        return;
    }

    // check by KY
    // put KY at the square of OU and check if it effects KY on the board
    check = Effect::KW(guard, _ocupd) & _bbord[aky];
    if (check) {
        auto sq = check.lsb();
        _chckp ^= Bitboard::Square[sq];
        _chkKY ^= Bitboard::Square[sq];
        _chkDE  = Effect::KW(guard, _ocupd) & Effect::KB(sq, _ocupd);
        ++_nchek;
    }

}



/**
 * Hash full
 */
void Position::_hashFull (void)
{

    // key for the pieces on board
    _keyBoard = 0;
    for (auto s : Square::all) {
        Piece::Piece p  = _board[s];
        _keyBoard       ^= Zobrist::position[s][p];
    }

    // key for the pieces in hands
    _keyHands = 0;
    for (auto p : Piece::hand) {
        _keyHands += (_hands[Color::Black][p] * Zobrist::hands[Color::Black][p]);
        _keyHands += (_hands[Color::White][p] * Zobrist::hands[Color::White][p]);
    }

    // whole key
    _key = _keyBoard ^ _keyHands;

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

