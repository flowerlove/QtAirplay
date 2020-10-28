#ifndef AUTOLOCK_H
#define AUTOLOCK_H

#include <Windows.h>

class AutoLock
{
public:
    AutoLock(HANDLE mutex, const char* name = NULL);
    ~AutoLock();

private:
    HANDLE m_mutex;
};

#endif // AUTOLOCK_H
