//============================================================================
// event.h - Defines an eventfd() based event-manager
//============================================================================
#pragma once
#include <cstdint>


//============================================================================
// CEvent - A waitable event object based on the "eventfd" system call
//============================================================================
class CEvent
{
public:

    // Constructs an event object in the "untriggered" state
    CEvent();

    // Closes the event file-descriptor
    ~CEvent();

    // Resets the event to the "untriggered" state
    void    reset();

    // Triggers the event with the specified value.  If the event is already
    // triggered, this will be added to the existing event value
    void    set(uint64_t value = 1);

    // Wait "milliseconds" for the event to become triggered.  Returns either
    // the triggered value, or zero if the event hasn't happened yet.  If the
    // return value is non-zero, the event is automatically reset to the
    // "untriggered" state.  If milliseconds is 0, it will wait forever
    uint64_t wait(uint32_t milliseconds = 0);

    // Returns 'true' if the event is in the "triggered" state
    bool    is_triggered();

    // Returns the event file descriptor for use with "select"
    int     fd() {return m_fd;}


protected:

    int     m_fd;

};
//============================================================================
