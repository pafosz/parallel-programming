#ifndef STAT_H
#define STAT_H

#include <iostream>
#include <chrono>
#include <vector>

class ExecutionTimer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::chrono::duration<double> duration;
public:
    ExecutionTimer();
    double stop();
};

#endif //STAT_H