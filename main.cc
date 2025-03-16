#include <iostream>

#include "include/matrix.h"
#include "include/stat.h"

int main(){
    int data[] = { 1, 2, 3, 4 };
    M::Matrix<int> A = { 
        {1, 2, 3},
        {5, 6, 4},
        {6, 7, 7}  
    };
    
    M::Matrix<int> B = {
        {1, 2, 1},
        {3, 6, 5},
        {6, 7, 7}       
    };
    
    ExecutionTimer timer;
    M::Matrix<int> C = A * B;
    double time = timer.stop();
    printf("%f", time);

    return 0;
}