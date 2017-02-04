#include <iostream>
#include <iomanip>

#ifndef _CN_H
#define _CN_H

template <typename T>
class CN {

public:

    static const int        Immense;

    CN()    : _v(0), _inf(true)  {}
    CN(T v) : _v(v), _inf(false) {}

    CN &                    operator=  (const T  &);
    CN &                    operator+= (const CN &);
    CN &                    operator-= (const CN &);
    CN                      operator+  (const CN &);
    CN                      operator-  (const CN &);
    bool                    operator<  (const CN &) const;
    bool                    operator<= (const CN &) const;
    bool                    operator>  (const CN &) const;
    bool                    operator>= (const CN &) const;
    bool                    operator!= (const T  &) const;

    void                    infinity (void) { _inf = true;
                                              _v   = 0;    }
    T                       value    (void) 
                                      const { return _v;   }
    bool                    isinf    (void)
                                      const { return _inf; }

private:
    T                       _v;
    bool                    _inf;

};

template <typename T>
CN<T> & CN<T>::operator=  (const T &rhs)
{
    _v    = rhs;
    _inf  = false;
    return *this;
}

template <typename T>
CN<T> & CN<T>::operator+= (const CN &rhs)
{
    if (rhs._inf) {
        _inf = true;
        _v   = 0;
    } else
    if (! _inf)   {
        _v  += rhs._v;
        if (_v >= Immense) {
            _inf = true;
            _v   = 0;
        }
    }
    return *this;
}

template <typename T>
CN<T> & CN<T>::operator-= (const CN &rhs)
{

    // in the context of pf-dn search
    // rhs must not be infinite
    if (rhs._v == 0) {
        return *this;
    }

    if (_inf) {
        _inf = false;
        _v   = Immense - (rhs._v - 1);
    } else {
        _v  -= rhs._v;
    }
    return *this;
}

template <typename T>
CN<T> CN<T>::operator+ (const CN &rhs)
{

    CN<T> v(*this);
    return (v += rhs);

}

template <typename T>
CN<T> CN<T>::operator- (const CN &rhs)
{

    CN<T> v(*this);
    return (v -= rhs);

}

template <typename T>
bool CN<T>::operator< (const CN &rhs) const
{
    if (_inf)     {
        return false;
    }
    if (rhs._inf) {
        return true;
    }
    return (_v < rhs._v);
}

template <typename T>
bool CN<T>::operator<= (const CN &rhs) const
{

    if (_inf && rhs._inf) {
        return true;
    }
    if (rhs._inf) {
        return true;
    }
    if (_inf)     {
        return false;
    }
    return (_v <= rhs._v);
}

template <typename T>
bool CN<T>::operator> (const CN &rhs) const
{
    if (rhs._inf) {
        return false;
    }
    if (_inf)     {
        return true;
    }
    return (_v > rhs._v);
}

template <typename T>
bool CN<T>::operator>= (const CN &rhs) const
{

    if (_inf && rhs._inf) {
        return true;
    }
    if (rhs._inf) {
        return false;
    }
    if (_inf)     {
        return true;
    }
    return (_v >= rhs._v);
}

template <typename T>
bool CN<T>::operator!= (const T &rhs) const
{

    if (_inf) {
        return false;
    }
    return (_v != rhs);
}

template <typename T>
std::ostream & operator<< (std::ostream &os, const CN<T> &v)
{

    if (v.isinf()) {
        os << "    Inf";
    } else {
        os << std::setw(7) << std::setfill(' ') << v.value();
    }

    return os;
}

#endif
