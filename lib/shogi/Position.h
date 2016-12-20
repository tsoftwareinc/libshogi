/**
 *****************************************************************************

 @file       Position.h

 @brief      Position definitions
  
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

#ifndef _GAME_POSITION_H
#define _GAME_POSITION_H

#include <Common.h>

#include <Array.h>
#include <CSASummary.h>
#include <Square.h>
#include <Bitboard.h>
#include <Zobrist.h>
#include <Evaluation.h>
#include <Move.h>

// begin namespace 'game'
namespace game {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _GAME_POSITION_DEBUG
#define _GAME_POSITION_CHECK(x) { assert ( (x) ); }
#define _GAME_POSITION_DEBUG_ERROR_STRING_MAX 256
#define _GAME_POSITION_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "GAME_POSITION_DEBUG     : " fmt, ## args); \
        }
#define _GAME_POSITION_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_GAME_POSITION_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _GAME_POSITION_CHECK(x)
#define _GAME_POSITION_DEBUG_OUT(fmt, args...)
#define _GAME_POSITION_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ---------------------------- Position class ----------------------------- */

/**
 *  Position of the game
 *
 */
class Position
{

public:

    /// Set piece values
    static void                 setValue   (const Evaluation::Eval
                                                  (& v)[Piece::Pieces   ]);

    /// Set hands values
    static void                 handsValue (const Evaluation::Eval
                                                  (& v)[Piece::Depth + 1]
                                                       [Piece::Kind     ]);


    // default constructor
    Position () {}

    /// Copy constructor avoiding copying context cache
    Position (const Position &);

    /// Constructor takes CSA game summary
    Position (const CSASummary &);

    /// Destructor
    virtual ~Position () {}

    /// Reset to the first position
    void                        init       (void);

    /// Whole hash key
    Zobrist::key                hash       (void)           const;

    /// Occupied squares
    Bitboard                    occupied   (void)           const;

    /// Get exchange value
    Evaluation::Eval            exchange   (void)           const;

    /// Get current turn
    Color::Color                turn       (void)           const;

    /// Piece in the hands
    int                         hand       (Color::Color,
                                            Piece::Piece  ) const;

    /// Piece on the board
    Piece::Piece                square     (Square::Square) const;

    /// Bitboard
    const Bitboard &            bitboard   (Piece::Piece  ) const;

    /// Occupied squares by color
    const Bitboard &            occupied   (Color::Color  ) const;


    /// Show the board
    void                        show       (void)           const;

    /// Show the move 
    void                        show       (Move::Move)     const;

    /// Move string
    std::string                 string     (Move::Move)     const;

    /// Evaluate current position
    virtual Evaluation::Eval    eval       (void);

    /// Set current turn
    void                        turn       (Color::Color);

    /// Perform a dropping move
    Move::Move                  drop       (const Move::Move &);

    /// Undo the last dropping move
    void                        remove     (const Move::Move &);

    /// Make a move
    Move::Move                  move       (const Move::Move &);

    /// Undo the last move
    void                        undo       (const Move::Move &);

    /// Make a move (CSA protocol)
    Move::Move                  move       (const CSAMove    &);

    /// Check if OU will be captured
    bool                        dusty      (const Move::Move &) const;
    bool                        dustyB     (const Move::Move &) const;
    bool                        dustyW     (const Move::Move &) const;


    /// Make check
    void                        makeCheck  (void);

    /// Generate moves
    void                        genMove    (foundation::Array<Move::Move, Move::Max> &);

    /// Generate moves for Black
    void                        genMoveB   (foundation::Array<Move::Move, Move::Max> &);

    /// Generate moves for White
    void                        genMoveW   (foundation::Array<Move::Move, Move::Max> &);

    /// Checkout minor moves
    void                        minorMove  (foundation::Array<Move::Move, Move::Max> &);

    /// Generate moves fast
    void                        genFast    (foundation::Array<Move::Move, Move::Max> &);

    /// Generate moves for Black fast
    void                        genFastB   (foundation::Array<Move::Move, Move::Max> &);

    /// Generate moves for White fast
    void                        genFastW   (foundation::Array<Move::Move, Move::Max> &);


    /// Generate capturing moves
    void                        genCapt    (foundation::Array<Move::Move, Move::Max> &);

    /// Generate capturing moves for Black
    void                        genCaptB   (foundation::Array<Move::Move, Move::Max> &);

    /// Generate capturing moves for White
    void                        genCaptW   (foundation::Array<Move::Move, Move::Max> &);


    /// Number of moves so far
                                operator int() const;

    /// Assign avoiding copying context cache
    Position &                  operator=  (const Position &);

    /// Print the board
    friend std::ostream &       operator<< (std::ostream &os, const Position &);


protected:

    /// Hash for pieces on board
    Zobrist::key                _keyBoard;

    /// Hash for pieces in hands
    Zobrist::key                _keyHands;

    /// Hash for both the board and the hands
    Zobrist::key                _key;

    /// Square of BOU
    Square::Square              _kingSB;

    /// Square of WOU
    Square::Square              _kingSW;

    /// Pieces on the board
    unsigned char               _board[Square::SQVD];

    /// Pieces in hand
    int                         _hands[Color::Colors][Piece::Kind];

    /// Bitboards
    Bitboard                    _bbord[Piece::Pieces];

    /// Occupied square by color
    Bitboard                    _piece[Color::Colors];

    /// Occupied square
    Bitboard                    _ocupd;

    /// Exchange value
    Evaluation::Eval            _exchg;

    /// Turn moved last
    Color::Color                _last;

    /// Turn to move next
    Color::Color                _next;

    /// Number of moves
    int                         _numMoves;



    ///
    /// Value of pieces
    ///

    /// Value of the pieces on the board
    static Evaluation::Eval     _pieceVal[Piece::Pieces];

    /**
        Value of the pieces in the hands

        handsVal[Depth][Color][Piece]

        Depth is a number of same pieces in hands. 18 at most,
        but, is actually 32 here to align on memory boundary and
        to handle captured EMPs.
    */
    static const int            EMPHandsMask = 31;
    static Evaluation::Eval     _handsVal[EMPHandsMask + 1]
                                         [Color::Colors][Piece::Kind];


    ///
    /// Context cache on thread local storage
    ///
#ifdef USE_THREADLOCALSTORAGE
#define _POSISION_CONTEXTCACHE static thread_local
#else
#define _POSISION_CONTEXTCACHE
#endif
    /// Pinned pieces
    _POSISION_CONTEXTCACHE Bitboard _pinnd;

    /// Number of checks
    _POSISION_CONTEXTCACHE int      _nchek;

    /// All pieces giving check
    _POSISION_CONTEXTCACHE Bitboard _chckp;

    /// Adjacent pieces giving check
    _POSISION_CONTEXTCACHE Bitboard _chkAD;

    /// KY giving check
    _POSISION_CONTEXTCACHE Bitboard _chkKY;

    /// KA giving check
    _POSISION_CONTEXTCACHE Bitboard _chkKA;

    /// HI giving check
    _POSISION_CONTEXTCACHE Bitboard _chkHI;

    /// Distant effect for intermediate blocking
    _POSISION_CONTEXTCACHE Bitboard _chkDE;

    /// Minor moves
    static const           int      MinorMoves = 64;
    _POSISION_CONTEXTCACHE foundation::Array<Move::Move, MinorMoves> _move;

    /// Effect
    _POSISION_CONTEXTCACHE Bitboard _effect;



    ///
    /// Functions 
    ///

    /// Clear the Kyokumen
    void                        _clear      (void);

    /// Move FU
    void                        _moveBFU    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBFU    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWFU    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWFU    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move KY
    void                        _moveBKY    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBKY    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKY    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWKY    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move KE  
    void                        _moveBKE    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBKE    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKE    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWKE    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
 
    /// Move GI  
    void                        _moveBGI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBGI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWGI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWGI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move GI with mask
    void                        _moveBGI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWGI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _prmtBGI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _prmtWGI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move KI, TO, NY, NK and NG
    void                        _moveBKI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBKI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWKI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move KI, TO, NY, NK and NG with mask
    void                        _moveBKI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move UM
    void                        _moveBUM    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBUM    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWUM    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWUM    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
 
    /// Move UM with mask
    void                        _moveUM     (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move RY
    void                        _moveBRY    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBRY    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWRY    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWRY    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
 
    /// Move RY with mask
    void                        _moveRY     (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move KA
    void                        _moveBKA    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBKA    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKA    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWKA    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
 
    /// Move KA and UM with mask
    void                        _moveBKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _promtKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _pfastKA    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move HI and RY
    void                        _moveBHI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBHI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWHI    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWHI    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move HI and RY with mask
    void                        _moveBHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _promtHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _pfastHI    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Move pinned piece 
    void                        _movePinB   (foundation::Array<Move::Move, Move::Max> &);
    void                        _movePinW   (foundation::Array<Move::Move, Move::Max> &);

    /// Move OU (not in check)
    void                        _moveBOU    (foundation::Array<Move::Move, Move::Max> &);
    void                        _moveBOU    (const Bitboard &mask,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWOU    (foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWOU    (const Bitboard &mask,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Get out of check
    void                        _getOutB    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fstOutB    (foundation::Array<Move::Move, Move::Max> &);
    void                        _getOutW    (foundation::Array<Move::Move, Move::Max> &);
    void                        _fstOutW    (foundation::Array<Move::Move, Move::Max> &);

    /// Escape from checks
    Bitboard                    _escape     (Square::Square);
    void                        _escapeB    (foundation::Array<Move::Move, Move::Max> &);
    void                        _escapeW    (foundation::Array<Move::Move, Move::Max> &);


    /// Moves piece to the square from the square 
    void                        _moveBlack  (Square::Square from, Square::Square to,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBlack  (Square::Square from, Square::Square to,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWhite  (Square::Square from, Square::Square to,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWhite  (Square::Square from, Square::Square to,
                                             foundation::Array<Move::Move, Move::Max> &);


    /// Drop FU
    void                        _dropBFU    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _dropWFU    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Drop KY
    void                        _dropBKY     (const Bitboard &,
                                              foundation::Array<Move::Move, Move::Max> &);
    void                        _dropWKY     (const Bitboard &,
                                              foundation::Array<Move::Move, Move::Max> &);

    /// Drop KE
    void                        _dropBKE    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _dropWKE    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// Drop others
    void                        _dropBOT    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _dropWOT    (const Bitboard &,
                                             foundation::Array<Move::Move, Move::Max> &);

    /// List all the move to certain square (except for OU)
    void                        _moveBTo    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastBTo    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _moveWTo    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);
    void                        _fastWTo    (const Bitboard &, Square::Square,
                                             foundation::Array<Move::Move, Move::Max> &);


    /// Check if any of the colored pieces reach to the square
    bool                        _chkEffectB (Square::Square);
    bool                        _chkEffectW (Square::Square);

    /// Check all the colored pieces reach to the square
    void                        _allEffectB (Square::Square);
    void                        _allEffectW (Square::Square);

    /// Make pin
    void                        _makePinB   (void);
    void                        _makePinW   (void);

    /// Make check
    void                        _makeCheckB (void);
    void                        _makeCheckW (void);

    /// Calculate hash
    void                        _hashFull   (void);

};

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

#endif
