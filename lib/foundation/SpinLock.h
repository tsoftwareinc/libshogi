/**
 *****************************************************************************

 @file       SpinLock.h

 @brief      Spin lock implementation
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-14

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
  14th Mar 2014  0.1    Initial release
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_SPINLOCK_H
#define _FOUNDATION_SPINLOCK_H

#include <pthread.h>

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_SPINLOCK_DEBUG
#define _FOUNDATION_SPINLOCK_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_SPINLOCK_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_SPINLOCK_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_SPINLOCK_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_SPINLOCK_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_SPINLOCK_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_SPINLOCK_CHECK(x)
#define _FOUNDATION_SPINLOCK_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_SPINLOCK_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
class SpinLockObject;
class SpinLock;
/* ------------------------------------------------------------------------- */



/* ------------------------- spinlock object class ------------------------- */

class SpinLockObject {

    friend class SpinLock;

public:

    /// constructor
    SpinLockObject  ()
    {
        pthread_spin_init(&_lock, 0);
    }

    /// destructor
    virtual ~SpinLockObject ()
    {
        pthread_spin_destroy(&_lock);
    }

private:

    /// void copy constructor
    SpinLockObject (const SpinLockObject &);

    /// void copy operator
    SpinLockObject &        operator= (const SpinLockObject &);

    /// lock variable
    pthread_spinlock_t      _lock;

};

/* ------------------------------------------------------------------------- */



/* ---------------------------- spinLock  class ---------------------------- */

class SpinLock
{

public:

    /// constructor
    SpinLock (SpinLockObject &v)
    {
        _var = &(v._lock);
        pthread_spin_lock(_var);
    }

    /// constructor
    SpinLock (SpinLockObject *v)
    {
        _var = &(v->_lock);
        pthread_spin_lock(_var);
    }

    /// destructor 
    ~SpinLock ()
    {
        pthread_spin_unlock(_var);
    }

private:

    /// void default constructor
    SpinLock  ();

    /// void copy constructor
    SpinLock  (const SpinLock &);

    /// void copy operator
    SpinLock &              operator= (const SpinLock &);

    /// lock variable
    pthread_spinlock_t *    _var;

};

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
