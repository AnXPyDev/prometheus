typedef clock_t HPTimer_Measurement;
typedef clock_t HPTimer_Duration;

static inline HPTimer_Measurement HPTimer_now() {
    return clock();
}

static inline HPTimer_Duration HPTimer_duration(HPTimer_Measurement start, HPTimer_Measurement end) {
    return end - start;
}

static inline HPTimer_Duration HPTimer_elapsed(HPTimer_Measurement start) {
    return HPTimer_duration(start, HPTimer_now());
}

static inline HPTimer_Seconds HPTimer_seconds(HPTimer_Duration duration) {
    return ((double)duration) / ((double)CLOCKS_PER_SEC);
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
