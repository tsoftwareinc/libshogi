/**
 *****************************************************************************

 @file       CSAFile.h

 @brief      CSA file reading
  
 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

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
  28th Apr 2014  0.1   Initial release
 
 *****************************************************************************/ 

#ifndef _GAME_CSAFILE_H
#define _GAME_CSAFILE_H

#include <Common.h>
#include <string>

#include <CSASummary.h>

// begin namespace 'game'
namespace game {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _GAME_CSAFILE_DEBUG
#define _GAME_CSAFILE_CHECK(x) { assert ( (x) ); }
#define _GAME_CSAFILE_DEBUG_ERROR_STRING_MAX 256
#define _GAME_CSAFILE_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "GAME_CSACONNECTION_DEBUG     : " fmt, ## args); \
        }
#define _GAME_CSAFILE_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_GAME_CSAFILE_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _GAME_CSAFILE_CHECK(x)
#define _GAME_CSAFILE_DEBUG_OUT(fmt, args...)
#define _GAME_CSAFILE_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* -------------------------- CSAFile  exceptions -------------------------- */
class CSAFileException                                {};
class CSAFileIOException    : public CSAFileException {};
/* ------------------------------------------------------------------------- */



/* ----------------------------- CSAFile class ----------------------------- */

class CSAFile {

public:

    /// Constructor with a CSA file
    CSAFile (const std::string &);

    /// Read the CSA file
    const CSASummary &          readGame  (const std::string &);

    /// CSA game summary
    const CSASummary &          summary   (void);

    /// Sequence of the moves
    foundation::
    List<CSAMove>::iterator begin         (void);
    foundation::
    List<CSAMove>::iterator end           (void);

private:

    /// Void copy constructor
    CSAFile (const CSAFile &);

    /// Void copy operation
    CSAFile &                   operator= (const CSAFile &);

    CSASummary                  _summary;

};

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

#endif
