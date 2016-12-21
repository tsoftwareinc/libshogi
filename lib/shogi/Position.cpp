/**
 *****************************************************************************

 @file       Position.cpp

 @brief      Position implementation
  
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

// promotion area for black RNK1 - RNK2
static constexpr Bitboard   BlackPRMask(
                                  0x01c0e070381c0e07ULL,
                                  0x0000000000000e07ULL );
static constexpr Bitboard   BlackPNMsk = ~BlackPRMask;

// promotion area for white RNK7 - RNK9
static constexpr Bitboard   WhitePRMask(
                                  0x70381c0e070381c0ULL,
                                  0x00000000000381c0ULL );
static constexpr Bitboard   WhitePNMask = ~WhitePRMask;

// the area not forcing promotion for FU andk KY : RNK2 - RNK3 for balack
// and RNK7 - RNK8 for white
static constexpr Bitboard   NFBFU(0x0180c06030180c06ULL,
                                  0x0000000000000c06ULL );
static constexpr Bitboard   NFWFU(0x30180c06030180c0ULL,
                                  0x00000000000180c0ULL );

// the area not forcing promotion for KE : RNK3 for balack and RNK7 for white
static constexpr Bitboard   NFBKE(0x0040201008040201ULL << 2,
                                  0x0000000000000201ULL << 2  );
static constexpr Bitboard   NFWKE(0x0040201008040201ULL << 6,
                                  0x0000000000000201ULL << 6  );

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
static const Bitboard &     (* PinDirection[])(Square::Square) = {
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
thread_local Array<Move::Move, Position::MinorMoves> Position::_move;

// Effect cache flag
thread_local Bitboard       Position::_effect;
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
   _kingSB(v._kingSB), _kingSW(v._kingSW), _ocupd(v._ocupd),
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
            auto p     = Piece::csaToPiece(pstr.substr(ptr, boardStrLen));
            auto c     = Piece::color(p);
            auto s     = r | f;
            _board[s]  = static_cast<unsigned char>(p);
            _bbord[p] ^= Bitboard::Square[s];
            _piece[c] ^= p != Piece::EMP ? Bitboard::Square[s] : Bitboard::Zero;
            _exchg    += _pieceVal[p];
        }
    }

    // occupied
    _ocupd = ~_bbord[Piece::EMP];

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
    _kingSB = _bbord[Piece::BOU].lsb();
    _kingSW = _bbord[Piece::WOU].lsb();

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
Bitboard Position::occupied (void) const
{

    return _ocupd;

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

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // number of moves
    --_numMoves;

    // make cache
    makeCheck();

}



/**
 * Perform a move, assuming the given move is always legal 
 * @param m move to perform
 * @return the performed move
 */
Move::Move Position::move (const Move::Move &m)
{

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

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // flip the turn
    _last              = _next;
    _next              = Color::flip(_next);

    // number of moves
    ++_numMoves;

    // square of OU
    _kingSB            = _bbord[Piece::BOU].lsb();
    _kingSW            = _bbord[Piece::WOU].lsb();

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

    // update whole hash
    _key               = _keyBoard ^ _keyHands;

    // number of moves
    --_numMoves;

    // square of OU
    _kingSB            = _bbord[Piece::BOU].lsb();
    _kingSW            = _bbord[Piece::WOU].lsb();

    // make check
    makeCheck();
 
}




/**
 * Perform a move (CSA protocol)
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
        return drop(Move::drop(pc, to));;
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
 * Moves for next player
 * @param m array to store moves
 *
 */
void Position::genMove (Array<Move::Move, Move::Max> &m)
{

    // move
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
    _move.setsz(0);

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
    _moveBOU((~_piece[Color::Black]), m);

    auto empty = ~_ocupd;
    _dropBFU(empty, m);
    _dropBKY(empty, m);
    _dropBKE(empty, m);
    _dropBOT(empty, m);

}



/**
 * Moves for white
 * @param m array to store moves
 *
 */
void Position::genMoveW (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    _move.setsz(0);

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

    auto empty = ~_ocupd;
    _dropWFU(empty, m);
    _dropWKY(empty, m);
    _dropWKE(empty, m);
    _dropWOT(empty, m);

}



/**
 * Minor moves
 * @param m array to store moves
 */
void Position::minorMove (Array<Move::Move, Move::Max> &m)
{

    // clear array for minor moves
    for (auto _m : _move) {
        m.add(_m);
    }

}



/**
 * Moves for next player (fast)
 * @param m array to store moves
 */
void Position::genFast (Array<Move::Move, Move::Max> &m)
{

    // move
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

    auto mask  = (~_piece[Color::Black]); 

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

    auto empty = ~_ocupd;
    _dropBFU(empty, m);
    _dropBKY(empty, m);
    _dropBKE(empty, m);
    _dropBOT(empty, m);

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

    auto mask  = (~_piece[Color::White]); 

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

    auto empty = ~_ocupd;
    _dropWFU(empty, m);
    _dropWKY(empty, m);
    _dropWKE(empty, m);
    _dropWOT(empty, m);

}



/**
 * Capturing moves for next player
 * @param m array to store moves
 */
void Position::genCapt (Array<Move::Move, Move::Max> &m)
{

    // ordinary move
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
 * Move BFU
 * @param m array to store moves
 */
void Position::_moveBFU (Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::BFU] & _pinnd) >> 1) & (~_piece[Color::Black]);
    auto pr = ef & BlackPRMask;
    auto np = ef & NFBFU;

    ef &= BlackPNMsk;
    while (ef) {
        auto sq = ef.pick();
        m.    add(Move::move   (sq + Square::DWARD, sq));
    }
    while (np) {
        auto sq = np.pick();
        _move.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (pr) {
        auto sq = pr.pick();
        m.    add(Move::promote(sq + Square::DWARD, sq));
    }

}



/**
 * Move BFU (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::BFU] & _pinnd) >> 1) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto sq = ef.pick();
        m.add(Move::move   (sq + Square::DWARD, sq));
    }
    while (pr) {
        auto sq = pr.pick();
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
    auto pr = ef & WhitePRMask;
    auto np = ef & NFWFU;

    ef &= WhitePNMask;
    while (ef) {
        auto sq = ef.pick();
        m.    add(Move::move   (sq + Square::UWARD, sq));
    }
    while (np) {
        auto sq = np.pick();
        _move.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (pr) {
        auto sq = pr.pick();
        m.    add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Move WFU (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWFU (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto ef = ((_bbord[Piece::WFU] & _pinnd) << 1) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto sq = ef.pick();
        m.add(Move::move   (sq + Square::UWARD, sq));
    }
    while (pr) {
        auto sq = pr.pick();
        m.add(Move::promote(sq + Square::UWARD, sq));
    }

}



/**
 * Move BKY
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_moveBKY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KB(sq, _ocupd) & (~_piece[Color::Black]);
        auto pr = ef & BlackPRMask;
        auto np = ef & NFBFU;

        ef &= BlackPNMsk;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move BKY (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastBKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KB(sq, _ocupd) & mask;
        auto pr = ef & BlackPRMask;
        auto np = ef & NFBFU;

        ef &= BlackPNMsk;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move WKY
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_moveWKY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KW(sq, _ocupd) & (~_piece[Color::White]);
        auto pr = ef & WhitePRMask;
        auto np = ef & NFWFU;

        ef &= WhitePNMask;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move WKY (fast)
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKY (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKY] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::KW(sq, _ocupd) & mask;
        auto pr = ef & WhitePRMask;
        auto np = ef & NFWFU;

        ef &= WhitePNMask;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

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
        auto pr = ef & BlackPRMask;
        auto np = ef & NFBKE;

        ef &= BlackPNMsk;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move BKE (fast)
 * @param m array to store moves
 */
void Position::_fastBKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BKE] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::BKE) & mask;
        auto pr = ef & BlackPRMask;
        auto np = ef & NFBKE;

        ef &= BlackPNMsk;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move WKE
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_moveWKE (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKE] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WKE) & (~_piece[Color::White]);
        auto pr = ef & WhitePRMask;
        auto np = ef & NFWKE;

        ef &= WhitePNMask;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move WKE
 * @param mask mask of effect
 * @param m array to store moves
 */
void Position::_fastWKE (const Bitboard &mask, Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::WKE] & _pinnd;

    while (pp) {
        auto sq = pp.pick();
        auto ef = Effect::AD(sq, Piece::WKE) & mask;
        auto pr = ef & WhitePRMask;
        auto np = ef & NFWKE;

        ef &= WhitePNMask;
        while (ef) {
            auto s = ef.pick();
            m.add(Move::move   (sq, s));
        }
        while (np) {
            auto s = np.pick();
            m.add(Move::move   (sq, s));
        }
        while (pr) {
            auto s = pr.pick();
            m.add(Move::promote(sq, s));
        }
    }

}



/**
 * Move BGI
 * @param m array to store moves
 */
void Position::_moveBGI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BGI] & _pinnd;
    auto pc = p & BlackPNMsk;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBGI((~_piece[Color::Black]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtBGI((~_piece[Color::Black]), sq, m);
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
    auto pc = p & BlackPNMsk;
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
        _moveWGI((~_piece[Color::White]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _prmtWGI((~_piece[Color::White]), sq, m);
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
 * Move BGI with mask
 * @param mask mask of effect
 * @param sq   square of BGI
 * @param m array to store moves
 */
void Position::_moveBGI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::BGI) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::move   (sq, s));
        m.add(Move::promote(sq, s));
    }

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

    auto ef = Effect::AD(sq, Piece::WGI) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::move   (sq, s));
        m.add(Move::promote(sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
        m.add(Move::promote(sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
        m.add(Move::promote(sq, s));
    }

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
        _moveBKI((~_piece[Color::Black]), sq, m);
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
        _moveWKI((~_piece[Color::White]), sq, m);
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
 * Move BKI with mask
 * @param mask mask of effect
 * @param sq   square of BKI
 * @param m array to store moves
 */
void Position::_moveBKI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::AD(sq, Piece::BKI) & mask;

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }

}



/**
 * Move BUM
 * @param m array to store moves
 */
void Position::_moveBUM (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BUM] & _pinnd;

    while (pp) {
        auto sq  = pp.pick();
        _moveUM((~_piece[Color::Black]), sq, m);
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
        auto sq  = pp.pick();
        _moveUM(mask, sq, m);
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
        auto sq  = pp.pick();
        _moveUM((~_piece[Color::White]), sq, m);
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
        auto sq  = pp.pick();
        _moveUM(mask, sq, m);
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

    auto ef = (Effect::AD(sq, Piece::BUM) | Effect::KA(sq, _ocupd)) & mask;

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }

}



/**
 * Move BRY
 * @param m array to store moves
 */
void Position::_moveBRY (Array<Move::Move, Move::Max> &m)
{

    auto pp = _bbord[Piece::BRY] & _pinnd;

    while (pp) {
        auto sq  = pp.pick();
        _moveRY((~_piece[Color::Black]), sq, m);
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
        auto sq  = pp.pick();
        _moveRY(mask, sq, m);
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
        auto sq  = pp.pick();
        _moveRY((~_piece[Color::White]), sq, m);
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
        auto sq  = pp.pick();
        _moveRY(mask, sq, m);
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

    auto ef = (Effect::AD(sq, Piece::BRY) | Effect::HI(sq, _ocupd)) & mask;

    while (ef) {
        auto s = ef.pick();
        m.add(Move::move(sq, s));
    }

}



/**
 * Move BKA
 * @param m array to store moves
 */
void Position::_moveBKA (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BKA] & _pinnd;
    auto pc = p & BlackPNMsk;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBKA((~_piece[Color::Black]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtKA((~_piece[Color::Black]), sq, m);
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
    auto pc = p & BlackPNMsk;
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
        _moveWKA((~_piece[Color::White]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtKA((~_piece[Color::White]), sq, m);
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
 * Move BKA with mask 
 * @param mask mask of effect
 * @param sq   square of BKA
 * @param m array to store moves
 */
void Position::_moveBKA (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::KA(sq, _ocupd) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto s = ef.pick();
        m.    add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    auto ef = Effect::KA(sq, _ocupd) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::promote(sq, s));
    }

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

    auto ef = Effect::KA(sq, _ocupd) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto s = ef.pick();
        m.    add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    auto ef = Effect::KA(sq, _ocupd) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::promote(sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.add(Move::promote(sq, s));
    }

}



/**
 * Move BHI
 * @param m array to store moves
 */
void Position::_moveBHI (Array<Move::Move, Move::Max> &m)
{

    auto p  = _bbord[Piece::BHI] & _pinnd;
    auto pc = p & BlackPNMsk;
    auto pp = p & BlackPRMask;

    while (pc) {
        auto sq = pc.pick();
        _moveBHI((~_piece[Color::Black]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtHI((~_piece[Color::Black]), sq, m);
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
    auto pc = p & BlackPNMsk;
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
        _moveWHI((~_piece[Color::White]), sq, m);
    }
    while (pp) {
        auto sq = pp.pick();
        _promtHI((~_piece[Color::White]), sq, m);
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
 * Move BHI with mask 
 * @param mask mask of effect
 * @param sq   square of BHI
 * @param m array to store moves
 */
void Position::_moveBHI (const Bitboard &mask, Square::Square sq,
                         Array<Move::Move, Move::Max> &m          )
{

    auto ef = Effect::HI(sq, _ocupd) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto s = ef.pick();
        m.    add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    auto ef = Effect::HI(sq, _ocupd) & mask;
    auto pr = ef & BlackPRMask;

    ef &= BlackPNMsk;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::promote(sq, s));
    }

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

    auto ef = Effect::HI(sq, _ocupd) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto s = ef.pick();
        m.    add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    auto ef = Effect::HI(sq, _ocupd) & mask;
    auto pr = ef & WhitePRMask;

    ef &= WhitePNMask;
    while (ef) {
        auto s = ef.pick();
        m.add(Move::move   (sq, s));
    }
    while (pr) {
        auto s = pr.pick();
        m.add(Move::promote(sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.    add(Move::promote(sq, s));
        _move.add(Move::move   (sq, s));
    }

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

    while (ef) {
        auto s = ef.pick();
        m.add(Move::promote(sq, s));
    }

}



/**
 * Move pinned piece for black
 * @param m array to store moves
 */
void Position::_movePinB (Array<Move::Move, Move::Max> &m)
{

    using namespace Piece;

    // check if any pinned piece 
    auto pinned = _pinnd ^ Bitboard::Fill;
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
        auto mk = PinDirection[dr](sq) & (~_piece[Color::Black]);
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
        auto mk = PinDirection[dr](sq) & (~_piece[Color::Black]);
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
    auto pinned = _pinnd ^ Bitboard::Fill;
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
        auto mk = PinDirection[dr](sq) & (~_piece[Color::White]);
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
        auto mk = PinDirection[dr](sq) & (~_piece[Color::White]);
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
    _moveBTo(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropBFU(_chkDE, m);
    _dropBKY(_chkDE, m);
    _dropBKE(_chkDE, m);
    _dropBOT(_chkDE, m);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _moveBTo(mask, s, m);
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
    _fastBTo(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropBFU(_chkDE, m);
    _dropBKY(_chkDE, m);
    _dropBKE(_chkDE, m);
    _dropBOT(_chkDE, m);

    // move the piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _fastBTo(mask, s, m);
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
    _moveWTo(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropWFU(_chkDE, m);
    _dropWKY(_chkDE, m);
    _dropWKE(_chkDE, m);
    _dropWOT(_chkDE, m);

    // move a piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _moveWTo(mask, s, m);
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
    _fastWTo(mask, _chckp.lsb(), m);

    // drop a piece to block
    _dropWFU(_chkDE, m);
    _dropWKY(_chkDE, m);
    _dropWKE(_chkDE, m);
    _dropWOT(_chkDE, m);

    // move the piece to block
    auto sq = _chkDE;
    while (sq) {
        auto s = sq.pick();
        _fastWTo(mask, s, m);
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
 * Let BOU escape from checks
 * @param m array to store moves
 */
void Position::_escapeB (Array<Move::Move, Move::Max> &m)
{

    // current position of OU to move
    auto current = _kingSB;

    // make direction mask
    auto dirmask = (~_piece[Color::Black]) & _escape(current);

    // move
    _moveBOU(dirmask, m);

}



/**
 * Let WOU escape from checks
 * @param m array to store moves
 */
void Position::_escapeW (Array<Move::Move, Move::Max> &m)
{

    // current position of OU to move
    auto current  = _kingSW;

    // make direction mask
    auto dirmask = (~_piece[Color::White]) & _escape(current);

    // move
    _moveWOU(dirmask, m);

}



/**
 * Move the black piece from square to square
 * @parm from square move from
 * @parm to   square move to
 * @parm m array to store moves
 */
void Position::_moveBlack (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::BFU:
        if (BFUMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            _move.add(Move::move   (from, to)); 
            m    .add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKY:
        if (BFUMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKE:
        if (BKEMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (BlackCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::BGI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::BKA:
    case Piece::BHI:
        if (BlackCanPromote[from] || BlackCanPromote[to]) {
            _move.add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Move the black piece from square to square (fast)
 * @parm from square move from
 * @parm to   square move to
 * @parm m array to store moves
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
            m.    add(Move::move   (from, to)); 
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
 * @parm from square move from
 * @parm to   square move to
 * @parm m array to store moves
 */
void Position::_moveWhite (Square::Square from, Square::Square to,
                           Array<Move::Move, Move::Max> &m         )
{

    switch (_board[from]) {
    case Piece::WFU:
        if (WFUMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            _move.add(Move::move   (from, to)); 
            m    .add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKY:
        if (WFUMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKE:
        if (WKEMPromote[to]) {
            m.    add(Move::promote(from, to)); 
        } else
        if (WhiteCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::WGI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
            m.    add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    case Piece::WKA:
    case Piece::WHI:
        if (WhiteCanPromote[from] || WhiteCanPromote[to]) {
            _move.add(Move::move   (from, to)); 
            m.    add(Move::promote(from, to)); 
        } else {
            m.    add(Move::move   (from, to)); 
        }
        break;
    default:
        m.add(Move::move(from, to)); 
    }

}



/**
 * Move the white piece from square to square (fast)
 * @parm from square move from
 * @parm to   square move to
 * @parm m array to store moves
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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
    while (sq) {
        auto s = sq.pick();
        m.add(Move::drop(h, s));
    }

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
 * All the moves to certain square
 * @param mask mask
 * @param dst  destination
 * @param m    array to store moves
 */
void Position::_moveBTo (const Bitboard &mask, Square::Square dst,
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
void Position::_fastBTo (const Bitboard &mask, Square::Square dst,
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
void Position::_moveWTo (const Bitboard &mask, Square::Square dst,
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
void Position::_fastWTo (const Bitboard &mask, Square::Square dst,
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
    if ((Effect::KY(Color::White, sq, _ocupd) & _bbord[aky]))   {
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
    if ((Effect::KY(Color::Black, sq, _ocupd) & _bbord[aky]))   {
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
    auto ky = Effect::KY(Color::White, sq, _ocupd) & _bbord[aky];
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
    auto ky = Effect::KY(Color::Black, sq, _ocupd) & _bbord[aky];
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
        auto yk  = Effect::KY(Color::White, sq, _ocupd);
        auto yo  = Effect::KY(Color::Black, guard, _ocupd);
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
        auto yk  = Effect::KY(Color::Black, sq, _ocupd);
        auto yo  = Effect::KY(Color::White, guard, _ocupd);
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

    Bitboard                check;

    // clear
    _nchek        = 0;
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

    Bitboard                check;

    // clear
    _nchek        = 0;
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

