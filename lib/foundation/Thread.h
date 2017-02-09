/**
 *****************************************************************************

 @file       Thread.h

 @brief      Thread implementation 

 @author     Hiroki Takada (TSoftware - http://www.tsoftware.jp)

 @date       2014-03-24

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
 24th Mar 2014  0.1    Initial release.
 
 *****************************************************************************/ 

#ifndef _FOUNDATION_THREAD_H
#define _FOUNDATION_THREAD_H

#include <semaphore.h>
#include <pthread.h>

#include <SpinLock.h>
#include <Semaphore.h>
#include <List.h>

// begin namespace 'foundation'
namespace foundation {

/* --------------------------- macro declaration --------------------------- */
#ifdef  _FOUNDATION_THREAD_DEBUG
#define _FOUNDATION_THREAD_CHECK(x) { assert ( (x) ); }
#define _FOUNDATION_THREAD_DEBUG_ERROR_STRING_MAX 256
#define _FOUNDATION_THREAD_DEBUG_OUT(fmt, args...)  { \
            fprintf(stderr, "FOUNDATION_THREAD_DEBUG     : " fmt, ## args); \
        }
#define _FOUNDATION_THREAD_DEBUG_FUNCTION_RESULT(x) { \
            char errorString_[_FOUNDATION_THREAD_DEBUG_ERROR_STRING_MAX]; \
            sprintf(errorString_, "%s - %s - %d", (x) , __FILE__, __LINE__); \
            perror(errorString_); \
        }
#else
#define _FOUNDATION_THREAD_CHECK(x)
#define _FOUNDATION_THREAD_DEBUG_OUT(fmt, args...)
#define _FOUNDATION_THREAD_DEBUG_FUNCTION_RESULT(x)
#endif
/* ------------------------------------------------------------------------- */



/* -------------------------- thread exceptions ---------------------------- */

class ThreadException {};

/* ------------------------------------------------------------------------- */



/* ------------------------- prototype declaration ------------------------- */

template <typename T, typename V>
class Thread;

/* ------------------------------------------------------------------------- */



/* ------------------------- thread function class ------------------------- */

template <typename T, typename V>
class Thread : public SpinLockObject {

public:

    /// global sync
    static Semaphore            globalSync;

    /// thread state
    enum ThreadState {
        INITIALIZED,
        STARTED,
        CLEARED
    };

    /// constructor
    Thread ();

    /// destructor
    virtual ~Thread ();

    /// run the thread
    void                        run      (V &);

    /// synchronize with thread termination
    void                        sync     (void);

    /// try to synchronize with thread termination
    bool                        trysync  (void);

    /// get argument of the thread
    V                           argument (void);

    /// get returned value of the thread
    T                           result   (void);

    /// yeild CPU to other threads
    static void                 yield    (void);


private:

    /// thread data
    struct _threadData {
        Thread<T,V> *           reference;
        T *                     retval;
        V                       argument;
        sem_t *                 sync;
    };

    /// void copy constructor
    Thread (const Thread &);

    /// void copy operator
    Thread &                    operator= (const Thread &);

    /// thread variable
    pthread_t                   _th;

    /// semaphore for terminal sync
    sem_t                       _sync;

    /// variable to hold thread argument
    V                           _arg;

    /// variable to hold thread returned value
    T                           _retval;

    /// flag to check if thread is running
    ThreadState                 _state;

    /// thread data
    _threadData                 _thdat;

    /// thread function
    virtual T                   _thread (V) = 0;

    /// thread uniform function
    static void *               _run (void *);

};



/**
 * Constructor
 * @param arg   argument given to the thread functoin
 */
template <typename T, typename V>
Thread<T,V>::Thread ()
 : _state(INITIALIZED)
{

    // initialize sync semaphore
    sem_init(&_sync, 0, 0);

}



/**
 * Destructor
 */
template <typename T, typename V>
Thread<T,V>::~Thread ()
{
        if (_state != CLEARED && _state != INITIALIZED) {
            pthread_join(_th, NULL);
        }
        sem_destroy(&_sync);
}



template <typename T, typename V>
void Thread<T,V>::run (V &arg)
{

    if (_state == STARTED) {
        return;
    }

    // copy arg
    _arg = arg;

    // initialize thread thread data
    _thdat.reference = this;
    _thdat.retval    = &_retval;
    _thdat.argument  = arg;
    _thdat.sync      = &_sync;

    // invoke thread
    int rtn = pthread_create(&_th, NULL, &_run, &_thdat);
    if (rtn != 0) {
        throw ThreadException();
    }

    // set state
    _state = STARTED;

}



/**
 * Wait until the threads' termination. After this synchronization, the return
 * value will be available to examine.
 */
template <typename T, typename V>
void Thread<T,V>::sync (void)
{

    if (_state != STARTED) {
        return;
    }

    // lock
    SpinLock    lock(this);

    // wait
    sem_wait(&_sync);

    // join
    pthread_join(_th, NULL);

    // set state
    _state = CLEARED;

}



/**
 * Try to synchronize with the threads' termination. Check if the attempt
 * is blocked, return immediately if the block will occur.
 * @return true if successfully synchronized with the threads' termination.
 *         false if not.
 */
template <typename T, typename V>
bool Thread<T,V>::trysync (void)
{

    _FOUNDATION_THREAD_CHECK(_state == STARTED);

    // lock
    SpinLock    lock(this);

    // try wait
    int rtn = sem_trywait(&_sync);
    if (rtn != 0 && errno == EAGAIN) {
        return false;
    }

    // join
    pthread_join(_th, NULL);

    // set state
    _state = CLEARED;

    return true;

}



/**
 * Get the argument value copied at first.
 * @return argument value
 */
template <typename T, typename V>
inline V Thread<T,V>::argument (void)
{
    // lock
    SpinLock lock(this);
    return   _arg;
}



/**
 * Get the returned value of the thread function. This isn't significant
 * before thread end.
 * @return returned value
 */
template <typename T, typename V>
inline T Thread<T,V>::result (void)
{

    _FOUNDATION_THREAD_CHECK(_state == CLEARED);

    // lock
    SpinLock lock(this);
    return   _retval;
}



/**
 * Yield CPU to other threads
 */
template <typename T, typename V>
inline void Thread<T,V>::yield (void)
{
    sched_yield();
}



/**
 * Internal wrapper function to invoke the thread function.
 * @param  data uniform argument for pthread function
 * @return uniform return value for pthread function
 */
template <typename T, typename V>
void * Thread<T,V>::_run (void *data)
{

    // cast thread data
    _threadData *thdat = static_cast<_threadData *>(data);

    // invoke thread function
    *(thdat->retval) = thdat->reference->_thread(thdat->argument);

    // sync 
    sem_post(thdat->sync);

    // global sync
    globalSync.post();

    return (void *) 0;

}

/* ------------------------------------------------------------------------- */

// end namespace 'foundation'
}

#endif
