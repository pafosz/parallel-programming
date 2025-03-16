#include "../include/stat.h"

ExecutionTimer::ExecutionTimer() :
    end{},
    duration{}
{
    start = std::chrono::high_resolution_clock::now();
}

double ExecutionTimer::stop() {
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    return duration.count();
}