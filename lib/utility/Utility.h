/**
 *****************************************************************************

 @file       Utility.h

 @brief      Common utility programs definitions

 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-28

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
  28th Mar 2014  0.1    Initial release.

 *****************************************************************************/

#ifndef _UTILITY_H
#define _UTILITY_H

#include <Common.h>
#include <BTree.h>


// begin namespace 'utility'
namespace utility {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _UTILITY_DEBUG
#define _UTILITY_CHECK(x) { assert ( (x) ); }
#define _UTILITY_DEBUG_ERROR_STRING_MAX 256
#define _UTILITY_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "UTILITY_DEBUG     : " fmt, ## args); \
        }
#define _UTILITY_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_UTILITY_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _UTILITY_CHECK(x)
#define _UTILITY_DEBUG_OUT(fmt, args...)
#define _UTILITY_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* -------------------------- function prototypes -------------------------- */

void        initRandomTree (void);
void        initRandom (int);
void        initRandom (void);
uint32_t    random (void);
uint64_t    Random (void);
uint64_t    uniqueRandom (void);
void        cleanRandomTree (void);

/* ------------------------------------------------------------------------- */

// endo of name space 'utility'
}

#endif
