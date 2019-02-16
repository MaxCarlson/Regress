#pragma once
#include <vector>

class MatrixBase
{
};

template<class T>
class Matrix
{
	int nrows;
	int ncolumns;
	std::vector<T> vals;

public:
	Matrix() = default;
	Matrix(int nrows, int ncolumns);

	void resize(int nrows, int ncolumns);

	int rows()		const { return nrows; }
	int columns()	const { return ncolumns; }
	T sum() const;
	Matrix<T> transpose() const;
	Matrix<T> cwiseProduct(const Matrix<T>& m) const;

	Matrix<T>& operator=(const Matrix<T>& other);

	T& operator()(int row, int col);
	const T& operator()(int row, int col) const;

	Matrix operator+(const Matrix& m2) const;

	Matrix operator-(const Matrix& m2) const;
	Matrix operator-() const;
	template<class Num, class T>
	friend Matrix operator-(const Num& n, const Matrix& m);

	Matrix operator*(const Matrix& m2) const;

	template<class Num>
	Matrix& operator*=(const Num& num);
	Matrix& operator*=(const Matrix& m2);


	template<class Num>
	Matrix operator*(const Num& num) const;

	template<class Num>
	friend Matrix operator*(const Num& num, const Matrix<T>& matrix);

	template<class Func>
	void unaryExpr(Func&& func);
};

template<class T>
inline Matrix<T>::Matrix(int nrows, int ncolumns) :
	nrows{		nrows			},
	ncolumns{	ncolumns		},
	vals(		nrows * ncolumns)
{
}

template<class T>
inline void Matrix<T>::resize(int rows, int columns)
{
	nrows		= rows;
	ncolumns	= columns;
	vals.resize(nrows * ncolumns);
}

// TODO: Parrallelize
template<class T>
inline T Matrix<T>::sum() const
{
	T total = 0;
	for (const auto& i : vals)
		total += i;
	return total;
}

template<class T>
inline Matrix<T> Matrix<T>::transpose() const
{
	Matrix<T> m(ncolumns, nrows);

	#pragma omp parallel for
	for (int n = 0; n < nrows*ncolumns; n++) {
		int i = n % nrows;
		int j = n / nrows;
		m.vals[n] = vals[ncolumns*i + j];
	}

	return m;
}

template<class T>
inline Matrix<T> Matrix<T>::cwiseProduct(const Matrix<T>& m) const
{
	Matrix<T> m2(nrows, ncolumns);

	if (nrows != m.nrows
		|| ncolumns != m.ncolumns)
		throw std::runtime_error("Matrices must match!");

	for (int i = 0; i < nrows * ncolumns; ++i)
		m2.vals[i] = vals[i] * m.vals[i];

	return m2;
}

template<class T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other)
{
	nrows		= other.nrows;
	ncolumns	= other.ncolumns;
	vals.resize(other.vals.size());
	std::copy(std::begin(other.vals), std::end(other.vals), std::begin(vals));
	return *this;
}

template<class T>
inline T & Matrix<T>::operator()(int row, int col) 
{
	return vals[row * ncolumns + col];
}

template<class T>
inline const T & Matrix<T>::operator()(int row, int col) const
{
	return vals[row * ncolumns + col];
}

template<class T>
inline Matrix<T> Matrix<T>::operator+(const Matrix & m2) const
{
	Matrix<T> m{ nrows, ncolumns };
	if (ncolumns != m2.ncolumns
		|| nrows != m2.nrows)
		throw std::runtime_error("Matrix 1&2 must be identically sized!");

	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < ncolumns; ++j)
			m(i, j) = this->operator()(i, j) + m2(i, j);

	return m;
}

template<class T>
inline Matrix<T> Matrix<T>::operator-(const Matrix & m2) const
{
	Matrix<T> m{ nrows, ncolumns };
	if(ncolumns != m2.ncolumns
		|| nrows != m2.nrows)
		throw std::runtime_error("Matrix 1&2 must be identically sized!"); // TODO: Debug assertions not if statments

	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < ncolumns; ++j)
			m(i, j) = this->operator()(i, j) - m2(i, j);

	return m;
}

template<class T>
inline Matrix<T> Matrix<T>::operator-() const
{
	Matrix<T> m{ nrows, ncolumns };

	for (int i = 0; i < nrows * ncolumns; ++i)
		m.vals[i] = -vals[i];
	return m;
}

template<class Num, class T>
inline Matrix<T> operator-(const Num & n, const Matrix<T> & m)
{
	Matrix<T> m1{ m.nrows, m.ncolumns };

	for (int i = 0; i < m.nrows * m.ncolumns; ++i)
		m1.vals[i] = n - m.vals[i];
	return m1;
}

template<class T>
inline Matrix<T> Matrix<T>::operator*(const Matrix & m2) const
{
	Matrix<T> m{ nrows, m2.ncolumns };

	if (ncolumns != m2.nrows)
		throw std::runtime_error("Matrix1 columns != Matrix2 rows!");
	
	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < m2.ncolumns; ++j)
		{
			T sum = 0;
			for (int k = 0; k < ncolumns; ++k)
				sum += this->operator()(i, k) * m2(k, j);
			m(i, j) = sum;
		}
	
	return m;
}

template<class T>
inline Matrix<T> & Matrix<T>::operator*=(const Matrix<T> & m2)
{
	*this = *this * m2;
	return *this;
}



template<class T>
template<class Num>
inline Matrix<T> & Matrix<T>::operator*=(const Num & num)
{
	*this = *this * num;
	return *this;
}

// TODO: Parallelize
template<class T>
template<class Num>
inline Matrix<T> Matrix<T>::operator*(const Num & num) const
{
	Matrix<T> m;
	m.resize(nrows, ncolumns);
	for (int i = 0; i < vals.size(); ++i)
		m.vals[i] = num * vals[i];
	return m;
}

template<class T>
template<class Func>
inline void Matrix<T>::unaryExpr(Func && func)
{
	// TODO: Add OpenMp
	for (auto& e : vals)
		func(e);
}

template<class Num, class T>
inline Matrix<T> operator*(const Num & num, const Matrix<T>& matrix)
{
	return matrix * num;
}


