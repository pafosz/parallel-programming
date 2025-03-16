#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <vector>
#include <random>

class RandomGenerator {
public:
    template <typename T>
    static std::vector<T> generate_matrix(size_t rows, size_t cols, const T& minVal, const T& maxVal);
};

template <typename T>
std::vector<T> RandomGenerator::generate_matrix(size_t rows, size_t cols, const T& minVal, const T& maxVal) {
    std::vector<T> matrix(rows * cols);
    std::random_device rd;
    std::mt19937 gen(rd());

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(minVal, maxVal);
        for (auto& elem : matrix) {
            elem = dist(gen);
        }
    }
    else {
        std::uniform_real_distribution<T> dist(minVal, maxVal);
        for (auto& elem : matrix) {
            elem = dist(gen);
        }
    }

    return matrix;
}


#endif //RANDOM_GENERATOR_H