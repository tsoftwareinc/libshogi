/**
 *****************************************************************************

 @file       Utility.cpp

 @brief      Common utility programs implementation 

 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-28

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
  28th Mar 2014  0.1    Initial release.

 *****************************************************************************/

#include <dSFMT.h>
#include <BTree.h>
#include <Utility.h>

using namespace foundation;

// begin name space 'utility'
namespace utility {

/* --------------------------- global  variables --------------------------- */

// random seed
static uint32_t             _randomStateSeed;

// binary tree to check if a newly generated random number is duplicate
static BTree<uint64_t> *    _randomNumberCheck = nullptr;
static const uint64_t       _midValue          = 0x8000000000000000ULL;
static bool                 _initTree          = false;

// state vector
static dsfmt_t              _d;

/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * Initialize binary tree
 */
void initRandomTree (void)
{

    if (! _initTree) {
        // initialize btree
        _randomNumberCheck = new BTree<uint64_t>(_midValue);
        _initTree = true;
    }

}



/**
 * Initialize random number generator with seed
 */
void initRandom (int s)
{

    // initialize rand function
    srand(static_cast<int>(s));

    // initialize
    _randomStateSeed = static_cast<uint32_t>(rand());

    // initilize dSFMT
    dsfmt_init_gen_rand(&_d, _randomStateSeed);

}



/**
 * Initialize random number generator
 */
void initRandom (void)
{

    static time_t           t;

    // prepare random seed
    time(&t);

    // initialize rand function
    initRandom(static_cast<int>(t));

}



/**
 * Generate a 32bit random number
 * @return random number
 */
uint32_t random (void)
{

    return dsfmt_genrand_uint32(&_d);

}



/**
 * Generate a 64 bit random number
 * @param var variable to hold the random number
 * @return 
 */
uint64_t Random (void)
{

    uint64_t                var;

    var =   ( static_cast<uint64_t>(dsfmt_genrand_uint32(&_d)) << 32 )
          ^ ( static_cast<uint64_t>(dsfmt_genrand_uint32(&_d)) << 16 )
          ^   static_cast<uint64_t>(dsfmt_genrand_uint32(&_d));

    return var;

}



/**
 * Generate a random number not duplicated
 * @param var variable to hold the random number
 * @return 64 bit unique random number 
 */
uint64_t uniqueRandom (void)
{

    uint64_t                var;

    // generate random number
    do {
        var = Random();
    } while (! _randomNumberCheck->add(var));

    return var;

}



/**
 * Clean up
 */
void cleanRandomTree (void)
{

    if (_randomNumberCheck != 0) {
        delete _randomNumberCheck;
        _initTree = false;
    }

}



/* ------------------------------------------------------------------------- */

// endo of name space 'utility'
}
