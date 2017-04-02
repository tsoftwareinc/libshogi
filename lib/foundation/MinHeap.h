/*****************************************************************************

 @file       MinHeap.h

 @brief      Min heap implementation

 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2017-03-31

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
  31st Mar 2017 0.1    Initial release.
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_MINHEAP_H
#define _FOUNDATION_MINHEAP_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_MINHEAP_DEBUG
#define _FOUNDATION_MINHEAP_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_MINHEAP_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_MINHEAP_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_MINHEAP_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_MINHEAP_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_MINHEAP_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_MINHEAP_CHECK(x)
#define _FOUNDATION_MINHEAP_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_MINHEAP_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- min heap  exception --------------------------- */
class MinHeapException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class MinHeap;
/* ------------------------------------------------------------------------- */



/* ---------------------------- min heap class ------------------------------ */

template <typename T>
class MinHeap

{

public:

    /// Constructor with size 
    MinHeap (size_t);

    /// Constructor with size and initial value
    MinHeap (size_t, T);

    /// Copy constructor
    MinHeap (const MinHeap<T> &);

    /// Destructor
    ~MinHeap ();

    /// Add entry
    void        add    (const T &);

    /// Delete entry
    T           del    (void);

    /// Check size
    size_t      size   (void) { return _reserve; }

    /// Check number of elements
    off_t       vsize  (void) { return _values;  }

    /// Get value
    T           value  (size_t) const;

    /// Assign
    MinHeap<T> & operator= (const MinHeap<T> &);

    /// Check if same
    bool        operator== (const MinHeap<T> &);

    /// Array access
    T &         operator[] (size_t);

    /// iterator
    class       iterator;
    iterator    begin  (void);
    iterator    end    (void);


protected:

    size_t      _reserve;
    off_t       _values;
    T *         _ptr;


private:

    /// Void default constructor
    MinHeap ();

    void        swap (T &, T &);

};


template <typename T>
void MinHeap<T>::swap (T &l, T &r)
{

    T t = l;
      l = r;
      r = t;
}


template <typename T>
MinHeap<T>::MinHeap (size_t size)
    : _reserve(size), _values(-1)
{

    _FOUNDATION_MINHEAP_CHECK(size);

    _ptr = new T[size];

}


template <typename T>
MinHeap<T>::MinHeap (size_t size, T data)
    : _reserve(size), _values(-1)
{

    _FOUNDATION_MINHEAP_CHECK(size);

    _ptr    = new T[size];
    _ptr[0] = data;

}


template <typename T>
MinHeap<T>::MinHeap (const MinHeap<T> &data)
    : _reserve(data._reserve), _values(data._values)
{

    _ptr = new T[data._reserve];

    operator=(data);

}


template <typename T>
MinHeap<T>::~MinHeap ()
{

    delete [] _ptr;

}


template <typename T>
void MinHeap<T>::add (const T &data)
{

    _FOUNDATION_MINHEAP_CHECK(_values < (off_t)_reserve);

    ++_values;
    _ptr[_values] = data;

    for (size_t i = _values; i > 0; ) {
        size_t prt = (i - 1) >> 1;
        if (_ptr[prt] > _ptr[i]) {
            swap(_ptr[prt], _ptr[i]);
        }
        i = prt;
    }

}


template <typename T>
T MinHeap<T>::del (void)
{

    T rtn   = _ptr[0];
    _ptr[0] = _ptr[_values]; 
    --_values;

    for (off_t i = 0; i < _values / 2; ) {
        off_t chd = i << 1;
        if (_ptr[i] > _ptr[chd + 1]) {
            if (_ptr[chd + 2] > _ptr[chd + 1]) {
                swap(_ptr[i], _ptr[chd + 1]);
                i = chd + 1;
            } else {
                swap(_ptr[i], _ptr[chd + 2]);
                i = chd + 2;
            }
        } else
        if (_ptr[i] > _ptr[chd + 2]) {
                swap(_ptr[i], _ptr[chd + 2]);
                i = chd + 2;
        } else {
            break;
        }
    }

    return rtn;

}


template <typename T>
T MinHeap<T>::value (size_t index) const
{

    _FOUNDATION_MINHEAP_CHECK(_reserve > index);

    return _ptr[index];

}


template <typename T>
MinHeap<T> & MinHeap<T>::operator= (const MinHeap<T> &rhs)
{

    // check if caller attempt to assign rhs to itself
    if (_ptr == rhs._ptr) {
        return *this;
    }

    _values = _reserve < rhs._values ? _reserve : rhs.values;

    for (size_t i = 0; i < _values; ++i) {
        _ptr[i] = rhs._ptr[i];
    }

    return *this;

}


template <typename T>
T & MinHeap<T>::operator[] (size_t index)
{

    _FOUNDATION_MINHEAP_CHECK(_reserve > index);

    return _ptr[index];

}


template <typename T>
bool MinHeap<T>::operator== (const MinHeap<T> &rhs)
{

    if (_values != rhs._values) {
        return false;
    }

    for (size_t i = 0; i < _values; ++i) {
        if (_ptr[i] != rhs._ptr[i]) {
            return false;
        }
    }

    return true;

}


template <typename T>
class MinHeap<T>::iterator {

public:
    iterator (T *ptr)
        : _cur(ptr) {}

    ~iterator () {}

    void                operator++ ();
    void                operator++ (int);
    void                operator-- ();
    void                operator-- (int);
    bool                operator!= (const iterator &);
    T &                 operator*  ();


private:
    T *                 _cur;

};


template <typename T>
void MinHeap<T>::iterator::operator++ ()
{

    _cur++;

}


template <typename T>
void MinHeap<T>::iterator::operator++ (int)
{

    _cur++;

}


template <typename T>
void MinHeap<T>::iterator::operator-- ()
{

    _cur--;

}


template <typename T>
void MinHeap<T>::iterator::operator-- (int)
{

    _cur--;

}


template <typename T>
bool MinHeap<T>::iterator::operator!= (const typename MinHeap<T>::iterator &rhs)
{

    return (_cur != rhs._cur);

}


template <typename T>
T & MinHeap<T>::iterator::operator* ()
{

    return *_cur;

}


template <typename T>
typename MinHeap<T>::iterator MinHeap<T>::begin (void)
{
    MinHeap<T>::iterator   itr(_ptr);
    return itr;

}


template <typename T>
typename MinHeap<T>::iterator MinHeap<T>::end   (void)
{

    MinHeap<T>::iterator   itr(_ptr + _values);
    return itr;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
