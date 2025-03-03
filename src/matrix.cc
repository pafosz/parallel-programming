#include "../include/matrix.h"

template <typename T>
M::Matrix<T>::Matrix(size_t rows, size_t cols);

template <typename T>
M::Matrix<T>::Matrix(size_t rows, size_t cols, const T *data);

template <typename T>
M::Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> list);

template <typename T>
size_t M::Matrix<T>::get_rows() const noexcept;

template <typename T>
size_t M::Matrix<T>::get_cols() const noexcept;

template <typename T>
const T& M::Matrix<T>::operator()(size_t row, size_t column) const;

template <typename T>
T& M::Matrix<T>::operator()(size_t row, size_t column);

template <typename T>
M::Matrix<T>& M::Matrix<T>::operator+=(const M::Matrix<T> &rhs);

template <typename T>
M::Matrix<T>& M::Matrix<T>::operator-=(const Matrix<T> &rhs);

template <typename T>
M::Matrix<T>& M::Matrix<T>::operator*=(Matrix<T> rhs);

template <typename T>
void M::Matrix<T>::print() const;