
#ifndef LINCE_BENCHMARK_H
#define LINCE_BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>


#define BENCHMARK_LOOP(COUNTER_TYPE, COUNTER, ITER_NUM) { clock_t _##COUNTER##_timer = clock(); COUNTER_TYPE COUNTER; for (COUNTER = 0; COUNTER != ITER_NUM ; ++COUNTER)

#define BENCHMARK_END(COUNTER_TYPE, COUNTER, ITER_NUM) _##COUNTER##_timer = clock() - _##COUNTER##_timer; printf("Iterations: %-10ld Time (ms): %-10ld\n", (long)ITER_NUM, (long)_##COUNTER##_timer); }


#endif /* LINCE_BENCHMARK_H */