#include "MyPosition.h"

using namespace foundation;
using namespace game;

/**
 * All pieces giving check
 * @return check bitboard
 */
const Bitboard & MyPosition::check (void) const
{

    return _chckp;

}



/**
 * Check effect on OU of last player
 * @return check bitboard
 */
const Bitboard & MyPosition::recheck (void)
{

    _last = _next;
    _next = Color::flip(_last);
    makeCheck();
    return _chckp;

}



/**
 * Adjacent pieces giving check
 * @return check bitboard
 */
const Bitboard & MyPosition::checkAD (void) const
{

    return _chkAD;

}



/**
 * KY giving check
 * @return check bitboard
 */
const Bitboard & MyPosition::checkKY (void) const
{

    return _chkKY;

}



/**
 * KA giving check
 * @return check bitboard
 */
const Bitboard & MyPosition::checkKA (void) const
{

    return _chkKA;

}



/**
 * HI giving check
 * @return check bitboard
 */
const Bitboard & MyPosition::checkHI (void) const
{

    return _chkHI;

}



/**
 * Pin
 * @return Pin
 */
const Bitboard & MyPosition::pin (void)
{

    _pinnd = Bitboard::Fill;

    if (_next == Color::Black) {
        _makePinB();
    } else {
        _makePinW();
    }
    return _pinnd;

}



/**
 * Check Hash
 * @return true if valid, false otherwise
 */
bool MyPosition::chkHash (void)
{

    Zobrist::key key   = _key;
    Zobrist::key board = _keyBoard;
    Zobrist::key hands = _keyHands;

    _hashFull();

    if (key   != _key)      {
        return false;
    }
    if (board != _keyBoard) {
        return false;
    }
    if (hands != _keyHands) {
        return false;
    }

    return true;

}



/**
 * Check identical
 * @return true if identical, false otherwise
 */
bool MyPosition::operator== (const MyPosition &rhs)
{

    // ocupd square
    if (_ocupd != rhs._ocupd) {
        return false;
    }

    // exchange value
    if (_exchg != rhs._exchg) {
        return false;
    }

    // square of OU
    if (_kingSB != rhs._kingSB) {
        return false;
    }
    if (_kingSW != rhs._kingSW) {
        return false;
    }

    // the board
    for (auto sq : Square::all) {
        if (_board[sq] != rhs._board[sq]) {
            return false;
        }
    }

    // pieces in the hands
    for (auto p : Piece::hand) {
        if (_hands[Color::Black][p] != rhs._hands[Color::Black][p]) {
            return false;
        }
        if (_hands[Color::White][p] != rhs._hands[Color::White][p]) {
            return false;
        }
    }

    // the bitboards
    for (auto p : Piece::all) {
        if (_bbord[p] != rhs._bbord[p]) {
            return false;
        }
    }

    // occupied square
    if (_piece[Color::Black] != rhs._piece[Color::Black]) {
            return false;
    }
    if (_piece[Color::White] != rhs._piece[Color::White]) {
            return false;
    }

    return true;

}



/**
 * Check identical
 * @return false if identical, true otherwise
 */
bool MyPosition::operator!= (const MyPosition &rhs)
{

    return (! operator==(rhs));

}




/**
 * Assign
 * @param rhs value to assign
 * @return assigned value
 */
MyPosition & MyPosition::operator= (const MyPosition &rhs)
{

    Position::operator=(rhs);
    return *this;

}

