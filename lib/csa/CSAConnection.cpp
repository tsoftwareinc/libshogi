/**
 *****************************************************************************

 @file       CSAConnection.cpp

 @brief      CSAConnection implementations
  
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
  28th Mar 2014  0.1   Initial release
 
 *****************************************************************************/ 

#include <cstring>
#include <cerrno>
#include <ctime>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <netinet/tcp.h>

#include <stdexcept>
#include <fstream>
#include <sstream>

#include <CSAConnection.h>


using namespace foundation;


// begin namespace 'game'
namespace game {



/* ------------------------------- parameters ------------------------------ */
static const char    LineEnd      = 0x0a;
static const size_t  SizeOfBuffer = 0x10000;
/* ------------------------------------------------------------------------- */



/* ---------------------------- function protos ---------------------------- */
static inline int    _openSocket  (int);
static inline void   _connect     (int, struct sockaddr_in &);
static inline size_t _send        (const std::string &, int);
static inline size_t _receive     (std::string &, int);
static inline void   _close       (int);
static inline int    _parseValue  (const std::string &, const std::string &);
static inline void   _parseText   (const std::string &, const std::string &, std::string &);
static inline void   _parseTextWS (const std::string &, const std::string &, std::string &);
/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * Internal function to open a socket
 * @param type socket type (SOCK_DGRAM : UDP / SOCK_STREAM : TCP)
 * @return socket file descriptor.
 */
static inline int _openSocket (int type = SOCK_STREAM)
{

    int                     sfd;

    // open a socket
    sfd = socket(AF_INET, type, 0);

    if (sfd == -1) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("socket()");
        throw CSAConnectionIOException();
    }

    // keepalive 
    int optval = 1;
    int optlen = sizeof(optval);
    if(setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("setsockopt()");
        throw CSAConnectionIOException();
    }
    optval = CSAConnection::KeepAliveInterval;
    if(setsockopt(sfd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen) < 0) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("setsockopt()");
        throw CSAConnectionIOException();
    }
    optval = CSAConnection::KeepAliveRetry;
    if(setsockopt(sfd, SOL_TCP, TCP_KEEPINTVL, &optval, optlen) < 0) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("setsockopt()");
        throw CSAConnectionIOException();
    }
 
    return sfd;

}



/**
 * Internal function to connect to the server
 * @param sfd   socket file descriptor
 * @param saddr address of the server
 */
static inline void _connect (int sfd, struct sockaddr_in &saddr)
{

    int                     rtn;

    // check if the socket is valid
    if (sfd == -1) {
        _GAME_CSACONNECTION_DEBUG_OUT("Invalid socket : _send().\n");
        throw CSAConnectionIllegalRequest();
    }

    // receive
    rtn = connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (rtn == -1) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("connect()");
        throw CSAConnectionIOException();
    }

}



/**
 * Internal function to send a message
 * @param var to hold the message to send
 * @param sfd socket file descriptor
 * @return number of bytes sent
 */
static inline size_t _send (const std::string &var, int sfd)
{

    ssize_t                 rtn;

    // check if the socket is valid
    if (sfd == -1) {
        _GAME_CSACONNECTION_DEBUG_OUT("Invalid socket : _send().\n");
        throw CSAConnectionIllegalRequest();
    }

    // receive
    rtn = send(sfd, var.c_str(), var.size(), MSG_NOSIGNAL);
    if (rtn == -1) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("send()");
        throw CSAConnectionIOException();
    }

    return static_cast<size_t>(rtn);

}



/**
 * Internal function to receive a message
 * @param var to hold the received message
 * @param sfd socket file descriptor
 * @return number of bytes received
 */
static inline size_t _receive (std::string &var, int sfd)
{

    ssize_t                 rtn;
    char                    buffer[SizeOfBuffer];

    // check if the socket is valid
    if (sfd == -1) {
        _GAME_CSACONNECTION_DEBUG_OUT("Invalid socket : _receive()\n");
        throw CSAConnectionIllegalRequest();
    }

    // receive
    rtn = recv(sfd, buffer, SizeOfBuffer, MSG_NOSIGNAL);
    if (rtn == -1) {
        // IO error
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("recv()");
        throw CSAConnectionIOException();
    }

    // check size
    if (rtn == static_cast<ssize_t>(SizeOfBuffer)) {
        _GAME_CSACONNECTION_DEBUG_OUT("The buffer size may not be sufficient.\n");
        throw CSAConnectionException();
    }

    // copy as a string
    buffer[rtn] = '\0';
    var         = buffer;

    return static_cast<size_t>(rtn);

}



/**
 * Internal function to close the connection
 * @param sfd socket file descriptor
 */
static inline void _close (int sfd)
{

    // check if the socket is valid
    if (sfd == -1) {
        _GAME_CSACONNECTION_DEBUG_OUT("Invalid socket : _send().\n");
        throw CSAConnectionIllegalRequest();
    }

    // close
    close(sfd);

}



/**
 * Internal function to parse the numeric value in the summary text
 * @param text  to be parsed
 * @param token keyword
 * @return to hold the parsed value. (-1) if not succeeded.
 */
static inline int _parseValue (const std::string &text, const std::string &token)
try {

    std::string             var;

    // search token
    auto ptr  = text.find(token);
    if ( ptr == std::string::npos) {
        return -1;
    }

    // skip token
    ptr += token.size();

    // skip white space
    while (text[ptr] == ' ') {
        ptr++;
    }

    // parse value until line end
    while (text[ptr] >= '0' && text[ptr] <= '9') {
        var += text[ptr++];
    }

    return std::stoi(var);

} catch (std::out_of_range) {
    return -1;
}



/**
 * Internal function to parse the value in the summary text
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
 * @param  text  to be parsed
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
 * Defalult constructor
 */
CSAConnection::CSAConnection ()
    : _sfd(-1), _port (PortNumber), _state (NOT_CONNECTED)
{

    return;

}



/**
 * Constructor to set address
 * @param  ipaddr address in string
 */
CSAConnection::CSAConnection (std::string ipaddr)
    : _sfd(-1), _port (PortNumber), _state (NOT_CONNECTED)
{

    setAddress(ipaddr);

}



/**
 * Constructor to set address and port number
 * @param  ipaddr address in string
 * @param  port   port number
 */
CSAConnection::CSAConnection (std::string ipaddr, unsigned short port)
    : _sfd(-1), _port(port) , _state (NOT_CONNECTED)
{

    setAddress(ipaddr);

}



/**
 * Destructor
 */
CSAConnection::~CSAConnection ()
{

    // close connection
    if (_state == CONNECTED) {

        _close(_sfd);

    }

}



/**
 * Connect to the server
 */
void CSAConnection::connect (void)
{

    address                 sa;
    struct sockaddr_in &    saddr = sa.sa_in;

    // check the state
    try {
        _checkCurrentState(NOT_CONNECTED, "NOT_CONNECTED");
    } catch (CSAConnectionException e) {
        throw e;
    }

    // open socket
    try {
        _sfd = _openSocket(SOCK_STREAM);
    }
    catch (CSAConnectionException e) {
        throw e;
    }

    // set address
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port   = htons(_port);
    saddr.sin_addr   = _ipaddr;

    // connect
    _connect(_sfd, saddr);

    // update state
    _state = CONNECTED;

}



/**
 * Login to the server
 * @param user user name
 * @param pass password
 */
void CSAConnection::login (std::string user, std::string pass)
try {

    // check current state
    _checkCurrentState(CONNECTED, "CONNECTED");

    // send login
    std::string loginstr  = "LOGIN " + user + " " + pass;
    loginstr             += LineEnd;
    _send(loginstr, _sfd);

    // wait for reply
    // some server sends back login acknowledge and game summary
    // at once. therefore, once receive those in temporary buffer
    // and let newGame() take over remainder
    _receive(_tempbuff, _sfd);
    // cut one line out
    auto lend = _tempbuff.find(LineEnd);
    if (lend == std::string::npos) {
        _tempbuff = "Login failed : " + _tempbuff;
        _GAME_CSACONNECTION_DEBUG_OUT("%s\n", _tempbuff.c_str());
        throw CSAConnectionException();
    }
    // reply to LOGIN
    std::string loginreply = _tempbuff.substr(0, lend);
    _tempbuff.erase(0, lend);
    if (loginreply.find("LOGIN:") == std::string::npos) {
        loginreply = "Login failed : " + loginreply;
        _GAME_CSACONNECTION_DEBUG_OUT("%s\n", loginreply.c_str());
        throw CSAConnectionException();
    }

    // logged in
    loginreply = "Login succeeded : " + loginreply;
    _GAME_CSACONNECTION_DEBUG_OUT("%s\n", loginreply.c_str());

    // copy user name and password
    _username = user;
    _password = pass;

    // change status
    _state = LOGGEDIN;

} catch (CSAConnectionException e) {
    throw e;
}



/**
 * Logout from the server
 */
void CSAConnection::logout (void)
{

    // the protocol design is not mature (Ref. 1.1.3)
    // suppose we can logout anytime
    try {
        _checkCurrentState(LOGGEDIN  | PROPOSED  | ONGOING,
                          "LOGGEDIN OR PROPOSED OR ONGOING");
    } catch (CSAConnectionIllegalRequest e) {
        throw e;
    }

    // send logout command and wait for the acknowledgement
    std::string logoutstr("LOGOUT");
    std::string logoutreply;
    logoutstr += LineEnd;
    try {
        _send(logoutstr, _sfd);
    } catch (CSAConnectionIOException) {
        _GAME_CSACONNECTION_DEBUG_OUT("Connection may be reset by peer.\n");
        _state = NOT_CONNECTED;
        return;
    }

    // check reply from the server (connection was kept)
    if (logoutreply.find("LOGOUT:") == std::string::npos) {
        logoutreply = "Logout failed : " + logoutreply;
        _GAME_CSACONNECTION_DEBUG_OUT("%s\n", logoutreply.c_str());
    } else {
        logoutreply = "Logout succeeded : " + logoutreply;
        _GAME_CSACONNECTION_DEBUG_OUT("%s\n", logoutreply.c_str());
    }

    _state = CONNECTED;

}



/**
 * Wait for a proposal of new game. Multiple game offerings are not
 * not supported yet.
 * @return the received game summary
 */
const CSASummary & CSAConnection::newGame (void)
try {

    enum {
        BEGIN_WAITING,
        CLOSE_WAITING,
        PARSE_BASEVAL,
        PARSE_TIMEBLK,
        PARSE_KYOKBLK
    } state;

    // check current state
    _checkCurrentState(LOGGEDIN | PROPOSED, "LOGGEDIN OR PROPOSED");

    std::string message;
    std::string smrystr;

    state = BEGIN_WAITING;

    while (1) {

        if (_tempbuff.size() == 0) {
            _receive(message, _sfd);
            smrystr += message;
        } else {
            smrystr  = _tempbuff;
            _tempbuff.resize(0);
        }

        switch (state) {

        case BEGIN_WAITING:
            if (smrystr.find("BEGIN Game_Summary") == std::string::npos) {
                continue;
            }
            state = CLOSE_WAITING;

        case CLOSE_WAITING:
            if (smrystr.find("END Game_Summary")   == std::string::npos) {
                continue;
            }
            state = PARSE_BASEVAL;

        case PARSE_BASEVAL:
#define _CSACONNECTION_CPP_GAMESUM_REQVAL(field_string,token_string) { \
            _parseText(smrystr, (token_string), _summary.field_string); \
            if (_summary.field_string.empty()) { \
                _GAME_CSACONNECTION_DEBUG_OUT(token_string " is not contained.\n"); \
                smrystr.erase(); \
                state = BEGIN_WAITING; \
                continue; \
            } \
        }
#define _CSACONNECTION_CPP_GAMESUM_DEFVAL(field_string,token_string,default_string) { \
            _parseText(smrystr, (token_string), _summary.field_string); \
            if (_summary.field_string.empty()) { \
                _summary.field_string = (default_string); \
            } \
        }
            // the required files of value must be presented.
            _CSACONNECTION_CPP_GAMESUM_REQVAL(version,     "Protocol_Version:");
            _CSACONNECTION_CPP_GAMESUM_REQVAL(format,      "Format:"          );
            _CSACONNECTION_CPP_GAMESUM_REQVAL(senteName,   "Name+:"           );
            _CSACONNECTION_CPP_GAMESUM_REQVAL(goteName,    "Name-:"           );
            _CSACONNECTION_CPP_GAMESUM_REQVAL(yourTurn,    "Your_Turn:"       );
            _CSACONNECTION_CPP_GAMESUM_REQVAL(toMove,      "To_Move:"         );

            // the fields with default value
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(mode,        "Protocol_Mode:",   "Server");
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(declaration, "Declaration:",     ""      );
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(gameID,      "Game_ID:",         ""      );
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(rematch,     "Rematch_On_Draw:", "NO"    );

#define _CSACONNECTION_CPP_GAMESUM_NUMVAL(field_string,token_string,default_value) { \
            _summary.field_string = _parseValue(smrystr, (token_string)); \
            if (_summary.field_string == -1) { \
                _summary.field_string = (default_value); \
            } \
        }
            // the numeric fields
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(maxMoves,    "Max_Moves:",        256    );
            state = PARSE_TIMEBLK;

        case PARSE_TIMEBLK:
#define _CSACONNECTION_CPP_GAMESUM_BLKCHK(block_string) { \
            if (smrystr.find((block_string)) == std::string::npos) { \
                _GAME_CSACONNECTION_DEBUG_OUT(block_string " is not contained.\n"); \
                smrystr.erase(); \
                state = BEGIN_WAITING; \
                continue; \
            } \
        }
            _CSACONNECTION_CPP_GAMESUM_BLKCHK("BEGIN Time");
            _CSACONNECTION_CPP_GAMESUM_BLKCHK("END Time"  );
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(unitString, "Time_Unit:", "1sec");
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(unit,       "Time_Unit:", 1     );
            if (_summary.unitString.find("msec") != std::string::npos) {
                _summary.timeUnit = CSASummary::MSECND;
            } else
            if (_summary.unitString.find("min")  != std::string::npos) {
                _summary.timeUnit = CSASummary::MINUIT;
            } else { 
                _summary.timeUnit = CSASummary::SECOND;
            }
            //
            //
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(timeRoundup, "Time_Roundup:",        "NO");
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(leastTime,   "Least_Time_Per_Move:",  0  );
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(totalTime,   "Total_Time:",          -1  );
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(byoyomi,     "Byoyomi:",             -1  );
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(delay,       "Delay:",                0  );
            _CSACONNECTION_CPP_GAMESUM_NUMVAL(increment,   "Increment:",            0  );
            state = PARSE_KYOKBLK;

        case PARSE_KYOKBLK:
            _CSACONNECTION_CPP_GAMESUM_BLKCHK("BEGIN Position");
            _CSACONNECTION_CPP_GAMESUM_BLKCHK("END Position"  );
            // trim the summary string to make the position block top.
            // this is to make sure the search hits 'P[0-9]' of kyokumen text
            smrystr.erase(0,  smrystr.find("BEGIN Position\n")
                             + std::string("BEGIN Position\n").size());
#define _CSACONNECTION_CPP_GAMESUM_REQVALWS(field_string,token_string) { \
            _parseTextWS(smrystr, (token_string), _summary.field_string); \
            if (_summary.field_string.empty()) { \
                _GAME_CSACONNECTION_DEBUG_OUT(token_string " is not contained.\n"); \
                smrystr.erase(); \
                state = BEGIN_WAITING; \
                continue; \
            } \
            }
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[0], "P1");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[1], "P2");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[2], "P3");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[3], "P4");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[4], "P5");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[5], "P6");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[6], "P7");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[7], "P8");
            _CSACONNECTION_CPP_GAMESUM_REQVALWS(position[8], "P9");
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(senteHand  , "P+", "");
            _CSACONNECTION_CPP_GAMESUM_DEFVAL(goteHand   , "P-", "");
            _summary.position.setsz(CSASummary::BoardSize);
            // check the player to move first
            {
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
                    // the first player was not given
                    throw CSAConnectionException();
                }
                // store sequence
                while (std::getline(is, l, LineEnd)) {
                    if (l == "END Position") {
                        break;
                    }
                    _summary.sequence.add(l);
                }
            }
            break;
        }
        break;
    }

    // set state
    _state = PROPOSED;

    return _summary;

} catch (CSAConnectionException e) {
    throw e;
}



/**
 * Accept proposal
 * @return true if agreed to start the game, false otherwise
 */
bool CSAConnection::accept (void)
try {

    // check current state
    _checkCurrentState(PROPOSED, "PROPOSED");

    // send acknowledgement
    std::string ackstr  = "AGREE " + _summary.gameID;
    ackstr             += LineEnd;
    _send(ackstr, _sfd);

    // wait for a kickoff message
    std::string kickoff;
    _receive(kickoff, _sfd);

    if (kickoff.find("START") != std::string::npos) {
        // agreed, set state and start fighting.
        _state = ONGOING;
        return true;
    }

    // get the state back
    _state = LOGGEDIN;

    return false;

} catch (CSAConnectionException e) {
    throw e;
}



/**
 * Reject the proposal
 */
void CSAConnection::reject (void)
try {

    // check current state
    _checkCurrentState(PROPOSED, "PROPOSED");

    // send acknowledgement
    std::string reject  = "REJECT " + _summary.gameID;
                reject += LineEnd;
    _send(reject, _sfd);

    // wait for a acknowledge message
    std::string ackstr;
    _receive(ackstr, _sfd);
    if (ackstr.find("REJECT:") == std::string::npos) {
        ackstr = "Unexpected reply for REJECT : " + ackstr + "\n";
        _GAME_CSACONNECTION_DEBUG_OUT("%s", ackstr.c_str());
    }

    // get the state back
    _state = LOGGEDIN;

} catch (CSAConnectionException e) {
    throw e;
}



/**
 * Close the CSA connection
 */
void CSAConnection::close (void)
{

    // check state
    // you must log out before closing the connection 
    try {
        _checkCurrentState(CONNECTED, "CONNECTED");
    }
    catch (CSAConnectionException e) {
        throw e;
    }

    // close the socket
    _close(_sfd);
    _sfd = -1;

    // change state
    _state = NOT_CONNECTED;

}



/**
 * Check if there is a message to read
 * @return true : there is one message at least. false : none of message.
 */
bool CSAConnection::message (void)
{

    struct pollfd           pfd;
    int                     rtn;

    // check state
    try {
        _checkCurrentState(ONGOING, "ONGOING");
    }
    catch (CSAConnectionException e) {
        throw e;
    }

    // poll
    pfd.fd     = _sfd;
    pfd.events = POLLIN;
    rtn = poll(&pfd, 1, 1);
    if (rtn == -1) {
        _GAME_CSACONNECTION_DEBUG_FUNCTION_RESULT("poll()");
        throw CSAConnectionIOException();
    }

    // check result of poll()
    if (pfd.revents & POLLIN) {
        return true;
    }

    return false;

}



/**
 * Receive a message from the server (stateful)
 * @param  array of string to hold the received message
 */
void CSAConnection::receive (Vector<std::string> &message)
try {

    std::string              var;
    List<std::string>       list;

    // the state must be ONGOING
    _checkCurrentState(ONGOING, "ONGOING");

    // receive message from the server
    auto rtn = _receive(var, _sfd);
    if (rtn == 0) {
        message.setsz(0);
        return;
    }

    // count lines
    std::string str;
    size_t line = 0;
    for (auto c : var) {
        if (c == LineEnd) {
            line++;
            list.add(str);
            str.erase();
        } else {
            str += c;
        }
    }

    // expand array
    message.resize(line);
    message.setsz();

    // set array
    size_t i = 0;
    for (auto m : list) {
        // record move for writing Kifu
        char movemark = m[0];
        if (movemark == '+' || movemark == '-' ||
            movemark == '#' || movemark == '%'    ) {
           _summary.sequence.add(m);
        }

        message[i++] = m;
    }

}
catch (CSAConnectionException e) {
    throw e;
}



/**
 * Send a message to the server (stateful)
 * @param  var to hold the message to send
 * @return number of bytes sent
 */
size_t CSAConnection::send (const std::string &var)
try {

    // the state must be ONGOING
    _checkCurrentState(ONGOING, "ONGOING");

    // send
    return _send(var + LineEnd, _sfd);

}
catch (CSAConnectionException e) {
    throw e;
}



/**
 * Resign the game (stateful)
 * 
 */
void CSAConnection::resign (void)
{

    // the state must be ONGOING
    _checkCurrentState(ONGOING, "ONGOING");

    std::string token("%TORYO");
    token += LineEnd;
    send(token);

}



/**
 * Write kifu
 * @param  filename kifu file name
 * @return number of bytes sent
 */
void CSAConnection::write (const std::string &filename)
try {

    // the state must be LOGGEDIN
    try {
        _checkCurrentState(ONGOING, "ONGOING");
    } catch (CSAConnectionException e) {
        throw e;
    }

    // open the file
    std::ofstream ofs(filename.c_str());

    // write to the file
    ofs << "N+" << _summary.senteName   << LineEnd;
    ofs << "N-" << _summary.goteName    << LineEnd;
    ofs << "P1" << _summary.position[0] << LineEnd;
    ofs << "P2" << _summary.position[1] << LineEnd;
    ofs << "P3" << _summary.position[2] << LineEnd;
    ofs << "P4" << _summary.position[3] << LineEnd;
    ofs << "P5" << _summary.position[4] << LineEnd;
    ofs << "P6" << _summary.position[5] << LineEnd;
    ofs << "P7" << _summary.position[6] << LineEnd;
    ofs << "P8" << _summary.position[7] << LineEnd;
    ofs << "P9" << _summary.position[8] << LineEnd;
    ofs << "P+" << _summary.senteHand   << LineEnd;
    ofs << "P-" << _summary.goteHand    << LineEnd;
    ofs <<         _summary.firstPlayer << LineEnd;

    for (auto move : _summary.sequence) {
        ofs << move << LineEnd;
    }

}
catch (std::exception) {
    throw CSAConnectionIOException();
}



/**
 * Return the current state
 * @return current state
 */
enum CSAConnection::State CSAConnection::getState (void) const
{

    return _state;

}



/**
 * Return the configured server address
 * @return configured server address in string
 */
std::string CSAConnection::getAddress (void) const
{

    return _ipaddrInString;

}



/**
 * Set server address
 * @param var to hold the address in string or host name
 */
void CSAConnection::setAddress (const std::string &ipaddr)
{

    // convert address in string into a numeric address
    int rtn = inet_aton(ipaddr.c_str(), &_ipaddr);
    if (rtn == 0) {
        // try to solve the host name
        struct hostent *host = gethostbyname(ipaddr.c_str());
        if (host == 0) {
            throw CSAConnectionIllegalRequest();
        }
        memcpy(&_ipaddr, host->h_addr, host->h_length);
        // save server host name
        _serverName     = ipaddr;
    }

    // save IP address string
    _ipaddrInString = inet_ntoa(_ipaddr);

}



/**
 * Return the configured server port number
 * @return Configured server port number
 */
unsigned int CSAConnection::getPortNumber (void) const
{

    return _port;

}



/**
 * Set server port number
 * @param  port port number
 */
void CSAConnection::setPortNumber (unsigned int port)
{

    // check if the port is valid
    if (port > 0xffff) {
        throw CSAConnectionIllegalRequest();
    } else {
        _port = static_cast<uint16_t>(port);
    }

}



/**
 * Check if the current state follow the expectation
 * @param  state expected state
 * @param  str expected state in string
 */
void CSAConnection::_checkCurrentState (int state, const std::string &str)
{

    if (! (_state & state)) {
        fprintf(stderr, "The state must be %s at %s - %d.\n",
                             str.c_str(), __FILE__, __LINE__);
        throw CSAConnectionIllegalRequest();
    }

}



/**
 * Check if the current state isn't the designated state
 * @param  state unexpected state
 * @param  str unexpected state in string
 */
void CSAConnection::_checkCurrentStateN (int state, const std::string &str)
{

    if ((_state & state)) {
        fprintf(stderr, "The state must not be %s at %s - %d.\n",
                             str.c_str(), __FILE__, __LINE__);
        throw CSAConnectionIllegalRequest();
    }

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}
