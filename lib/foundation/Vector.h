/*****************************************************************************

 @file       Vector.h

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

#ifndef _FOUNDATION_VECTOR_H
#define _FOUNDATION_VECTOR_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_VECTOR_DEBUG
#define _FOUNDATION_VECTOR_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_VECTOR_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_VECTOR_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_VECTOR_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_VECTOR_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_VECTOR_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_VECTOR_CHECK(x)
#define _FOUNDATION_VECTOR_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_VECTOR_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ---------------------------- vector exceptions  ------------------------- */
class VectorException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class Vector;
/* ------------------------------------------------------------------------- */



/* ---------------------------- Vector  class ------------------------------ */

template <typename T>
class Vector

{

public:

    /// Constructor
    Vector ()
        : _reserve(0), _values(0), _ptr(nullptr) {}

    /// Constructor with size 
    Vector (size_t);

    /// Constructor with size and initial value
    Vector (size_t, T);

    /// Copy constructor
    /// copies ONLY the significant elements
    Vector (const Vector<T> &);

    /// Destructor
    ~Vector ();

    /// Resize
    void        resize (size_t);

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

    /// Check number of elements
    size_t      vsize  (void) { return _values;     }

    /// Set number of elements to vector size
    void        setsz  (void) { _values = _reserve; }

    /// Set number of elements
    void        setsz  (const size_t &sz)
                       { _values = _reserve <= sz ?
                                   _reserve :  sz; }
    /// Get value
    T           value  (size_t) const;

    /// Assign
    /// copies ONLY the significant elements
    Vector<T> & operator=  (const Vector<T> &);

    /// Check if same
    /// compares ONLY the significant elements
    bool        operator== (const Vector<T> &);

    /// Array access
    T &         operator[] (size_t);

    /// iterator
    class       iterator;
    iterator    begin  (void);
    iterator    end    (void);


protected:

    size_t      _reserve;
    size_t      _values;
    T *         _ptr;

};


template <typename T>
Vector<T>::Vector (size_t size)
    : _reserve(size), _values(0)
{
    if (size) {
        _ptr = new T[size];
    }

}


template <typename T>
Vector<T>::Vector (size_t size, T data)
    : _reserve(size), _values(0)
{

    _ptr = new T[size];

    for (size_t i = 0; i < _reserve; ++i) {
        _ptr[i] = data;
    }

}


template <typename T>
Vector<T>::Vector (const Vector<T> &data)
    : _reserve(data._reserve), _values(data._values)
{

    _ptr  = new T[data._reserve];

    operator=(data);

}


template <typename T>
Vector<T>::~Vector ()
{

    delete [] _ptr;

}


template <typename T>
void Vector<T>::resize (size_t size)
{

    T *ptr  = new T[size];
    _values = _values > size ? size : _values;

    if (_ptr) {
        for (size_t i = 0; i < _values; ++i) {
            ptr[i] = _ptr[i];
        }
        delete [] _ptr;
    }

    _reserve  = size;
    _ptr      = ptr;

}


template <typename T>
void Vector<T>::add (const T &data)
{

    _FOUNDATION_VECTOR_CHECK(_values < _reserve);

    _ptr[_values++] = data;

}


template <typename T>
T Vector<T>::del (void)
{

    _FOUNDATION_VECTOR_CHECK(_values != 0);

    return _ptr[_values--];

}


template <typename T>
void Vector<T>::acsort (void)
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


template <typename T>
void Vector<T>::dcsort (void)
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


template <typename T>
T Vector<T>::value (size_t index) const
{

    _FOUNDATION_VECTOR_CHECK(_reserve > index);
    return _ptr[index];

}


template <typename T>
Vector<T> & Vector<T>::operator= (const Vector<T> &rhs)
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
T & Vector<T>::operator[] (size_t index)
{

    _FOUNDATION_VECTOR_CHECK(_reserve > index);
    return _ptr[index];

}


template <typename T>
bool Vector<T>::operator== (const Vector<T> &rhs)
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
class Vector<T>::iterator {

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
void Vector<T>::iterator::operator++ ()
{

    _cur++;

}


template <typename T>
void Vector<T>::iterator::operator++ (int)
{

    _cur++;

}


template <typename T>
void Vector<T>::iterator::operator-- ()
{

    _cur--;

}


template <typename T>
void Vector<T>::iterator::operator-- (int)
{

    _cur--;

}


template <typename T>
bool Vector<T>::iterator::operator!= (const typename Vector<T>::iterator &rhs)
{

    return (_cur != rhs._cur);

}


template <typename T>
T & Vector<T>::iterator::operator* ()
{

    return *_cur;

}


template <typename T>
typename Vector<T>::iterator Vector<T>::begin (void)
{
    Vector<T>::iterator   itr(_ptr);
    return itr;

}


template <typename T>
typename Vector<T>::iterator Vector<T>::end   (void)
{

    Vector<T>::iterator   itr(_ptr + _values);
    return itr;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
