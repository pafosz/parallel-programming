#include <iostream>
#include <omp.h>

#include "include/matrix.h"
#include "stat.h"

#include "matrix.h"
#include <omp.h>

// Статическая функция (видна только в этом файле)
template<typename T>
static M::Matrix<T> matrix_multiply_omp(const M::Matrix<T>& lhs, const M::Matrix<T>& rhs, int threads) {
    if (lhs.get_cols() != rhs.get_rows()) {
        throw std::invalid_argument{"Matrix dimensions mismatch"};
    }

    M::Matrix<T> result(lhs.get_rows(), rhs.get_cols());


#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < lhs.get_rows(); ++i) {
        for (int j = 0; j < rhs.get_cols(); ++j) {
            T sum{};
            for (size_t k = 0; k < lhs.get_cols(); ++k) {
                sum += lhs(i, k) * rhs(k, j);
            }
            result(i, j) = sum;
        }
    }

    return result;
}

int main(){

    M::Matrix<int> A(1000, 1000);
    M::Matrix<int> B(1000, 1000);
    A.fill_random(1, 100);
    B.fill_random(1, 100);

    for (int threads = 1; threads <= omp_get_max_threads(); threads *= 2) {
        std::cout << "\nThreads: " << threads << " - ";
        ExecutionTimer timer2;
        matrix_multiply_omp(A,B,threads);
        timer2.stop();
        std::cout << "\n";
    }

    return 0;
}