#pragma once
#include <vector>

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

	T& operator()(int row, int col);
	const T& operator()(int row, int col) const;

	Matrix operator+(const Matrix& m2);
	Matrix operator-(const Matrix& m2);
	Matrix operator*(const Matrix& m2);

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
inline void Matrix<T>::resize(int nrows, int ncolumns)
{
	vals.resize(nrows * ncolumns);
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
inline Matrix<T> Matrix<T>::operator+(const Matrix & m2)
{
	Matrix<T> m{ nrows, ncolumns };

	return m;
}

template<class T>
inline Matrix<T> Matrix<T>::operator-(const Matrix & m2)
{
	Matrix<T> m{ nrows, ncolumns };

	return m;
}

template<class T>
inline Matrix<T> Matrix<T>::operator*(const Matrix & m2)
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
template<class Func>
inline void Matrix<T>::unaryExpr(Func && func)
{
	// TODO: Add OpenMp
	for (auto& e : vals)
		func(e);
}
