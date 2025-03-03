#include <iostream>

#include "include/matrix.h"

int main(){
    int data[] = { 1, 2, 3, 4 };
    M::Matrix<int> A = { 
        {1, 2, 3, 4, 10},
        {5, 6, 7, 8, 9}
    };
    A.print();
    M::Matrix<int> B={
        {1, 2, 3, 4}, 
        {5, 6, 7, 8}
    };
    std::cout << A.get_cols() << "x" << A.get_rows() << std::endl;
    // M::Matrix<int> C = A - B;
    // C.print();
    return 0;
}