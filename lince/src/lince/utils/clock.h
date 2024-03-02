#ifndef LINCE_CLOCK_H
#define LINCE_CLOCK_H

/** @struct Measures the number of seconds elapsed */
typedef struct LinceClock {
    double start; ///< Arbitrary timestamp of clock creation.
} LinceClock;

/** @brief Create a new clock. Starts ticking immediately. */
LinceClock LinceNewClock();

/** @brief Return the number of seconds since the clock began */
double LinceReadClock(LinceClock clock);

/** @brief Resets the clock counter to zero */
void LinceResetClock(LinceClock clock);

#endif /* LINCE_CLOCK_H */