//==========================================================================================================
// cthread.h - Defines a class for conveniently creating worker threads
//==========================================================================================================
#pragma once
#include <thread>

class CThread
{
public:

    // Default constructor
    CThread();

    // Call this to spawn the thread
    void    spawn(const void* p1=0, const void* p2=0, const void* p3=0, const void* p4=0);

    // Call this to join (i.e., wait for the completion of) this thread
    void    join() {m_thread.join();}

    // Call this to fetch the unique index of this thread
    int     get_index() {return m_thread_index;}

    // Call this to over-ride the index that was automatically assigne at construction time
    void    set_index(int idx) {m_thread_index = idx;}

    // Call this to fetch the number of currently running threads
    int     running_threads() {return m_running_threads;}


protected:

    // Over-ride this with the entry-point to your thread
    virtual void  main() {}

    // When main starts up, these are parameters passed in via spawn()
    void    *m_p1, *m_p2, *m_p3, *m_p4;

    // An integer that refers uniquely to this thread
    int     m_thread_index;

private:

    // This is a count of running threads
    static int m_running_threads;

    // This is a count of the number of threads that have been constructed
    static int m_constructed_threads;

    // This is the actual thread object
    std::thread m_thread;

    // This is the entry point for the new thread
    void entry_point();
};
