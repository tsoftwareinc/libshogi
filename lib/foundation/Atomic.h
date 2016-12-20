/**
 *****************************************************************************

 @file       Atomic.h

 @brief      Variable and atomic operations
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-19

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
  19th Mar 2014  0.1    Initial release
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_ATOMIC_H
#define _FOUNDATION_ATOMIC_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_ATOMIC_DEBUG
#define _FOUNDATION_ATOMIC_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_ATOMIC_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_ATOMIC_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_ATOMIC_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_ATOMIC_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_ATOMIC_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_ATOMIC_CHECK(x)
#define _FOUNDATION_ATOMIC_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_ATOMIC_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class Atomic;
/* ------------------------------------------------------------------------- */



/* ------------------------- atomic variable class ------------------------- */

template <typename T>
class Atomic {

/// tightly leans on GCC builtin compiler extentions, this doesn't
/// work on certain processors. available types are the followings;
/// int, long, long long and those unsigned counterparts.

public:

    /// constructor
    Atomic<T> () {}

    /// constructor
    Atomic<T> (T v) : _v(v) {}

    /// read
    void                    get        (T &);

    /// assign
    void                    set        (const T &);

    /// atomic compare and swap
    bool                    swap       (const T &, const T &);

    /// prepositive increment
    T                       operator++ ();

    /// postpositive increment
    T                       operator++ (int);

    /// prepositive decrement
    T                       operator-- ();

    /// postpositive decrement
    T                       operator-- (int);

    /// check equality
    bool                    operator== (const T &);

    /// check greater
    bool                    operator>  (const T &);

    /// check less
    bool                    operator<  (const T &);

    /// check greater than equal
    bool                    operator>= (const T &);

    /// check less than equal
    bool                    operator<= (const T &);

private:

    /// void copy constructor
    Atomic<T> (const Atomic<T> &);

    /// void copy
    Atomic<T> &             operator= (const Atomic<T> &);

    /// atomic variable
    volatile T __attribute__((aligned(64)))  _v;

};



/**
 * Get value
 * @param  v variable holding the value
 */
template <typename T>
inline void Atomic<T>::get (T &v)
{

    v = _v;
    __sync_synchronize();

}



/**
 * Set value
 * @param  v variable holding the value
 */
template <typename T>
inline void Atomic<T>::set (const T &v)
{

    __sync_synchronize();
    _v = v;

}



/**
 * Compare the current value with the given old value and swap it to
 * the new value if the current value matches with the old value.
 * @param oldval a value to be tested  (must be private within a thread)
 * @param newval a value to be swapped (must be private within a thread)
 * @return result to check if the current value matched and swapped.
 */
template <typename T>
inline bool Atomic<T>::swap (const T &oldval, const T &newval)
{

    return __sync_bool_compare_and_swap(&_v, oldval, newval);

}



template <typename T>
inline T Atomic<T>::operator++ ()
{

    return __sync_add_and_fetch(&_v, 1);

}



template <typename T>
inline T Atomic<T>::operator++ (int)
{

    return __sync_fetch_and_add(&_v, 1);

}



template <typename T>
inline T Atomic<T>::operator-- ()
{

    return __sync_sub_and_fetch(&_v, 1);

}



template <typename T>
inline T Atomic<T>::operator-- (int)
{

    return __sync_fetch_and_sub(&_v, 1);

}



template <typename T>
inline bool Atomic<T>::operator== (const T &rhs)
{

    __sync_synchronize();
    return (_v == rhs);

}



template <typename T>
inline bool Atomic<T>::operator< (const T &rhs)
{

    __sync_synchronize();
    return (_v < rhs);

}


template <typename T>
inline bool Atomic<T>::operator> (const T &rhs)
{

    __sync_synchronize();
    return (_v > rhs);

}


template <typename T>
inline bool Atomic<T>::operator<= (const T &rhs)
{

    __sync_synchronize();
    return (_v <= rhs);

}


template <typename T>
inline bool Atomic<T>::operator>= (const T &rhs)
{

    __sync_synchronize();
    return (_v >= rhs);

}


/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
