/**
 *****************************************************************************

 @file       List.h

 @brief      Linked list definitions
  
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
   3rd Mar 2014  0.2    Isolate data from the structure
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_LIST_H
#define _FOUNDATION_LIST_H

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_LIST_DEBUG
#define _FOUNDATION_LIST_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_LIST_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_LIST_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_LIST_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_LIST_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_LIST_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_LIST_CHECK(x)
#define _FOUNDATION_LIST_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_LIST_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ---------------------------- list exceptions ---------------------------- */
class ListException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class ListHead;

template <typename T>
class ListItem;

template <typename T>
class List;
/* ------------------------------------------------------------------------- */



/* ---------------------------- list item class ---------------------------- */

template <typename T>
class ListHead {

    friend class List<T>;

public:
    /// destructor
    virtual ~ListHead () {}

protected:

    /// constructor List class will use
    ListHead (List<T> *ref)
        : _next (this), _prev(this), _ref(ref) {}

    /// pointer to the next list item
    ListHead<T> *       _next;

    /// pointer to the previouse list item
    ListHead<T> *       _prev;

    /// pointer to the List class containing this item
    List<T> *           _ref;

private:

    /// void constructor
    ListHead ();

    /// copy constructor
    ListHead (const ListHead<T> &);

    /// void copy operation
    ListHead<T> &       operator= (const ListHead<T> &);

};



template <typename T>
class ListItem : public ListHead<T> {

    friend class List<T>;

private:

    /// constructor List class uses
    ListItem (T newdata, List<T> *ref)
        : ListHead<T>(ref), _data(newdata) {}

    /// data to be stored in
    T                   _data;

};

/* ------------------------------------------------------------------------- */



/* ------------------------------ List  class ------------------------------ */

template <typename T>
class List
{

public:

    /// default constructor
    List  ();

    /// copy costructor
    List (const List<T> &);

    /// destructor 
    ~List ();

    /// add one item to the list
    ListItem<T> *       add     (T);

    /// get one item from a top of the list (FIFO)
    void                top     (T &);

    /// get one item from a top of the list (FIFO)
    T                   top     (void);

    /// get one item from a tail of the list (LIFO)
    void                tail    (T &);

    /// check size of the list
    size_t              size    (void) { return _size; }

    /// delete one item from the list
    void                del     (ListHead<T> *);

    /// delete one item at the top
    void                deltop  (void);

    /// delete one item at the tail
    void                deltail (void);

    /// iterator class
    class               iterator;

    /// iterator for top to tail iteration
    iterator            begin   (void);

    /// iterator for tail to top iteration 
    iterator            reverse (void);

    /// iterator to check end
    iterator            end     (void);

private:

    /// void copy operator
    List<T> &           operator= (const List<T> &);

    /// top item
    ListHead<T> *       _top;

    /// very end item 
    ListHead<T> *       _end;

    ///
    size_t              _size;


private:

    /// cast between ListHead and ListItem
    static inline ListItem<T> *_cast   (void *p)
    {
        return static_cast<ListItem<T> *>(p);
    }


};



/**
 * Default constructor
 */
template <typename T>
List<T>::List()
    : _size(0)
{

    ListHead<T>         *ptr = new ListHead<T>(this); 

    _top  = ptr;
    _end  = ptr;

}



/**
 * Copy constructor
 * @param l list to copy
 */
template <typename T>
List<T>::List(const List<T> &l)
    : _size(0)
{

    ListHead<T>         *ptr = new ListHead<T>(this); 

    _top  = ptr;
    _end  = ptr;

    for (auto i  = l._end->_prev;
              i != l._end;
              i  = i->_prev      ) {
        auto item = static_cast<ListItem<T> *>(i);
        add(item->_data);
    }

}



/**
 * Destructor
 */
template <typename T>
List<T>::~List()
{

    while (_size) {
        del(_top);
    }

    delete _top;

}



/**
 * Add the given data to the list.
 * @param  data data to add.
 * @return list item containing the data
 */
template <typename T>
ListItem<T> * List<T>::add (T data)
{

    ListItem<T>         *ptr = new ListItem<T>(data, this);
    ListHead<T>         *prev;

    prev        = _top->_prev;
     ptr->_prev = _top->_prev;
     ptr->_next = _top;
    _top->_prev = ptr;
    prev->_next = ptr;

    _top        = ptr;
    _size++;

    return ptr;

}


/**
 * Pick up the data from tail of the list. (LIFO)
 * @param data variable to hold the data picked up.
 */
template <typename T>
void List<T>::tail (T &data)
{

    // exception on peeking the empty list
    _FOUNDATION_LIST_CHECK(_size);

    // copy data
    // copy operation must have been implemented on type <T>
    data = (_cast(_top))->_data;

    // delete
    del(_top);

}



/**
 * Pick up the data from top of the list. (FIFO)
 * @param data variable to hold the data picked up.
 */
template <typename T>
void List<T>::top (T &data)
{

    // exception on peeking the empty list
    _FOUNDATION_LIST_CHECK(_size);

    // copy data
    // copy operation must have been implemented on type <T>
    data = (_cast(_end->_prev))->_data;

    // delete
    del(_end->_prev);

}



/**
 * Pick up the data from top of the list. (FIFO)
 * @return data at top of the list.
 */
template <typename T>
T List<T>::top (void)
{

    // exception on peeking the empty list
    _FOUNDATION_LIST_CHECK(_size);

    // copy data
    // copy operation must have been implemented on type <T>
    T data = (_cast(_end->_prev))->_data;

    // delete
    del(_end->_prev);

    return data;

}



/**
 * Delete the list item.
 * @param ptr the list item to be deleted.
 */
template <typename T>
void List<T>::del (ListHead<T> *ptr)
{

    ListHead<T>         *prev;
    ListHead<T>         *next;

    // sanity checks
    _FOUNDATION_LIST_CHECK(ptr);
    _FOUNDATION_LIST_CHECK(_size);
    _FOUNDATION_LIST_CHECK(_top      != _end);
    _FOUNDATION_LIST_CHECK(ptr->_ref == this);

    prev        = ptr->_prev;
    next        = ptr->_next;

    if (ptr == _top) {
        _top = ptr->_next;
    }

    prev->_next = next;
    next->_prev = prev;

    // dereference
    delete ptr;

    _size--;

}



/**
 * Delete the item at tail.
 */
template <typename T>
void List<T>::deltop (void)
{

    del(_end->_prev);

}



/**
 * Delete the item at tail.
 */
template <typename T>
void List<T>::deltail (void)
{

    del(_top);

}



template <typename T>
class List<T>::iterator {

public:

    /// constructor
    iterator (ListHead<T> *ptr)
        : _cur(ptr), _prev(ptr->_prev),
          _next(ptr->_next) , _list(ptr->_ref) {}

    /// go forward
    void                operator++ ();
    void                operator++ (int);

    /// go backward
    void                operator-- ();
    void                operator-- (int);

    /// a relational operator to check end of iterations
    bool                operator!= (const iterator &);

    /// operator to obtain the data
    T                   operator*  ();

    /// delete the item
    void                del        (void);


private:
    ListHead<T>         *_cur;
    ListHead<T>         *_prev;
    ListHead<T>         *_next;
    List<T>             *_list;

};


template <typename T>
void List<T>::iterator::operator-- ()
{

    _cur  = _next;
    _next = _cur->_next;
    _prev = _cur->_prev;

}


template <typename T>
void List<T>::iterator::operator-- (int)
{

    _cur  = _next;
    _next = _cur->_next;
    _prev = _cur->_prev;

}



template <typename T>
void List<T>::iterator::operator++ ()
{

    _cur  = _prev;
    _next = _cur->_next;
    _prev = _cur->_prev;

}



template <typename T>
void List<T>::iterator::operator++ (int)
{

    _cur  = _prev;
    _next = _cur->_next;
    _prev = _cur->_prev;

}



template <typename T>
bool List<T>::iterator::operator!= (const typename List<T>::iterator &rhs)
{

    return (_cur != rhs._cur);

}



template <typename T>
T List<T>::iterator::operator* ()
{

    return _cast(_cur)->_data;

}



template <typename T>
void List<T>::iterator::del (void)
{

    _list->del(_cur);

}



/**
 * Open an iteration from the first item.
 * @return iterator
 */
template <typename T>
typename List<T>::iterator List<T>::begin   (void)
{

    List<T>::iterator   itr(_end->_prev);
    return itr;

}


/**
 * Open an iteration from the last item.
 * @return iterator
 */
template <typename T>
typename List<T>::iterator List<T>::reverse (void)
{

    List<T>::iterator   itr(_top);
    return itr;

}


/**
 * Terminator
 * @return iterator to check end of the list.
 */
template <typename T>
typename List<T>::iterator List<T>::end     (void)
{

    List<T>::iterator   itr(_end);
    return itr;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
