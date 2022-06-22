//==========================================================================================================
// Cvsprintf() - A helper object for performing vsprintf without overflowing a buffer
//==========================================================================================================
#pragma once

class Cvsprintf
{
public:
    // Constructor just sets the heap pointer to null
    Cvsprintf() {m_heap = nullptr;}
    
    // Destructor ensures that any allocated heap memory is freed
    ~Cvsprintf() {delete[] m_heap;}

    // This works exactly like vsprintf
    char*   printf(const char* fmt, va_list& ap, const char* prefix = nullptr, int extra = 0);

protected:

    char m_local[256];
    char* m_heap;
};
