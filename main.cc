#include <iostream>

#include "include/matrix.h"

int main(){

    int data[] = {1, 2, 3, 4, 5, 6, 9, 8, 7};
    int data2[] = {-1, -2, -3, -4, -5, -6, 7, 8, 9};
    M::Matrix<int> A(3, 3, data);
    M::Matrix<int> B(3, 3, data2);
    
    A *= B;
    A.print();

    return 0;
}