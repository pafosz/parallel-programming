#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <filesystem>
#include <string>
#include <mpi.h>

#include "../include/matrix.h"

static void create_directory(const std::string& dir_name) {
    try {
        if (std::filesystem::create_directories(dir_name)) {
            std::cout << "Directory " << dir_name << " created.\n";
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

    static void chdir(const std::string& dir_name) {
    try {
        std::filesystem::current_path(dir_name);
        std::cout << "The current directory has been changed to: " << std::filesystem::current_path() << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        if (!std::filesystem::exists(dir_name)) {
            std::cerr << "Directory does not exist. Creating: " << dir_name << std::endl;
            create_directory(dir_name);
            try {
                std::filesystem::current_path(dir_name);
                std::cout << "The current directory has been changed to: " << std::filesystem::current_path()
                << std::endl;
            }
            catch (const std::filesystem::filesystem_error& e2) {
                std::cerr << "Failed to change directory even after creation: " << e2.what() << std::endl;
            }
        } else {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

template <typename T>
M::Matrix<T> matrix_multiply_mpi(const M::Matrix<T>& A, const M::Matrix<T>& B, int num_procs) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const size_t rows = A.get_rows();
    const size_t cols = B.get_cols();
    const size_t inner_dim = A.get_cols();

    M::Matrix<T> result(rows, cols);

    // Распределение работы между процессами
    const size_t rows_per_process = rows / num_procs;
    const size_t remainder = rows % num_procs;

    size_t start_row = rank * rows_per_process + std::min(rank, static_cast<int>(remainder));
    size_t end_row = start_row + rows_per_process + (rank < remainder ? 1 : 0);

    // Каждый процесс вычисляет свою часть матрицы
    M::Matrix<T> local_result(end_row - start_row, cols);

    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            T sum{};
            for (size_t k = 0; k < inner_dim; ++k) {
                sum += A(i, k) * B(k, j);
            }
            local_result(i - start_row, j) = sum;
        }
    }

    // Собираем результаты на root процессе (ранг 0)
    if (rank == 0) {
        for (size_t i = start_row; i < end_row; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = local_result(i - start_row, j);
            }
        }

        for (int src = 1; src < num_procs; ++src) {
            const size_t src_start = src * rows_per_process + std::min(src,
                static_cast<int>(remainder));
            const size_t src_end = src_start + rows_per_process + (src < remainder ? 1 : 0);
            const size_t src_rows = src_end - src_start;

            std::vector<T> recv_buffer(src_rows * cols);
            MPI_Recv(recv_buffer.data(), src_rows * cols, MPI_INT, src, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (size_t i = 0; i < src_rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    result(src_start + i, j) = recv_buffer[i * cols + j];
                }
            }
        }
    } else {
        std::vector<T> send_buffer(local_result.get_rows() * local_result.get_cols());
        for (size_t i = 0; i < local_result.get_rows(); ++i) {
            for (size_t j = 0; j < local_result.get_cols(); ++j) {
                send_buffer[i * local_result.get_cols() + j] = local_result(i, j);
            }
        }
        MPI_Send(send_buffer.data(), send_buffer.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return result;
}

void write_csv_results(const std::vector<int>& sizes,
                      const std::vector<int>& proc_counts,
                      const std::vector<std::vector<double>>& results) {
    std::ofstream file("statistic.csv");
    if (!file.is_open()) {
        std::cerr << "Couldn't open statistic.csv for writing" << std::endl;
        return;
    }

    file << "Processes\\Sizes";
    for (int size : sizes) {
        file << "," << size;
    }
    file << "\n";

    for (size_t i = 0; i < proc_counts.size(); i++) {
        file << proc_counts[i];
        for (size_t j = 0; j < sizes.size(); j++) {
            file << "," << std::fixed << std::setprecision(4) << results[i][j];
        }
        file << "\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const std::vector<int> SIZES = {100, 200, 300, 400, 500, 1000, 2000};
    std::vector<int> PROC_COUNTS;

    // Определяем количество процессов для тестирования (1, 2, 4, ..., max)
    for (int procs = 1; procs <= world_size; procs *= 2) {
        PROC_COUNTS.push_back(procs);
    }
    if (PROC_COUNTS.back() != world_size) {
        PROC_COUNTS.push_back(world_size);
    }

    std::vector<std::vector<double>> results(PROC_COUNTS.size(),
                                          std::vector<double>(SIZES.size()));

    if (rank == 0) {
        chdir(R"(C:\Users\user\Desktop\ALL\University\3 cours\6 semester\PP\Labs\Lab3)");
        const std::string dir_result = "result";
        create_directory(dir_result);
        chdir(dir_result);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (size_t size_idx = 0; size_idx < SIZES.size(); size_idx++) {
        int current_size = SIZES[size_idx];

        M::Matrix<int> A{}, B{};
        if (rank == 0) {
            std::cout << "Processing size: " << current_size << "x" << current_size << std::endl;
            A = M::Matrix<int>(current_size, current_size);
            B = M::Matrix<int>(current_size, current_size);
            A.fill_random(0, 100);
            B.fill_random(0, 100);
            std::string dir_name = std::to_string(SIZES[size_idx]);
            chdir(dir_name);
            A.write_to_file("A.txt");
            B.write_to_file("B.txt");
        }

        // Рассылаем размер матрицы всем процессам
        MPI_Bcast(&current_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Выделяем память и рассылаем данные матриц
        if (rank != 0) {
            A = M::Matrix<int>(current_size, current_size);
            B = M::Matrix<int>(current_size, current_size);
        }

        MPI_Bcast(A.get_data(), current_size * current_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(B.get_data(), current_size * current_size, MPI_INT, 0, MPI_COMM_WORLD);
        M::Matrix<int> result(current_size, current_size);
        for (size_t proc_idx = 0; proc_idx < PROC_COUNTS.size(); proc_idx++) {
            int num_procs = PROC_COUNTS[proc_idx];

            MPI_Barrier(MPI_COMM_WORLD);
            double start_time = MPI_Wtime();

            if (rank < num_procs) {
                result = matrix_multiply_mpi(A, B, num_procs);
            }

            MPI_Barrier(MPI_COMM_WORLD);
            double end_time = MPI_Wtime();

            if (rank == 0) {
                double time = (end_time - start_time);
                results[proc_idx][size_idx] = time;
                std::cout << "  Processes: " << num_procs << " Time: " << time << " ms" << std::endl;
            }
        }
        result.write_to_file("result.txt");
        chdir("..");
    }


    if (rank == 0) {
        write_csv_results(SIZES, PROC_COUNTS, results);
    }

    MPI_Finalize();
    return 0;
}