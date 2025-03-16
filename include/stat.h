#ifndef STAT_H
#define STAT_H

#include <iostream>
#include <chrono>
#include <vector>

class ExecutionTimer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _start, _end;
    std::chrono::duration<double> _duration;
public:
    ExecutionTimer();
    void stop();
    double get_duration() const;
};

#endif //STAT_H