/**
 *****************************************************************************

 @file       HashTree.h

 @brief      Hash tree implementation 
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-23

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
  23rd Mar 2014  0.1    Initial release.
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_HASHTREE_H
#define _FOUNDATION_HASHTREE_H

#include <List.h>

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_HASHTREE_DEBUG
#define _FOUNDATION_HASHTREE_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_HASHTREE_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_HASHTREE_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_HASHTREE_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_HASHTREE_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_HASHTREE_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_HASHTREE_CHECK(x)
#define _FOUNDATION_HASHTREE_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_HASHTREE_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* ------------------------- hash tree  exceptions ------------------------- */
class HashException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T, typename V>
class HashNode;

template <typename T, typename V>
class HashTree;
/* ------------------------------------------------------------------------- */



/* --------------------------- hash  node class ---------------------------- */

template <typename T, typename V>
class HashNode {

    friend class HashTree<T,V>;

public:

    /// default constructor
    HashNode ();

    /// constructor
    HashNode (const T &k) :
      _key(k), _value(), _left(nullptr), _right(nullptr)
    {}

    /// constructor
    HashNode (const T &k, const V &v) :
      _key(k), _value(v), _left(nullptr), _right(nullptr)
    {}

private:

    /// void copy constructor
    HashNode (const HashNode<T,V> &);

    /// void copy operation
    HashNode<T,V> &         operator= (const HashNode<T,V> &);

    /// key
    T                       _key;

    /// value
    V                       _value;

    /// pointer to the left child
    HashNode<T,V> *         _left;

    /// pointer to the right child
    HashNode<T,V> *         _right;

};

/// An example constructor to accomodate cases with specific type and a
//  constants for top node : using Zobrist hash key and Position class.
#if 0
template <>
HashNode <Zobrist::key, Position> ::HashNode () :
      _key(0x8000000000000000ULL), _value(), _left(nullptr), _right(nullptr)
    {}
#endif

/* ------------------------------------------------------------------------- */



/* ---------------------------- hash tree class ---------------------------- */

template <typename T, typename V>
class HashTree {

public:

    /// default constructor
    HashTree ();

    /// constructor
    HashTree  (const T &);

    /// constructor
    HashTree  (const T &, const V &);

    /// destructor 
    ~HashTree ();

    /// add new node
    bool                    add   (const T &);

    /// add new node
    bool                    add   (const T &, const V &);
 
    /// set the value
    bool                    set   (const T &, const V &);

    /// find the value
    bool                    find  (const T &, V &);

    /// find the value
    bool                    find  (const T &);

    /// delete all nodes other than _top
    void                    clean (void);

    /// get height
    unsigned long           height(void);

    /// get size
    unsigned long           size  (void);

private:

    /// void copy constructor
    HashTree (const HashTree<T,V> &);

    /// void copy operation
    HashTree<T,V> &         operator= (const HashTree<T,V> &);

    /// top node
    HashNode<T,V> *         _top;

    /// number of tree nodes
    unsigned long           _size;

    /// height of the tree
    unsigned long           _height;

};



/**
 * Constructor
 */
template <typename T, typename V>
HashTree<T,V>::HashTree ()
 : _size(1), _height(1)
{

    _top = new HashNode<T,V>();

}



/**
 * Constructor
 * @param key   key for the top node of the tree
 */
template <typename T, typename V>
HashTree<T,V>::HashTree (const T &key)
 : _size(1), _height(1)
{

    _top = new HashNode<T,V>(key);

}



/**
 * Constructor
 * @param key   key for the top node of the tree
 * @param value value for the top node of the tree
 */
template <typename T, typename V>
HashTree<T,V>::HashTree (const T &key, const V &value)
 : _size(1), _height(1)
{

    _top = new HashNode<T,V>(key, value);

}



/**
 * Destructor
 */   
template <typename T, typename V>
HashTree<T,V>::~HashTree ()
{

    // clean
    clean();

    // delete the top node
    delete _top;

}



/**
 * Add the given key and value to the hash tree. 
 * The type T (key) must have had three relational operators: '==', '<'
 * and '>' implimtented.
 * @param  key    key to add.
 * @return true in successful addition
 */   
template <typename T, typename V>
bool HashTree<T,V>::add (const T &key)
{

    unsigned long           level;
    HashNode<T,V>           *node;

    // recursive search from the top
    for (level = 2, node = _top; ; ++level) {

        // check duplication
        if (key == node->_key) {
            return false;
        }

        // follow children
        if (key <  node->_key) {
            if (node->_left  == nullptr) {
                node->_left   = new HashNode<T,V>(key);
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                node->_right  = new HashNode<T,V>(key);
                break;
            } else {
                node = node->_right;
                continue;
            }
        }
    }

    // increment size
    _size++;

    // update the height
    // need atomic operation when sharing the tree
    if (level > _height) {
        _height = level;
    }

    return true;

}



/**
 * Add the given key and value to the hash tree. 
 * The type T (key) must have had three relational operators: '==', '<'
 * and '>' implimtented.
 * @param  key   key to add.
 * @param  value value to add.
 * @return true in successful addition
 */   
template <typename T, typename V>
bool HashTree<T,V>::add (const T &key, const V &value)
{

    unsigned long           level;
    HashNode<T,V> *         node;

    // recursive search from the top
    for (level = 2, node = _top; ; ++level) {

        // check duplication
        if (key == node->_key) {
            return false;
        }

        // follow children
        if (key < node->_key) {
            if (node->_left  == nullptr) {
                node->_left   = new HashNode<T,V>(key, value);
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                node->_right  = new HashNode<T,V>(key, value);
                break;
            } else {
                node = node->_right;
                continue;
            }
        }
    }

    // increment size
    _size++;

    // update the height
    // need atomic operation when sharing the tree
    if (level > _height) {
        _height = level;
    }

    // successful addition
    return true;

}



/**
 * Set the value at the key. 
 * @param key   key
 * @param value value to set.
 * @return true in successful set
 */   
template <typename T, typename V>
bool HashTree<T,V>::set (const T &key, const V &value)
{

    // recursive search from the top
    HashNode<T,V> * node = _top;
    while (1) {

        // check if the node is what we are looking for
        if (key == node->_key) {
            node->_value = value;
            return true;
        }

        // follow children
        if (key <  node->_key) {
            if (node->_left  == nullptr) {
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                break;
            } else {
                node = node->_right;
                continue;
            }
        }
    }

    return false;

}



/**
 * Find the value at the key. 
 * @param  key   key
 * @param  value variable to hold the value.
 * @return true if the key exists. false if not.
 */   
template <typename T, typename V>
bool HashTree<T,V>::find (const T &key, V &var)
{

    // recursive search from the top
    HashNode<T,V> *node = _top;
    while (1) {

        // check if the node is what we are looking for
        if (key == node->_key) {
            var = node->_value;
            return true;
        }

        // follow children
        if (key <  node->_key) {
            if (node->_left  == nullptr) {
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                break;
            } else {
                node = node->_right;
                continue;
            }
        }
    }

    return false;

}



/**
 * Check if the key exists in the tree
 * @param  key   key
 * @return true if the key exists. false if not.
 */   
template <typename T, typename V>
bool HashTree<T,V>::find (const T &key)
{

    // recursive search from the top
    HashNode<T,V> *node = _top;
    while (1) {

        // check if the node is what we are looking for
        if (key == node->_key) {
            return true;
        }

        // follow children
        if (key <  node->_key) {
            if (node->_left  == nullptr) {
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                break;
            } else {
                node = node->_right;
                continue;
            }
        }
    }

    return false;

}



/**
 * Clean
 */   
template <typename T, typename V>
void HashTree<T,V>::clean (void)
{

    // save constext
    List<HashNode<T,V> *>   stack;
    HashNode<T,V> *         node  = _top;

    // recursive delete
    while (1) {

        // check if the node is leaf
        if (node->_right == nullptr && node->_left == nullptr) {
            // check dig end
            if (node == _top) {
                break;
            }
            // delete the leaf
            delete node;
            stack.tail(node);
            continue;
        }

        // save the current node
        HashNode<T,V> *prnt = node;

        // follow the right hand child first
        if (node->_right != nullptr) {
            node          = prnt->_right;
            prnt->_right  = nullptr;
        } else {
            node          = prnt->_left;
            prnt->_left   = nullptr;
        }

        // save the constext
        stack.add(prnt);

    }

    // reset
    _size   = 1;
    _height = 1;

}



/**
 * Get the height
 */   
template <typename T, typename V>
unsigned long HashTree<T,V>::height (void)
{

    return height;

}



/**
 * Get the height
 */   
template <typename T, typename V>
unsigned long HashTree<T,V>::size (void)
{

    return size;

}


/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
