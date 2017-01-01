/**
 *****************************************************************************

 @file       BTree.h

 @brief      Binary tree implementation 
  
 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-16

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
  16th Mar 2014  0.1    Initial release.
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_BTREE_H
#define _FOUNDATION_BTREE_H

#include <List.h>

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_BTREE_DEBUG
#define _FOUNDATION_BTREE_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_BTREE_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_BTREE_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_BTREE_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_BTREE_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_BTREE_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_BTREE_CHECK(x)
#define _FOUNDATION_BTREE_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_BTREE_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* --------------------------- btree  exceptions --------------------------- */
class BTreeException {};
/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */
template <typename T>
class BTreeNode;

template <typename T>
class BTree;
/* ------------------------------------------------------------------------- */



/* --------------------------- btree node class ---------------------------- */

template <typename T>
class BTreeNode {

    friend class BTree<T>;

public:

    /// default constructor
    BTreeNode ();

    /// constructor
    BTreeNode (const T &v) :
      _value(v), _left(nullptr), _right(nullptr) {}

private:

    /// void copy constructor
    BTreeNode (const BTreeNode<T> &);

    /// void copy operation
    BTreeNode<T> &          operator= (const BTreeNode<T> &);

    /// value
    T                       _value;

    /// pointer to the left child
    BTreeNode<T> *          _left;

    /// pointer to the right child
    BTreeNode<T> *          _right;

};

/// An example constructor using Zobrist hash key.
#if 0
template <>
BTreeNode <Zobrist::key> ::BTreeNode () :
      _value(0x8000000000000000ULL), _left(nullptr), _right(nullptr)
    {}
#endif

/* ------------------------------------------------------------------------- */



/* --------------------------- binary tree class --------------------------- */
template <typename T>
class BTree {

public:

    /// void default constructor
    BTree  ();

    /// constructor
    BTree  (const T &);

    ///
    ~BTree ();

    /// add new node
    bool                    add    (const T &);

    /// find the value
    bool                    find   (const T &);

    /// find the value
    unsigned long           height (void) { return _height; }

    /// clean up the tree
    void                    clean  (void);

private:

    /// void copy constructor
    BTree  (const BTree<T> &);

    /// void copy operation
    BTree<T> &              operator= (const BTree<T> &);

    /// top node
    BTreeNode<T> *          _top;

    /// number of tree nodes
    unsigned long           _size;

    /// height of the tree
    unsigned long           _height;

};



/**
 * Constructor
 * @param value value for the top node of the tree
 */
template <typename T>
BTree<T>::BTree ()
 : _size(1), _height(1)
{

    _top = new BTreeNode<T>();

}



/**
 * Constructor
 * @param value value for the top node of the tree
 */
template <typename T>
BTree<T>::BTree (const T &value)
 : _size(1), _height(1)
{

    _top = new BTreeNode<T>(value);

}



/**
 * Destructor
 */   
template <typename T>
BTree<T>::~BTree ()
{

    clean();
    delete _top;

}



/**
 * Add the given value to the tree. The fuction will end in failure
 * The type T must have had three relational operators: '==', '<'
 * and '>' implimtented.
 * @param  value  value to add.
 * @return true in successful addition. false in duplication.
 */   
template <typename T>
bool BTree<T>::add (const T &value)
{

    unsigned long           level;
    BTreeNode<T> *          node;

    // recursive search from the top
    for (level = 2, node = _top; ; ++level) {

        // check duplication
        if (value == node->_value) {
            return false;
        }

        // follow children
        if (value <  node->_value) {
            if (node->_left  == nullptr) {
                node->_left   = new BTreeNode<T>(value);
                break;
            } else {
                node = node->_left;
                continue;
            }
        } else {
            if (node->_right == nullptr) {
                node->_right  = new BTreeNode<T>(value);
                break;
            } else {
                node = node->_right;
                continue;
            }
        }

    }

    // increment size
    _size++;

    // need atomic operation when sharing the tree
    if (level > _height) {
        _height = level;
    }

    return true;

}



/**
 * Find the value in the tree.
 * The type T must have had three relational operators: '==', '<'
 * and '>' implimtented.
 * @param value value to retrieve.
 * @return true if the value exists. false if not.
 */   
template <typename T>
bool BTree<T>::find (const T &value)
{

    unsigned long           level;
    BTreeNode<T>            *node;

    // recursive search from the top
    for (level = 2, node = _top; ; ++level) {

        // check duplication
        if (value == node->_value) {
            return true;
        }

        // follow children
        if (value <  node->_value) {
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
 * Clean up the tree
 */   
template <typename T>
void BTree<T>::clean (void)
{

    // save constext
    List<BTreeNode<T> *>    stack;
    BTreeNode<T> *          node  = _top;

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
        BTreeNode<T> * prnt = node;

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

    _size   = 1;
    _height = 1;

}


/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
