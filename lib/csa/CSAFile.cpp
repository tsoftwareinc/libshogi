/**
 *****************************************************************************

 @file       CSAFile.cpp

 @brief      CSA file reading
  
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
  28th Mar 2014  0.1   Initial release
 
 *****************************************************************************/ 

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <CSAFile.h>

using namespace foundation;

// begin namespace 'game'
namespace game {

/* ------------------------------- parameters ------------------------------ */
static const char           LineEnd = 0x0a;
static const char           CReturn = 0x0d;
/* ------------------------------------------------------------------------- */



/* ---------------------------- function protos ---------------------------- */
static inline void _parseText   (const std::string &, const std::string &, std::string &);
static inline void _parseTextWS (const std::string &, const std::string &, std::string &);
/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * Parse the text in the summary text
 * @param  text  to be parsed
 * @param  token keyword
 * @param  var   to hold the parsed value
 */
static inline void _parseText (const std::string &text,
                               const std::string &token, std::string &var)
try {

    // clear
    var.erase();

    // search token
    auto ptr  = text.find(token);
    if ( ptr == std::string::npos) {
        return;
    }

    // skip token
    ptr += token.size();

    // skip white space
    while (text[ptr] == ' ') {
        ptr++;
    }

    // parse value until line end
    while (text[ptr] != LineEnd) {
        var += text[ptr++];
    }

} catch (std::out_of_range) {
    var.erase();
    return;
}



/**
 * Internal function to parse the value in the summary text including
 * white space.
 * @param  text  to be pased
 * @param  token keyword
 * @param  var   to hold the parsed value
 */
static inline void _parseTextWS (const std::string &text,
                                 const std::string &token, std::string &var)
try {

    // clear
    var.erase();

    // search token
    auto ptr  = text.find(token);
    if ( ptr == std::string::npos) {
        return;
    }

    // skip token
    ptr += token.size();

    // parse value until line end
    while (text[ptr] != LineEnd) {
        var += text[ptr++];
    }

} catch (std::out_of_range) {
    var.erase();
    return;
}



/**
 * Constructor with a CSA file
 * @param filename file path to the file to read
 */
CSAFile::CSAFile (const std::string &filename)
{

    readGame(filename);

}



/**
 * Read CSA kifu file
 * @param path to the file to read
 * @retrun CSA game summary
 */
const CSASummary & CSAFile::readGame (const std::string &filename)
try {

    // open and read the file
    std::string   smrystr;
    std::ifstream ifs;
    try {
        ifs.open(filename.c_str());
        if (! ifs) {
            // failed to open the file
            throw CSAFileIOException();
        }
        std::string l;
        while (! (std::getline(ifs, l)).eof()) {
            // erase comment
            auto ptr = l.find('\'');
            if (ptr != std::string::npos) { 
                l.erase(ptr);
            }
            if (l.size() == 0) {
                continue;
            }
            // replace line end
            ptr = l.size() - 1;
            if (l[ptr] == CReturn) {
                l[ptr]  = LineEnd;
            }
            if (l[ptr] != LineEnd) {
                l += LineEnd;
            }
            // take this string
            smrystr += l;
        }
    } catch (std::exception e) {
        throw CSAFileIOException();
    }

    // the fields with default value
#define _CSAFILE_CPP_GAMESUM_DEFVAL(field_string,token_string,default_string) { \
    _parseText(smrystr, (token_string), _summary.field_string); \
    if (_summary.field_string.empty()) \
        _summary.field_string = (default_string); \
    }
    _CSAFILE_CPP_GAMESUM_DEFVAL(version,   "V",  "1");
    _CSAFILE_CPP_GAMESUM_DEFVAL(senteName, "N+", "" );
    _CSAFILE_CPP_GAMESUM_DEFVAL(goteName,  "N-", "" );

    // position
    // we assume the position was given when the file contain "P1-" at line head
    auto ptr = smrystr.find("P1");
    auto plt = ptr == 0 ? LineEnd : smrystr[ptr - 1];
    if (ptr != std::string::npos && plt <= 0x20) {

#define _CSAFILE_CPP_GAMESUM_REQVALWS(field_string,token_string) { \
        _parseTextWS(smrystr, (token_string), _summary.field_string); \
        if (_summary.field_string.empty()) { \
            _GAME_CSAFILE_DEBUG_OUT(token_string " is not contained.\n"); \
            throw CSAFileException(); \
        } \
        }
        smrystr.erase(0,  smrystr.find("P1-"));
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[0], "P1");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[1], "P2");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[2], "P3");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[3], "P4");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[4], "P5");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[5], "P6");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[6], "P7");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[7], "P8");
        _CSAFILE_CPP_GAMESUM_REQVALWS(position[8], "P9");
        _CSAFILE_CPP_GAMESUM_DEFVAL(senteHand  , "P+", "");
        _CSAFILE_CPP_GAMESUM_DEFVAL(goteHand   , "P-", "");

    } else {
        _summary.position[0] = "-KY-KE-GI-KI-OU-KI-GI-KE-KY";
        _summary.position[1] = " * -HI *  *  *  *  * -KA * ";
        _summary.position[2] = "-FU-FU-FU-FU-FU-FU-FU-FU-FU";
        _summary.position[3] = " *  *  *  *  *  *  *  *  * ";
        _summary.position[4] = " *  *  *  *  *  *  *  *  * ";
        _summary.position[5] = " *  *  *  *  *  *  *  *  * ";
        _summary.position[6] = "+FU+FU+FU+FU+FU+FU+FU+FU+FU";
        _summary.position[7] = " * +KA *  *  *  *  * +HI * ";
        _summary.position[8] = "+KY+KE+GI+KI+OU+KI+GI+KE+KY";
        _summary.senteHand   = "";
        _summary.goteHand    = "";
    }

    // check the player to move first
    std::string l;
    std::istringstream is(smrystr);
    while(1) {
        std::getline(is, l, LineEnd);
        auto fply = l.substr(0, 1);
        if (fply == "+" || fply == "-") {
            _summary.firstPlayer = fply;
            break;
        }
    }
    if (_summary.firstPlayer.empty()) {
        throw CSAFileException();
    }

    // store sequence
    while (std::getline(is, l, LineEnd)) {
        if (l[0] != '+' && l[0] != '-') {
            continue;
        }
        _summary.sequence.add(l);
    }

    return _summary;

} catch (CSAFileException e) {
    throw e;
}



/**
 * CSA game summary
 * @return CSA game summary
 */
const CSASummary & CSAFile::summary (void)
{

    return _summary;

}



/**
 * Sequece of the moves
 * @return iterator begin
 */
List<CSAMove>::iterator CSAFile::begin (void)
{

    return _summary.sequence.begin();

}



/**
 * Sequece of the moves
 * @return iterator end
 */
List<CSAMove>::iterator CSAFile::end (void)
{

    return _summary.sequence.end();

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}
