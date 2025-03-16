#include <iostream>

#include "include/matrix.h"
#include "include/stat.h"
#include "include/random_generator.h"

int main(){
    M::Matrix<int> A(1000, 1000);
    A.fill_random(0, 1000);
    M::Matrix<int> B(1000, 1000);
    B.fill_random(0, 1000);

    ExecutionTimer timer;
    A *= B;
    timer.stop();
    double time = timer.get_duration();
    std::cout << time;
    

    return 0;
}