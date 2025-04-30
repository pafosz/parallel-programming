#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

class Matrix {
private:
    std::vector<int> data;
    size_t rows, cols;

public:
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        data.resize(rows * cols);
    }

    void fill_random(int min_val, int max_val) {
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = min_val + rand() % (max_val - min_val + 1);
        }
    }

    int& operator()(size_t i, size_t j) { return data[i * cols + j]; }
    const int& operator()(size_t i, size_t j) const { return data[i * cols + j]; }

    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }
    int* get_data() { return data.data(); }

    void print_part() const {
        const size_t print_size = std::min(size_t(5), rows);
        for (size_t i = 0; i < print_size; ++i) {
            for (size_t j = 0; j < print_size; ++j) {
                std::cout << data[i * cols + j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

void matrix_multiply_mpi(const Matrix& A, const Matrix& B, Matrix& C, int rank, int num_procs) {
    const size_t rows = A.get_rows();
    const size_t cols = B.get_cols();
    const size_t inner_dim = A.get_cols();

    const size_t rows_per_proc = rows / num_procs;
    const size_t remainder = rows % num_procs;

    size_t start_row = rank * rows_per_proc + std::min<size_t>(rank, remainder);
    size_t end_row = start_row + rows_per_proc + (rank < static_cast<int>(remainder) ? 1 : 0);

    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            int sum = 0;
            for (size_t k = 0; k < inner_dim; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }

    if (rank == 0) {
        for (int src = 1; src < num_procs; ++src) {
            size_t src_start = src * rows_per_proc + std::min<size_t>(src, remainder);
            size_t src_end = src_start + rows_per_proc + (src < static_cast<int>(remainder) ? 1 : 0);
            size_t src_rows = src_end - src_start;

            MPI_Recv(&C(src_start, 0), src_rows * cols, MPI_INT, src, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        MPI_Send(&C(start_row, 0), (end_row - start_row) * cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    const std::vector<size_t> SIZES = {100, 200, 300, 400, 500, 1000, 2000};
    const int MIN_VAL = 0;
    const int MAX_VAL = 1000;

    if (rank == 0) {
        std::cout << "Running with " << num_procs << " processes" << std::endl;
    }

    for (size_t size : SIZES) {
        Matrix A(size, size);
        Matrix B(size, size);
        Matrix C(size, size);

        if (rank == 0) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            A.fill_random(MIN_VAL, MAX_VAL);
            B.fill_random(MIN_VAL, MAX_VAL);
        }

        MPI_Bcast(A.get_data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(B.get_data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);

        double start_time = MPI_Wtime();
        matrix_multiply_mpi(A, B, C, rank, num_procs);
        double end_time = MPI_Wtime();

        if (rank == 0) {
            double time_ms = (end_time - start_time);
            std::cout << "Size " << size << "x" << size
                      << " Time: " << time_ms << "s" << std::endl;

            if (size <= 5) {
                std::cout << "Matrix A (first 5x5):" << std::endl;
                A.print_part();
                std::cout << "Matrix B (first 5x5):" << std::endl;
                B.print_part();
                std::cout << "Result (first 5x5):" << std::endl;
                C.print_part();
            }
        }
    }

    MPI_Finalize();
    return 0;
}