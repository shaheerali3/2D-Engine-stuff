// winlite.h - Minimal Windows API declarations (no windows.h)
#ifndef WINLITE_H
#define WINLITE_H

#ifdef _WIN32
#define WINAPI __stdcall

typedef union _LARGE_INTEGER {
    long long QuadPart;
} LARGE_INTEGER;

/* Function declarations */
int WINAPI QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);
int WINAPI QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);
void *WINAPI VirtualAlloc(void *lpAddress, unsigned long dwSize, unsigned long flAllocationType, unsigned long flProtect);
void *WINAPI CreateThread(void *lpThreadAttributes, unsigned long dwStackSize, unsigned (__stdcall *lpStartAddress)(void *), void *lpParameter, unsigned long dwCreationFlags, unsigned long *lpThreadId);
unsigned long WINAPI WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);

#endif // _WIN32
#endif // WINLITE_H