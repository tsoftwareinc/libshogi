/*****************************************************************************

 @file       MaxHeap.h

 @brief      Max heap implementation

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

#ifndef _FOUNDATION_MAXHEAP_H
#define _FOUNDATION_MAXHEAP_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_MAXHEAP_DEBUG
#define _FOUNDATION_MAXHEAP_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_MAXHEAP_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_MAXHEAP_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_MAXHEAP_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_MAXHEAP_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_MAXHEAP_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_MAXHEAP_CHECK(x)
#define _FOUNDATION_MAXHEAP_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_MAXHEAP_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- max heap  exception --------------------------- */
class MaxHeapException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class MaxHeap;
/* ------------------------------------------------------------------------- */



/* ---------------------------- max heap class ------------------------------ */

template <typename T>
class MaxHeap

{

public:

    /// Constructor with size 
    MaxHeap (size_t);

    /// Constructor with size and initial value
    MaxHeap (size_t, T);

    /// Copy constructor
    MaxHeap (const MaxHeap<T> &);

    /// Destructor
    ~MaxHeap ();

    /// Add entry
    void        add    (const T &);

    /// Delete entry
    T           del    (void);

    /// Check size
    size_t      size   (void) { return _reserve; }

    /// Check number of elements
    off_t       vsize  (void) { return (_values + 1); }

    /// Get value
    T           value  (size_t) const;

    /// Assign
    MaxHeap<T> & operator= (const MaxHeap<T> &);

    /// Check if same
    bool        operator== (const MaxHeap<T> &);

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
    MaxHeap ();

    void        swap (T &, T &);

};


template <typename T>
void MaxHeap<T>::swap (T &l, T &r)
{

    T t = l;
      l = r;
      r = t;
}


template <typename T>
MaxHeap<T>::MaxHeap (size_t size)
    : _reserve(size), _values(-1)
{

    _FOUNDATION_MAXHEAP_CHECK(size);

    _ptr = new T[size];

}


template <typename T>
MaxHeap<T>::MaxHeap (size_t size, T data)
    : _reserve(size), _values(-1)
{

    _FOUNDATION_MAXHEAP_CHECK(size);

    _ptr    = new T[size];
    _ptr[0] = data;

}


template <typename T>
MaxHeap<T>::MaxHeap (const MaxHeap<T> &data)
    : _reserve(data._reserve), _values(data._values)
{

    _ptr = new T[data._reserve];

    operator=(data);

}


template <typename T>
MaxHeap<T>::~MaxHeap ()
{

    delete [] _ptr;

}


template <typename T>
void MaxHeap<T>::add (const T &data)
{

    _FOUNDATION_MAXHEAP_CHECK(_values < (off_t)_reserve);

    ++_values;
    _ptr[_values] = data;

    for (size_t i = _values; i > 0; ) {
        size_t prt = (i - 1) >> 1;
        if (_ptr[prt] < _ptr[i]) {
            swap(_ptr[prt], _ptr[i]);
        }
        i = prt;
    }

}


template <typename T>
T MaxHeap<T>::del (void)
{

    T rtn   = _ptr[0];
    _ptr[0] = _ptr[_values]; 
    --_values;

    for (off_t i = 0; i < _values / 2; ) {
        off_t chd = i << 1;
        if (_ptr[i] < _ptr[chd + 1]) {
            if (_ptr[chd + 2] < _ptr[chd + 1]) {
                swap(_ptr[i], _ptr[chd + 1]);
                i = chd + 1;
            } else {
                swap(_ptr[i], _ptr[chd + 2]);
                i = chd + 2;
            }
        } else
        if (_ptr[i] < _ptr[chd + 2]) {
                swap(_ptr[i], _ptr[chd + 2]);
                i = chd + 2;
        } else {
            break;
        }
    }

    return rtn;

}


template <typename T>
T MaxHeap<T>::value (size_t index) const
{

    _FOUNDATION_MAXHEAP_CHECK(_reserve > index);

    return _ptr[index];

}


template <typename T>
MaxHeap<T> & MaxHeap<T>::operator= (const MaxHeap<T> &rhs)
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
T & MaxHeap<T>::operator[] (size_t index)
{

    _FOUNDATION_MAXHEAP_CHECK(_reserve > index);

    return _ptr[index];

}


template <typename T>
bool MaxHeap<T>::operator== (const MaxHeap<T> &rhs)
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
class MaxHeap<T>::iterator {

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
void MaxHeap<T>::iterator::operator++ ()
{

    _cur++;

}


template <typename T>
void MaxHeap<T>::iterator::operator++ (int)
{

    _cur++;

}


template <typename T>
void MaxHeap<T>::iterator::operator-- ()
{

    _cur--;

}


template <typename T>
void MaxHeap<T>::iterator::operator-- (int)
{

    _cur--;

}


template <typename T>
bool MaxHeap<T>::iterator::operator!= (const typename MaxHeap<T>::iterator &rhs)
{

    return (_cur != rhs._cur);

}


template <typename T>
T & MaxHeap<T>::iterator::operator* ()
{

    return *_cur;

}


template <typename T>
typename MaxHeap<T>::iterator MaxHeap<T>::begin (void)
{
    MaxHeap<T>::iterator   itr(_ptr);
    return itr;

}


template <typename T>
typename MaxHeap<T>::iterator MaxHeap<T>::end   (void)
{

    MaxHeap<T>::iterator   itr(_ptr + _values);
    return itr;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
