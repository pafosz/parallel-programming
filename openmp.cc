#include <iostream>
#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <direct.h>
#include <stdexcept>
#include <iomanip>

#include "include/matrix.h"
#include "stat.h"

constexpr auto MIN_VALUE = 0;
constexpr auto MAX_VALUE = 100;

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

void create_directory(const std::string& dir_name) {
#ifdef _WIN32
    _mkdir(dir_name.c_str());
#else
    mkdir(dir_name.c_str(), 0777);
#endif
}

bool change_directory(const std::string& path) {
#ifdef _WIN32
    return _chdir(path.c_str()) == 0;
#else
    return chdir(path.c_str()) == 0;
#endif
}

void test_threads_performance(const M::Matrix<int>& A, const M::Matrix<int>& B, const std::string& dir_name) {
    std::vector<int> thread_counts;
    int max_threads = omp_get_max_threads();

    for (int threads = 1; threads <= max_threads; threads *= 2) {
        thread_counts.push_back(threads);
    }
    if (thread_counts.back() != max_threads) {
        thread_counts.push_back(max_threads);
    }

    std::vector<double> thread_times(thread_counts.size());

    for (size_t i = 0; i < thread_counts.size(); i++) {
        int threads = thread_counts[i];
        ExecutionTimer timer;
        M::Matrix<int> result = matrix_multiply_omp(A, B, threads);
        timer.stop();
        thread_times[i] = timer.get_duration();
        std::cout << "Size " << A.get_rows() << "x" << A.get_cols()
                  << ", Threads: " << threads
                  << ", Time: " << thread_times[i] << " ms" << std::endl;
    }

    std::ofstream thread_file(dir_name + "/threads.txt");
    if (!thread_file.is_open()) {
        throw std::runtime_error("Couldn't open threads.txt for writing");
    }

    thread_file << "Threads\tTime(ms)\n";
    for (size_t i = 0; i < thread_counts.size(); i++) {
        thread_file << thread_counts[i] << "\t" << thread_times[i] << "\n";
    }
    thread_file.close();
}

template<typename T>
void write_matrix(const std::string& filename,
                 const M::Matrix<T>& matrix,
                 std::ios_base::openmode mode = std::ios::out) {
    std::ofstream file(filename, mode);
    if (!file) throw std::runtime_error("File error");

    file << matrix.get_rows() << " " << matrix.get_cols() << "\n";
    for (int i = 0; i < matrix.get_rows(); ++i) {
        for (int j = 0; j < matrix.get_cols(); ++j) {
            file << matrix(i, j) << " ";
        }
        file << "\n";
    }
}

void write_csv_results(const std::vector<int>& sizes,
                      const std::vector<int>& thread_counts,
                      const std::vector<std::vector<double>>& results) {
    std::ofstream file("statistic.csv");
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open statistic.csv for writing");
    }

    file << "Threads\\Sizes";
    for (int size : sizes) {
        file << "," << size;
    }
    file << "\n";

    for (size_t i = 0; i < thread_counts.size(); i++) {
        file << thread_counts[i];
        for (size_t j = 0; j < sizes.size(); j++) {
            file << "," << std::fixed << std::setprecision(4) << results[i][j];
        }
        file << "\n";
    }
}

int main() {
    std::vector<int> SIZES = {100, 200, 300, 400, 500, 1000, 2000};
    std::vector<int> THREAD_COUNTS;

    for (int threads = 1; threads <= omp_get_max_threads(); threads *= 2) {
        THREAD_COUNTS.push_back(threads);
    }
    if (THREAD_COUNTS.back() != omp_get_max_threads()) {
        THREAD_COUNTS.push_back(omp_get_max_threads());
    }

    std::vector<std::vector<double>> results(THREAD_COUNTS.size(),
                                          std::vector<double>(SIZES.size()));

    if (!change_directory("C:\\Users\\user\\Desktop\\ALL\\University\\3 cours\\6 semester\\PP\\Labs")) {
        std::cerr << "Failed to change directory!" << std::endl;
        return 1;
    }

    std::string dir_result = "result_for_lab2";
    create_directory(dir_result);
    if (!change_directory(dir_result)) {
        std::cerr << "Failed to enter results directory!" << std::endl;
        return 1;
    }

    for (size_t size_idx = 0; size_idx < SIZES.size(); size_idx++) {
        int current_size = SIZES[size_idx];
        std::cout << "Processing size: " << current_size << "x" << current_size << std::endl;

        M::Matrix<int> A(current_size, current_size);
        M::Matrix<int> B(current_size, current_size);
        A.fill_random(MIN_VALUE, MAX_VALUE);
        B.fill_random(MIN_VALUE, MAX_VALUE);

        for (size_t thread_idx = 0; thread_idx < THREAD_COUNTS.size(); thread_idx++) {
            int threads = THREAD_COUNTS[thread_idx];

            ExecutionTimer timer;
            M::Matrix<int> result = matrix_multiply_omp(A, B, threads);
            timer.stop();

            double time = timer.get_duration();
            results[thread_idx][size_idx] = time;

            std::cout << "  Threads: " << threads << " Time: " << time << " ms" << std::endl;
        }

        std::string dir_name = std::to_string(current_size);
        create_directory(dir_name);
        if (!change_directory(dir_name)) {
            std::cerr << "Failed to enter size directory!" << std::endl;
            continue;
        }

        // Последовательная запись файлов
        try {
            write_matrix("A.txt", A);
            write_matrix("B.txt", B);
            write_matrix("result.txt", matrix_multiply_omp(A, B, omp_get_max_threads()));
        } catch (const std::exception& e) {
            std::cerr << "Error writing files: " << e.what() << std::endl;
        }

        change_directory("..");
    }

    write_csv_results(SIZES, THREAD_COUNTS, results);

    return 0;
}