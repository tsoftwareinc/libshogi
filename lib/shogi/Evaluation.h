/**
 *****************************************************************************

 @file       Evaluation.h

 @brief      Definition of type of Evaluation
  
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

#ifndef _GAME_EVALUATION_H
#define _GAME_EVALUATION_H

#include <Common.h>
#include <Piece.h>

// begin namespace 'game::Evaluation'
namespace game { namespace Evaluation {

/* ------------------------------ definitions ------------------------------ */

/// Evaluation
using Eval = int;

/// Infinite value
const Eval                  Infinity = 9999;

/// Comparison
inline Eval                 max (Eval v1, Eval v2) {
                                return (v1 > v2 ? v1 : v2);
                            }

inline Eval                 min (Eval v1, Eval v2) {
                                return (v1 < v2 ? v1 : v2);
                            }

/// Value of the pieces on board
static const Eval           Value[Piece::Pieces] = {
                            //  EMP   BFU   BKY   BKE   BGI   BKA   BHI   BKI
                                  0,  100,  200,  300,  400,  800, 1000,  500,
                            //  BOU   BTO   BNY   BNK   BNG   BUM   BRY
                                  0,  500,  500,  500,  500, 1300, 1500,    0,
                            //        WFU   WKY   WKE   WGI   WKA   WHI   WKI
                                  0, -100, -200, -300, -400, -800,-1000, -500,
                            //  WOU   WTO   WNY   WNK   WNG   WUM   WRY
                                  0, -500, -500, -500, -500,-1300,-1500,    0
                            };

/// Number of pieces
/// Value of the pieces in hands
static const Eval           Hands[Piece::Depth + 1][Piece::Kind] = {
                            //  EMP    FU    KY    KE    GI    KA    HI    KI
                             {    0,    0,    0,    0,    0,    0,    0,    0},
                             {    0,  100,  200,  300,  400,  800, 1000,  500},
                             {    0,  200,  400,  600,  800, 1600, 2000, 1000},
                             {    0,  300,  600,  900, 1200,    0,    0, 1500},
                             {    0,  400,  800, 1200, 1600,    0,    0, 2000},
                             {    0,  500,    0,    0,    0,    0,    0,    0},
                             {    0,  600,    0,    0,    0,    0,    0,    0},
                             {    0,  700,    0,    0,    0,    0,    0,    0},
                             {    0,  800,    0,    0,    0,    0,    0,    0},
                             {    0,  900,    0,    0,    0,    0,    0,    0},
                             {    0, 1000,    0,    0,    0,    0,    0,    0},
                             {    0, 1100,    0,    0,    0,    0,    0,    0},
                             {    0, 1200,    0,    0,    0,    0,    0,    0},
                             {    0, 1300,    0,    0,    0,    0,    0,    0},
                             {    0, 1400,    0,    0,    0,    0,    0,    0},
                             {    0, 1500,    0,    0,    0,    0,    0,    0},
                             {    0, 1600,    0,    0,    0,    0,    0,    0},
                             {    0, 1700,    0,    0,    0,    0,    0,    0},
                             {    0, 1800,    0,    0,    0,    0,    0,    0}
                            };

/* ------------------------------------------------------------------------- */

// end namespace 'game::Evaluation'
} }

#endif
