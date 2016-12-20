/**
 *****************************************************************************

 @file       Semaphore.h

 @brief      Semaphore implementation
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-24

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
  24th Mar 2014  0.1    Initial release
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_SEMAPHORE_H
#define _FOUNDATION_SEMAPHORE_H

#include <semaphore.h>

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_SEMAPHORE_DEBUG
#define _FOUNDATION_SEMAPHORE_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_SEMAPHORE_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_SEMAPHORE_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_SEMAPHORE_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_SEMAPHORE_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_SEMAPHORE_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_SEMAPHORE_CHECK(x)
#define _FOUNDATION_SEMAPHORE_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_SEMAPHORE_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
class Semaphore;
/* ------------------------------------------------------------------------- */



/* ---------------------------- semaphore class ---------------------------- */

class Semaphore {

public:

    /// constructor
    Semaphore  ()
    {
        sem_init(&_sem, 0, 0);
    }

    /// constructor
    Semaphore  (unsigned int val)
    {
        sem_init(&_sem, 0, val);
    }

    /// destructor
    virtual ~Semaphore ()
    {
        sem_destroy(&_sem);
    }

    /// wait
    void                    wait (void)
    {
        sem_wait(&_sem);
    }

    /// trywait
    bool                    check (void)
    {
        while (1) {
            if (sem_trywait(&_sem) == 0) {
                return true;
            }
            if (errno == EAGAIN)    {
                return false;
            }
        }
    }

    /// post
    void                    post (void)
    {
        sem_post(&_sem);
    }

private:

    /// void copy constructor
    Semaphore (const Semaphore &);

    /// void copy operator
    Semaphore &             operator= (const Semaphore &);

    /// semaphore
    sem_t      _sem;

};

/* --------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
