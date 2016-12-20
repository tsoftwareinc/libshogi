/**
 *****************************************************************************

 @file       Zobrist.cpp

 @brief      Zobrist implementation
  
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

#include <Zobrist.h>
#include <Utility.h>

// begin namespace 'game'
namespace game {

/* ---------------------------- global variables --------------------------- */

// Random seed
static const int            _seed = 20160331;

// Tables for random numbers to hash pieces on the board
Zobrist::key                Zobrist::position[Square::Squares][Piece::Pieces ];

// Tables for random numbers to hash pieces in the hand 
Zobrist::key                Zobrist::hands   [Color ::Colors ][Piece::Variety];

/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */


/**
 * The conversion from uniqueRandom() to key
 * @parm d a return from uniqueRandom()
 * @return key
 *
 */
static Zobrist::key _conv(uint64_t d)
{

    return static_cast<Zobrist::key>(d);

}



/**
 * Initialize the tables of random numbers
 *
 */
void Zobrist::initialize (void)
{

    // initialize random generator
    utility::initRandom(_seed);

    // initialize random tree
    utility::initRandomTree();

    //
    for (auto sq : Square::all) {
        for (auto p : Piece::all) {
            position[sq][p] = _conv(utility::uniqueRandom());
        }
        position[sq][Piece::EMP] = 0;
    }

    for (auto c : Color::all) {
        for (auto p : Piece::hand) {
            hands[c][p]     = _conv(utility::uniqueRandom());
        }
        hands[c][Piece::EMP]     = 0;
    }

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}
