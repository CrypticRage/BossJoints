#include "Debug.h"

void BossJoints::XTrace0(LPCTSTR lpszText)
{
    ::OutputDebugString(lpszText);
}

void BossJoints::XTrace(LPCTSTR lpszFormat, ...)
{
    va_list args;
    va_start(args, lpszFormat);
    int nBuf;
    TCHAR szBuffer[512]; // get rid of this hard-coded buffer
    nBuf = _vsnwprintf_s(szBuffer, 511, lpszFormat, args);
    ::OutputDebugString(szBuffer);
    va_end(args);
}
