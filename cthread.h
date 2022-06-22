//=============================================================================
// CThread() - Encapsulates a linux pthread along with support classes
//=============================================================================
#pragma once
#include "pthread.h"



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

    // Terminates a thread.  If wait_flag is true, it will wait for the
    // thread to finish executing
    void    cancel(bool wait_flag = true);


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