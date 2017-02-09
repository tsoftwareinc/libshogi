/*****************************************************************************

 @file       Array.h

 @brief      Array implementation

 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2012-07-03

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
   3rd JUL 2012  0.1    Initial release.
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_ARRAY_H
#define _FOUNDATION_ARRAY_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_ARRAY_DEBUG
#define _FOUNDATION_ARRAY_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_ARRAY_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_ARRAY_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_ARRAY_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_ARRAY_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_ARRAY_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_ARRAY_CHECK(x)
#define _FOUNDATION_ARRAY_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_ARRAY_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* --------------------------- Array exceptions  --------------------------- */
class ArrayException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T, const size_t Size>
class Array;
/* ------------------------------------------------------------------------- */



/* ---------------------------- Array  class ------------------------------ */

template <typename T, const size_t Size>
class Array

{

public:

    /// Constructor
    Array ()
        : _reserve(Size), _values(0) {}

    /// Constructor with initial value
    Array (T);

    /// Copy constructor 
    /// copies ONLY the significant elements
    Array (const Array<T, Size> &);

    /// Add entry
    void        add    (const T &);

    /// Delete entry
    T           del    (void);

    /// Sort in ascending order
    void        acsort (void);

    /// Sort in decending order
    void        dcsort (void);

    /// Check size
    size_t      size   (void) { return _reserve;    }

    /// Check number of significant elements
    size_t      vsize  (void) { return _values;     }

    /// Set number of elements to array size
    void        setsz  (void) { _values = _reserve; }

    /// Set number of significant elements
    void        setsz  (const size_t &sz)
                       { _values = _reserve <= sz ?
                                      _reserve : sz;}

    /// Get value
    T           value  (size_t) const;

    /// Assign
    /// copies ONLY the significant elements
    Array<T,Size> & operator= (const Array<T, Size> &);

    /// Check if same
    /// compares ONLY the significant elements
    bool        operator== (const Array<T, Size> &);

    /// Array access
    T &         operator[] (size_t);

    /// iterator
    class       iterator;
    iterator    begin  (void);
    iterator    end    (void);


private:

    size_t      _reserve;
    size_t      _values;
    T           _ptr[Size];

};


template <typename T, const size_t Size>
Array<T, Size>::Array (T data)
    : _reserve(Size), _values(0)
{

    for (size_t i = 0; i < _reserve; ++i) {
        _ptr[i] = data;
    }

}


template <typename T, const size_t Size>
Array<T, Size>::Array (const Array<T, Size> &data)
    : _reserve(data._reserve), _values(data._values)
{

    operator=(data);

}


template <typename T, const size_t Size>
void Array<T, Size>::add (const T &data)
{

    // sanity check
    _FOUNDATION_ARRAY_CHECK(_values < _reserve);

    _ptr[_values++] = data;

}


template <typename T, const size_t Size>
T Array<T, Size>::del (void)
{

    // sanity check
    _FOUNDATION_ARRAY_CHECK(_values != 0);
    return _ptr[_values--];

}


template <typename T, const size_t Size>
void Array<T, Size>::acsort (void)
{

    for (int i = 0; i < static_cast<int>(_values - 1); ++i) {
        for (int j = static_cast<int>(_values - 1); j > i; --j) {
            if (_ptr[j] < _ptr[j - 1]) {
                T         t = _ptr[  j  ];
                _ptr[  j  ] = _ptr[j - 1];
                _ptr[j - 1] = t;
            }
        }
    }

}


template <typename T, const size_t Size>
void Array<T, Size>::dcsort (void)
{

    for (int i = 0; i < static_cast<int>(_values - 1); ++i) {
        for (int j = static_cast<int>(_values - 1); j > i; --j) {
            if (_ptr[j] > _ptr[j - 1]) {
                T         t = _ptr[  j  ];
                _ptr[  j  ] = _ptr[j - 1];
                _ptr[j - 1] = t;
            }
        }
    }

}


template <typename T, const size_t Size>
T Array<T, Size>::value (size_t index) const
{

    _FOUNDATION_ARRAY_CHECK(_reserve > index);
    return _ptr[index];

}


template <typename T, const size_t Size>
Array<T, Size> & Array<T, Size>::operator= (const Array<T, Size> &rhs)
{

    _values = _reserve < rhs._values ? _reserve : rhs._values;

    for (size_t i = 0; i < _values; ++i) {
        _ptr[i] = rhs._ptr[i];
    }

    return *this;

}


template <typename T, const size_t Size>
T & Array<T, Size>::operator[] (size_t index)
{

    _FOUNDATION_ARRAY_CHECK(_reserve > index);
    return _ptr[index];

}


template <typename T, const size_t Size>
bool Array<T, Size>::operator== (const Array<T, Size> &rhs)
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


template <typename T, const size_t Size>
class Array<T, Size>::iterator {

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


template <typename T, const size_t Size>
void Array<T, Size>::iterator::operator++ ()
{

    _cur++;

}


template <typename T, const size_t Size>
void Array<T, Size>::iterator::operator++ (int)
{

    _cur++;

}


template <typename T, const size_t Size>
void Array<T, Size>::iterator::operator-- ()
{

    _cur--;

}


template <typename T, const size_t Size>
void Array<T, Size>::iterator::operator-- (int)
{

    _cur--;

}


template <typename T, const size_t Size>
bool Array<T, Size>::iterator::operator!= (const typename Array<T, Size>::iterator &rhs)
{

    return (_cur != rhs._cur);

}


template <typename T, const size_t Size>
T &  Array<T, Size>::iterator::operator* ()
{

    return *_cur;

}


template <typename T, const size_t Size>
typename Array<T, Size>::iterator Array<T, Size>::begin (void)
{
    Array<T, Size>::iterator   itr(_ptr);
    return itr;

}


template <typename T, const size_t Size>
typename Array<T, Size>::iterator Array<T, Size>::end   (void)
{

    Array<T, Size>::iterator   itr(_ptr + _values);
    return itr;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
