#include "autolock.h"

AutoLock::AutoLock(HANDLE mutex, const char *name)
{
    int idx = 0;
    while (true)
    {
        if (WAIT_OBJECT_0 == WaitForSingleObject(m_mutex, 10000))
        {
            break;
        }
        else
        {
            idx++;
        }
    }
}

AutoLock::~AutoLock()
{
    ReleaseMutex(m_mutex);
}
