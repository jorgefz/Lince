#ifndef LINCE_CLOCK_H
#define LINCE_CLOCK_H

typedef struct LinceClock {
    double start;
} LinceClock;

/** @brief Create a new clock. Starts ticking immediately. */
LinceClock LinceNewClock();

/** @brief Return the number of seconds since the clock began */
double LinceReadClock(LinceClock clock);

/** @brief Resets the clock counter to zero */
void LinceResetClock(LinceClock clock);

#endif /* LINCE_CLOCK_H */