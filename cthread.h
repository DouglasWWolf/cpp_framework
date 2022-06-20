//=============================================================================
// CThread() - Encapsulates a linux pthread along with support classes
//=============================================================================
#pragma once
#include "pthread.h"


//=============================================================================
// PCriticalSection() - Implement a critical-section mutex.  These should
//                      NEVER be declared on the stack (because they will
//                      be ineffective)
//=============================================================================
class PCriticalSection
{
public:

    // Constructor() - Initializes the mutex
    PCriticalSection();

    // Waits until a lock is obtained on the mutex
    void    lock()      {pthread_mutex_lock(&m_mutex);}

    // Unlocks the mutex
    void    unlock()    {pthread_mutex_unlock(&m_mutex);}

    // Tries to lock the mutex and returns 'false' if it cant
    bool    try_lock()   {return pthread_mutex_trylock(&m_mutex);}

protected:

    pthread_mutex_t m_mutex;
};
//=============================================================================


//=============================================================================
// PSingleLock - Use these to perform convenient locking of PCriticalSections
//
// Note: These are intended to be created on the stack
//=============================================================================
class PSingleLock
{
public:

    // Constructor
    PSingleLock(PCriticalSection *cs, bool lock_now = true)
    {
        m_is_locked = false;
        m_cs = cs;
        if (lock_now) lock();
    }

    // Destructor, unlocks the mutex
    ~PSingleLock() {unlock();}

    // Call this to lock the mutex
    void    lock()
    {
        if (!m_is_locked) m_cs->lock();
        m_is_locked = true;
    }

    // Call this to unlock the mutex
    void    unlock()
    {
        if (m_is_locked) m_cs->unlock();
        m_is_locked = false;
    }

    // Call this to attempt a lock on the mutex
    bool    try_lock()
    {
        if (m_is_locked) return true;
        m_is_locked = m_cs->try_lock();
        return m_is_locked;
    }

protected:
    PCriticalSection*   m_cs;
    bool                m_is_locked;
};
//=============================================================================


//=============================================================================
// CThread - Encapsulates a POSIX thread
//=============================================================================
class CThread
{
//----------------------------------------------------------------------------
// These routines implement the spawned thread
//----------------------------------------------------------------------------
protected:

    friend void* launch_cthread(void*);

    // This is the entry point to the thread when it starts
    virtual void main(void* p1=0, void* p2=0, void* p3=0) {};

    // Call this to terminate the thread
    virtual void terminate();

//----------------------------------------------------------------------------
// These API's are for other threads to interface with this thread
//----------------------------------------------------------------------------
public:

    // Default Constructor
    CThread();

    // All base-classes should have virtual destructors
    virtual  ~CThread() {};

    // Call this to spawn the thread
    int     spawn(void* p1=0, void* p2=0, void* p3=0);

    // Call this to manually change the thread ID
    void    set_thread_id(int id) {m_id = id;}

    // Call this to join (i.e., merge wait on) this thread
    void    join();

    // Terminates a thread.  If bWaitFlag is true, it will wait for the
    // thread to finish executing
    void    cancel(bool bWaitFlag = true);


//----------------------------------------------------------------------------
// Thread-local storage
//----------------------------------------------------------------------------
protected:

    // The POSIX thread object
    pthread_t   m_thread;

    // The ID of this thread
    int         m_id;

    // Global count of the # of CThread objects that have been created
    static int  m_thread_count;

};
//=============================================================================