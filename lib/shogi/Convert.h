/**
 *****************************************************************************

 @file       Convert.h

 @brief      Definitions for conversion beyond the types
  
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

#ifndef _GAME_CONVERT_H
#define _GAME_CONVERT_H

#include <Square.h>
#include <Region.h>

// begin namespace 'game::Convert'
namespace game { namespace Convert {

/* ------------------------------ definitions ------------------------------ */

/**
 * Convert Region to Square
 * @param r Region
 * @return corresponding Square
 */
inline Square::Square toSquare(Region::Region r)
{

    return Square::regionToSquare[r];

}



/**
 * Convert Square to Region
 * @param s Square
 * @return corresponding Region
 */
inline Region::Region toRegion (Square::Square s)
{

    return Region::squareToRegion[s];

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Convert'
} }

#endif
