/**
 *****************************************************************************

 @file       CSASummary.h

 @brief      CSA game summary definitions
  
 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

 @date       2014-03-28

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
  28th Apr 2014  0.1   Initial release
 
 *****************************************************************************/ 

#ifndef _GAME_CSASUMMARY_H
#define _GAME_CSASUMMARY_H

#include <iostream>
#include <string>
#include <List.h>
#include <Array.h>

// begin namespace 'game'
namespace game {

/* ----------------------------- CSAMove  class ----------------------------- */

struct CSAMove {

    CSAMove (const char * const str) : move(str) {}
    CSAMove (std::string        str) : move(str) {}

    friend std::ostream &   operator<< (std::ostream &, const CSAMove &);
    std::string             move;

};

/* ------------------------------------------------------------------------- */



/* --------------------------- CSASummary  class --------------------------- */

struct CSASummary {

    /// Board size
    static const int    BoardSize = 9;

    /// Game summary
    std::string         version;
    std::string         mode;
    std::string         format;
    std::string         declaration;
    std::string         gameID;
    std::string         senteName;
    std::string         goteName;
    std::string         yourTurn;
    std::string         rematch;
    std::string         toMove;
    int                 maxMoves;

    /// Time setting
    std::string         unitString;
    int                 unit;
    enum {
        MSECND,
        SECOND,
        MINUIT
    }                   timeUnit;
    int                 leastTime;
    std::string         timeRoundup;
    int                 totalTime;
    int                 byoyomi;
    int                 delay;
    int                 increment;

    /// Initial position
    foundation::
    Array<std::string, BoardSize>
                        position;
    std::string         senteHand;
    std::string         goteHand;
    std::string         firstPlayer;
    foundation::
    List<CSAMove>       sequence;

};

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

#endif
