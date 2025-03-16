#include "../include/stat.h"

ExecutionTimer::ExecutionTimer() :
    _end{},
    _duration{}
{
    _start = std::chrono::high_resolution_clock::now();
}

void ExecutionTimer::stop() {
    _end = std::chrono::high_resolution_clock::now();
    _duration = _end - _start;
    std::cout << "Duration of the process: " << _duration.count() << std::endl;
}

double ExecutionTimer::get_duration() const {
    return _duration.count();
}