#include <vector>

template<class T>
class Matrix
{
	int nrows;
	int ncolumns;
	std::vector<T> vals;

public:
	Matrix(int nrows, int ncolumns);

	int rows()		const { return nrows; }
	int columns()	const { return ncolumns; }

	T& operator()(int row, int col);
	const T& operator()(int row, int col) const;


	Matrix<T> operator*(const Matrix& m2);

	friend Matrix operator+(const Matrix& m1, const Matrix& m2);
	friend Matrix operator-(const Matrix& m1, const Matrix& m2);
	//friend Matrix operator*(const Matrix& m1, const Matrix& m2);

	friend void matrixMul(Matrix& m, const Matrix& m1, const Matrix& m2);
};

template<class T>
inline Matrix<T>::Matrix(int nrows, int ncolumns) :
	nrows{		nrows			},
	ncolumns{	ncolumns		},
	vals(		nrows * ncolumns)
{
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
inline Matrix<T> operator+(const Matrix<T> & m1, const Matrix<T> & m2)
{
	Matrix<T> m{ m1.rows, m1.columns };

	return m;
}

template<class T>
inline Matrix<T> operator-(const Matrix<T> & m1, const Matrix<T> & m2)
{
	Matrix<T> m{ m1.rows, m1.columns };

	return m;
}

template<class T>
inline void matrixMul(Matrix<T> & m, const Matrix<T> & m1, const Matrix<T> & m2)
{
	for (int i = 0; i < m1.rows; ++i)
		for (int j = 0; j < m2.columns; ++j)
		{
			int sum = 0;
			for (int k = 0; k < m1.columns; ++k)
				sum += m1(i, k) + m2(k, j);
			m(i, j) = sum;
		}
}

template<class T>
inline Matrix<T> Matrix<T>::operator*(const Matrix & m2)
{
	Matrix<T> m{ nrows, m2.ncolumns };
	//matrixMul(m, *this, m2);
	
	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < m2.ncolumns; ++j)
		{
			int sum = 0;
			for (int k = 0; k < ncolumns; ++k)
				sum += this->operator()(i, k) * m2(k, j);
			m(i, j) = sum;
		}
	
	return m;
}

/*
template<class T>
inline Matrix<T> operator*(const Matrix<T> & m1, const Matrix<T> & m2)
{
	Matrix<T> m{ m1.rows, m2.columns };
	matrixMul(m, m1, m2);
	return m;
}
*/