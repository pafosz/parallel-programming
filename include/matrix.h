#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>

namespace M
{
	template <typename T = float>
	class Matrix
	{
		public:
		Matrix(size_t rows, size_t cols); // все значения нулями
		Matrix(size_t rows, size_t cols, const T *data);
		Matrix(std::initializer_list<std::initializer_list<T>> list);

		size_t get_rows() const noexcept;
		size_t get_cols() const noexcept;

		const T& operator()(size_t row, size_t column) const;
		T& operator()(size_t row, size_t column);

		Matrix<T>& operator+=(const Matrix<T>& rhs);	

		Matrix<T>& operator-=(const Matrix<T>& rhs);		
		
		Matrix<T>& operator*=(Matrix<T> rhs);		
		
		void print() const;

	private:
		size_t _rows, _cols;
		std::vector<T> _data;
	};
}
template <typename T>
M::Matrix<T>::Matrix(size_t rows, size_t cols) : 
	_rows{rows},
	_cols{cols},
	_data(rows * cols, 0)
{ }

template <typename T>
M::Matrix<T>::Matrix(size_t rows, size_t cols, const T *data) : 
	_rows{rows},
	_cols{cols},
	_data(data, data + rows * cols)
{ }

template <typename T>
M::Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> list) : 
				Matrix{static_cast<size_t>(list.size()),
				static_cast<size_t>(list.size() ? list.begin()->size() : 0)}
{
	std::for_each(list.begin(), list.end(),
			[this, i{0}](auto &row) mutable
			{
				std::copy(row.begin(), row.end(), _data.begin() + i++ * _cols);
			}
		);
}

template <typename T>
size_t M::Matrix<T>::get_rows() const noexcept
{
	return _rows;
}

template <typename T>
size_t M::Matrix<T>::get_cols() const noexcept
{
	return _cols;
}

template <typename T>
const T& M::Matrix<T>::operator()(size_t row, size_t column) const
{
	return _data[row * _cols + column];
}

template <typename T>
T& M::Matrix<T>::operator()(size_t row, size_t column)
{
	return _data[row * _cols + column];
}

template<typename T>
M::Matrix<T>& M::Matrix<T>::operator+=(const Matrix<T>& rhs) 
{
	if(this->_cols != rhs._cols || this->_rows != rhs._rows){
		throw std::invalid_argument{"Failed to sum matrices"};
	}
	std::transform(_data.begin(), _data.end(),
		rhs._data.begin(),
		_data.begin(),
		std::plus<T>{}
	);
	return *this;
}

template<typename T>
M::Matrix<T> operator+(M::Matrix<T> lhs, const M::Matrix<T>& rhs) 
{
    lhs += rhs;
    return lhs;
}	

template<typename T>
M::Matrix<T>& M::Matrix<T>::operator-=(const Matrix<T>& rhs)
{
	if(_rows != rhs._rows || _cols != rhs._cols){
		throw std::invalid_argument{"Failed to subtruct matrices"};
	}
	std::transform(_data.begin(), _data.end(),
		rhs._data.begin(),
		_data.begin(),
		std::minus<T>{}
	);
	return *this;
}

template<typename T>
M::Matrix<T> operator-(M::Matrix<T> lhs, const M::Matrix<T>& rhs) 
{ 
    lhs -= rhs;  
    return lhs;
}

template<typename T>
M::Matrix<T>& M::Matrix<T>::operator*=(Matrix<T> rhs)
{
	if(_cols != rhs._rows){
		throw std::invalid_argument{"Failed to multyply matrices"};
	}
	Matrix<T> result(_rows, rhs._cols);
	for(size_t i = 0; i < _cols; ++i)
	{
		for(size_t j = 0; j < rhs._rows; ++j)
		{
			T sum{};
			for(size_t k = 0; k < _cols; ++k)
			{
				sum += (*this)(i, k) * rhs(k, j);
			}
			result(i, j) = sum;
		}
	}
	*this = result;
	return *this;
}

template<typename T>
M::Matrix<T> operator*(M::Matrix<T> lhs, const M::Matrix<T>& rhs)
{
	lhs *= rhs;
	return lhs;
}

template<typename T>
void M::Matrix<T>::print() const 
{
	for(size_t i = 0; i < _rows; i++){
		for(size_t j = 0; j < _cols; j++){
			std::cout << (*this)(i, j) << "\t";
		}
		std::cout << "\n";
	}
}

#endif // MATRIX_H