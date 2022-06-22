//==========================================================================================================
// cvsprintf.cpp - Implements a helper object for performing vsprintf without overflowing a buffer
//==========================================================================================================
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "cvsprintf.h"



//==========================================================================================================
// printf() - Works exactly like vsprintf, with an optional prefix to output
//==========================================================================================================
char* Cvsprintf::printf(const char* fmt, va_list& ap, const char* prefix, int extra)
{
    // Point to our local buffer
    char* out = m_local;

    // We know that the terminating nul-byte will take one byte of our buffer
    int length = 1 + extra;

    // Find out how long the prefix is
    int prefix_length = prefix ? strlen(prefix) : 0;

    // If the caller supplied us with a prefix...
    if (prefix)
    {
        // Stuff it into our local buffer
        strcpy(out, prefix);

        // Keep track of how many bytes of the buffer we've used up
        length += prefix_length;

        // And update the pointer to the end of our output sting
        out += length;
    }

    // Stuff the formatted string into the local buffer
    length += vsnprintf(out, sizeof(m_local) - length, fmt, ap);

    // If the entire string fit into our local buffer, we're done
    if (length <= sizeof(m_local)) return m_local;

    // We're going to need to use a buffer on the heap.  If one exists already, destroy it
    if (m_heap)  { delete[] m_heap; m_heap = nullptr; }

    // Allocate enough space on the heap for our entire string (plus the extra bit)
    m_heap = new char[length];

    // Point to the place where we intend to store our formatted string
    out = m_heap;

    // If we have a prefix, place it into the buffer
    if (prefix)
    {
        strcpy(out, prefix);
        out += prefix_length;
    }

    // Now stuff our formatted text into the buffer
    vsnprintf(out, length, fmt, ap);

    // And hand the caller the pointer to the heap buffer
    return m_heap;
}
//==========================================================================================================
