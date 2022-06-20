//============================================================================
// event.cpp - Implements an event manager
//============================================================================
#include "event.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/select.h>


//============================================================================
// Constructor - Creates the event object in the untriggered state
//============================================================================
CEvent::CEvent()
{
    m_fd = eventfd(0, 0);
}
//============================================================================


//============================================================================
// Destructor - Closes the event file descriptor
//============================================================================
CEvent::~CEvent()
{
    if (m_fd > 0) close(m_fd);
    m_fd = -1;
}
//============================================================================


//============================================================================
// is_triggered() - Returns 'true' if the event is currently triggered
//============================================================================
bool CEvent::is_triggered()
{
    fd_set rfds;

    // We're effectively going to poll
    timeval timeout = {0,0};

    // We're only polling just the one specified file-descriptor
    FD_ZERO(&rfds);
    FD_SET(m_fd, &rfds);

    // Is there data on this descriptor?
    int count = select(m_fd+1, &rfds, NULL, NULL, &timeout);

    // Tell the caller whether there is an event value ready for reading
    return (count > 0);
}
//============================================================================



//============================================================================
// Reset() - Resets this object to the "untriggered" state
//============================================================================
void CEvent::reset()
{
    char value[8];

    // If the event is in the triggered state, clear it
    if (is_triggered()) read(m_fd, &value, sizeof value);
}
//============================================================================


//============================================================================
// set() - Triggers the event with the specified value
//============================================================================
void CEvent::set(uint64_t value)
{
    write(m_fd, &value, sizeof(value));
}
//============================================================================


//============================================================================
// wait() - Waits the specified number of milliseconds for the event to
//          become triggered.
//
// Passed:  milliseconds = number of milliseconds to wait.  0 = Forever
//
// Returns: The event value, or 0 if the event is untriggered
//============================================================================
uint64_t CEvent::wait(uint32_t milliseconds)
{
    uint64_t    event_value;
    fd_set      rfds;

    // Convert 'milliseconds' into seconds and microseconds
    int32_t seconds = milliseconds / 1000;
    int32_t microseconds = (milliseconds - seconds * 1000) * 1000;
    
    // We're going to wait the specified number of milliseconds for the event
    // to become triggered
    timeval timeout = {seconds, microseconds};

    // Decide whether to pass "select" a timeval or a NULL
    timeval* p_timeout = (milliseconds == 0) ? nullptr : &timeout;

    // We're only polling just the one specified file-descriptor
    FD_ZERO(&rfds);
    FD_SET(m_fd, &rfds);

    // Is there data on this descriptor?
    int count = select(m_fd+1, &rfds, NULL, NULL, p_timeout);

    // If there is no data available, tell the caller that we're untriggered
    if (count < 1) return 0;

    // We're triggered.  Read the event value
    read(m_fd, &event_value, sizeof(event_value));

    // Hand the caller the event value
    return event_value;
}
//============================================================================
