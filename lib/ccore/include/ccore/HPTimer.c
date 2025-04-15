typedef double HPTimer_Seconds;
typedef double HPTimer_Millis;
typedef double HPTimer_Micros;
typedef double HPTimer_Nanos;
typedef double HPTimer_Picos;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hptimers/Windows.c"
#else
#include <time.h>
#include "hptimers/Generic.c"
#endif
