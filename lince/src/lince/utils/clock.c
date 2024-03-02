#include "lince/utils/clock.h"
#include <GLFW/glfw3.h>


/** @brief Create a new clock. Starts ticking immediately. */
LinceClock LinceNewClock(){
    return (LinceClock){.start = glfwGetTime()};
}

/** @brief Return the number of seconds since the clock began */
double LinceReadClock(LinceClock clock){
    return glfwGetTime() - clock.start;
}

/** @brief Resets the clock counter to zero */
void LinceResetClock(LinceClock clock){
    clock = LinceNewClock();
}