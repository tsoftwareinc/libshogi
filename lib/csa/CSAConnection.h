/**
 *****************************************************************************

 @file       CSAConnection.h

 @brief      CSA protocol definitions
  
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

#ifndef _GAME_CSACONNECTION_H
#define _GAME_CSACONNECTION_H

#include <Common.h>
#include <string>

#include <stdint.h>
#include <netinet/ip.h>
#include <semaphore.h>

#include <Vector.h>
#include <List.h>
#include <CSASummary.h>


// begin namespace 'game'
namespace game {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _GAME_CSACONNECTION_DEBUG
#define _GAME_CSACONNECTION_CHECK(x) { assert ( (x) ); }
#define _GAME_CSACONNECTION_DEBUG_ERROR_STRING_MAX 256
#define _GAME_CSACONNECTION_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "GAME_CSACONNECTION_DEBUG     : " fmt, ## args); \
        }
#define _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_GAME_CSACONNECTION_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _GAME_CSACONNECTION_CHECK(x)
#define _GAME_CSACONNECTION_DEBUG_OUT(fmt, args...)
#define _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ----------------------- CSAConnection  exceptions ----------------------- */
class CSAConnectionException                                      {};
class CSAConnectionIOException    : public CSAConnectionException {};
class CSAConnectionIllegalRequest : public CSAConnectionException {};
/* ------------------------------------------------------------------------- */



/* -------------------------- CSAConnection class -------------------------- */

class CSAConnection {

public:

    /// Default port number for CSA server
    static const short  PortNumber        = 4081;

    /// Keep alive interval
    static const int    KeepAliveInterval =   60;
    static const int    KeepAliveRetry    =   10;

    /// Socket address
    union address {
        struct sockaddr     sa;
        struct sockaddr_in  sa_in;
        struct sockaddr_in6 sa_in6;
    };

    /// Constructor
    CSAConnection ();

    /// Construct with server address
    CSAConnection (std::string);

    /// Construct with server address and port number
    CSAConnection (std::string, unsigned short);

    /// Destructor
    virtual ~CSAConnection();

    /// States
    enum State {
        NOT_CONNECTED = 0x00000001,
        CONNECTED     = 0x00000002,
        LOGGEDIN      = 0x00000004,
        PROPOSED      = 0x00000008,
        ONGOING       = 0x00000010
    };

    /// Connect
    void                connect (void);

    /// Login to the server  
    void                login   (std::string, std::string);

    /// Logout from the server  
    void                logout  (void);

    /// Wait for a new game proposal
    const CSASummary &  newGame (void);

    /// Accept the proposal
    bool                accept  (void);

    /// Reject the proposal
    void                reject  (void);

    /// Close the connection
    void                close   (void);

    /// Check if there is a message to read
    bool                message (void);

    /// Receive message
    void                receive (foundation::Vector<std::string> &);

    /// Send a message
    size_t              send    (const std::string &);

    /// Resign the game
    void                resign  (void);

    /// Write kifu file
    void                write   (const std::string &);


    /// Get current stete
    enum State          getState      (void) const;

    /// Get server address
    std::string         getAddress    (void) const;

    /// Set server address
    void                setAddress    (const std::string &);

    /// Get port number
    unsigned int        getPortNumber (void) const;

    /// Set port number
    void                setPortNumber (unsigned int);

private:

    /// Void copy constructor
    CSAConnection (const CSAConnection &);

    /// Void copy operation
    CSAConnection &     operator= (const CSAConnection &);

    /// Socket descriptor
    int                 _sfd;

    /// Server address
    struct in_addr      _ipaddr;

    /// Server IP address in string
    std::string         _ipaddrInString;

    /// Server host name
    std::string         _serverName;

    /// Server port number
    uint16_t            _port;

    /// State
    enum State          _state;

    /// Login name
    std::string         _username;

    /// Login password
    std::string         _password;

    /// Current game
    CSASummary          _summary;

    /// Temporary buffer
    std::string         _tempbuff;

    /// Check state
    void                _checkCurrentState  (int, const std::string &);
    void                _checkCurrentStateN (int, const std::string &);

};

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

#endif
