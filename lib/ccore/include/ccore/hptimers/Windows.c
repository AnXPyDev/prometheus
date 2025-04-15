const LARGE_INTEGER large_integer;

typedef LONGLONG HPTimer_Measurement;
typedef LONGLONG HPTimer_Duration;

static inline HPTimer_Measurement HPTimer_now(void) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart;
}

static inline HPTimer_Duration HPTimer_duration(HPTimer_Measurement start, HPTimer_Measurement end) {
    return end - start;
}

static inline HPTimer_Duration HPTimer_elapsed(HPTimer_Measurement start) {
    return HPTimer_duration(start, HPTimer_now());
}

static inline HPTimer_Seconds HPTimer_seconds(HPTimer_Duration duration) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return ((double)duration) / ((double)freq.QuadPart);
}

static inline HPTimer_Millis HPTimer_millis(HPTimer_Duration duration) {
    return HPTimer_seconds(duration) * (double)(1000.0);
}

static inline HPTimer_Micros HPTimer_micros(HPTimer_Duration duration) {
    return HPTimer_millis(duration) * (double)(1000.0);
}

static inline HPTimer_Nanos HPTimer_nanos(HPTimer_Duration duration) {
    return HPTimer_micros(duration) * (double)(1000.0);
}

static inline HPTimer_Picos HPTimer_picos(HPTimer_Duration duration) {
    return HPTimer_nanos(duration) * (double)(1000.0);
}
