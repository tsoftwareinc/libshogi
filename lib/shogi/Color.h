/**
 *****************************************************************************

 @file       Color.h

 @brief      Definition of the color (player's turn)
  
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

#ifndef _GAME_COLOR_H
#define _GAME_COLOR_H

#include <Common.h>

// begin namespace 'game::Color'
namespace game { namespace Color {

/* ------------------------------ definitions ------------------------------ */

// Players color
using Color = int;
enum : Color {

    /// Black to play first
#define COLOR_ALL \
    Black, \
    White
    COLOR_ALL,

    /// Number of colors
    Colors,

};

static const Color          all[] = { COLOR_ALL };



/**
* Flip the color
* @param c color to be fipped
* @return  fliped color
*/
inline Color::Color         flip(Color c)
{

    return (c ^ White);

}


/* ------------------------------------------------------------------------- */

// end namespace 'game::Color'
} }

#endif
