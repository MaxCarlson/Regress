#pragma once
#include "Expr.h"


//
// Unary Expressions
//

// Transpose operators
template<class Type>
inline MatrixExpr<MatUnaExpr<
	typename Matrix<Type>::col_const_iterator,
	MatrixTransposeOp<Type>,
	Type>, Type>
	operator~(const Matrix<Type>& it) noexcept
{
	using ExprType = MatUnaExpr<
		typename Matrix<Type>::col_const_iterator,
		MatrixTransposeOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		it.ccol_begin(),
		it.rows(),
		it.cols() },
		MatrixOpBase::Op::TRANSPOSE };
}

/*
// TODO: NOT working!
template<class Type, class Iter>
inline MatrixExpr<MatUnaExpr<
	MatrixExpr<Iter, Type>,
	MatrixTransposeOp<Type>,
	Type>, Type>
	operator~(const MatrixExpr<Iter, Type>& it) noexcept
{
	using ExprType = MatUnaExpr<
		MatrixExpr<Iter, Type>,
		MatrixTransposeOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		it,
		it.rhsRows(),
		it.rhsCols() },
		MatrixOpBase::Op::TRANSPOSE };
}
*/

//
// Binary Expressions
//

// Addition operators
template<class Type>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	typename Matrix<Type>::const_iterator,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const Matrix<Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		typename Matrix<Type>::const_iterator,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::ADD };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename Matrix<Type>::const_iterator,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const MatrixExpr<Iter, Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename Matrix<Type>::const_iterator,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs.begin(),
		lhs.lhsRows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::ADD };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const Matrix<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs,
		lhs.rows(), 
		rhs.lhsRows(),		
		rhs.rhsCols() },	
		MatrixOpBase::Op::ADD };
}

template<class Type, class Iter1, class Iter2>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter1, Type>,
	MatrixExpr<Iter2, Type>,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const MatrixExpr<Iter1, Type>& lhs, const MatrixExpr<Iter2, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter1, Type>,
		MatrixExpr<Iter2, Type>,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs,
		lhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::ADD };
}

// Subtraction operators
template<class Type>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	typename Matrix<Type>::const_iterator,
	MatrixSubOp<Type>,
	Type>, Type>
	operator-(const Matrix<Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		typename Matrix<Type>::const_iterator,
		MatrixSubOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::SUB };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename Matrix<Type>::const_iterator,
	MatrixSubOp<Type>,
	Type>, Type>
	operator-(const MatrixExpr<Iter, Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename Matrix<Type>::const_iterator,
		MatrixSubOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs.begin(),
		lhs.lhsRows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::SUB };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixSubOp<Type>,
	Type>, Type>
	operator-(const Matrix<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixSubOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs,
		lhs.rows(),
		rhs.lhsRows(),		
		rhs.rhsCols() },	
		MatrixOpBase::Op::SUB };
}

template<class Type, class Iter1, class Iter2>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter1, Type>,
	MatrixExpr<Iter2, Type>,
	MatrixSubOp<Type>,
	Type>, Type>
	operator-(const MatrixExpr<Iter1, Type>& lhs, const MatrixExpr<Iter2, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter1, Type>,
		MatrixExpr<Iter2, Type>,
		MatrixSubOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs,
		lhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::SUB };
}

// Multiply operators
template<class Type>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	typename Matrix<Type>::const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const Matrix<Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		typename Matrix<Type>::const_iterator,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::MULTIPLY };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename Matrix<Type>::const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixExpr<Iter, Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename Matrix<Type>::const_iterator,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs.begin(),
		lhs.lhsRows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::MULTIPLY };
}

template<class Type, class Iter>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const Matrix<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs,
		lhs.rows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::MULTIPLY };
}

template<class Type, class Iter1, class Iter2>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter1, Type>,
	MatrixExpr<Iter2, Type>,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixExpr<Iter1, Type>& lhs, const MatrixExpr<Iter2, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter1, Type>,
		MatrixExpr<Iter2, Type>,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs,
		lhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::MULTIPLY };
}

//
// Binary Numerical Constant Expressions
//

// TODO: Use ?
//class = typename std::enable_if<std::is_arithmetic<Num>::value, Num>::type >

// Numerical addition operators
template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	NumericIteratorWrapper<Num>,
	MatrixAddNumericOp<Type>,
	Type>, Type>
	operator+(const Matrix<Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		NumericIteratorWrapper<Num>,
		MatrixAddNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs.begin(),
		NumericIteratorWrapper<Num>{ num },
		lhs.rows(),
		lhs.rows(),
		lhs.cols() },
		MatrixOpBase::NUMERICAL_ADD };
}

template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	typename Matrix<Type>::const_iterator,
	MatrixAddNumericOp<Type>,
	Type>, Type>
	operator+(const Num& num, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		typename Matrix<Type>::const_iterator,
		MatrixAddNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs.begin(),
		rhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::NUMERICAL_ADD };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	NumericIteratorWrapper<Num>,
	MatrixAddNumericOp<Type>,
	Type>, Type>
	operator+(const MatrixExpr<Iter, Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		NumericIteratorWrapper<Num>,
		MatrixAddNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs,
		NumericIteratorWrapper<Num>{ num },
		lhs.lhsRows(),
		lhs.lhsRows(),
		lhs.rhsCols() },
		MatrixOpBase::NUMERICAL_ADD };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	MatrixExpr<Iter, Type>,
	MatrixAddNumericOp<Type>,
	Type>, Type>
	operator+(const Num& num, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		MatrixExpr<Iter, Type>,
		MatrixAddNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs,
		rhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::NUMERICAL_ADD };
}

// Numerical subtraction operators
template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	NumericIteratorWrapper<Num>,
	MatrixSubNumericOp<Type>,
	Type>, Type>
	operator-(const Matrix<Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		NumericIteratorWrapper<Num>,
		MatrixSubNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs.begin(),
		NumericIteratorWrapper<Num>{ num },
		lhs.rows(),
		lhs.rows(),
		lhs.cols() },
		MatrixOpBase::NUMERICAL_SUB };
}

template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	typename Matrix<Type>::const_iterator,
	MatrixSubNumericOp<Type>,
	Type>, Type>
	operator-(const Num& num, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		typename Matrix<Type>::const_iterator,
		MatrixSubNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs.begin(),
		rhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::NUMERICAL_SUB };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	NumericIteratorWrapper<Num>,
	MatrixSubNumericOp<Type>,
	Type>, Type>
	operator-(const MatrixExpr<Iter, Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		NumericIteratorWrapper<Num>,
		MatrixSubNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs,
		NumericIteratorWrapper<Num>{ num },
		lhs.lhsRows(),
		lhs.lhsRows(),
		lhs.rhsCols() },
		MatrixOpBase::NUMERICAL_SUB };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	MatrixExpr<Iter, Type>,
	MatrixSubNumericOp<Type>,
	Type>, Type>
	operator-(const Num& num, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		MatrixExpr<Iter, Type>,
		MatrixSubNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs,
		rhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::NUMERICAL_SUB };
}

// Numerical multiplication operators
template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	NumericIteratorWrapper<Num>,
	MatrixMulNumericOp<Type>,
	Type>, Type>
	operator*(const Matrix<Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		NumericIteratorWrapper<Num>,
		MatrixMulNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs.begin(),
		NumericIteratorWrapper<Num>{ num },
		lhs.rows(),
		lhs.rows(),
		lhs.cols() },
		MatrixOpBase::NUMERICAL_MUL };
}

template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	typename Matrix<Type>::const_iterator,
	MatrixMulNumericOp<Type>,
	Type>, Type>
	operator*(const Num& num, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		typename Matrix<Type>::const_iterator,
		MatrixMulNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs.begin(),
		rhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::NUMERICAL_MUL };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	NumericIteratorWrapper<Num>,
	MatrixMulNumericOp<Type>,
	Type>, Type>
	operator*(const MatrixExpr<Iter, Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		NumericIteratorWrapper<Num>,
		MatrixMulNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs,
		NumericIteratorWrapper<Num>{ num },
		lhs.lhsRows(),
		lhs.lhsRows(),
		lhs.rhsCols() },
		MatrixOpBase::NUMERICAL_MUL };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	MatrixExpr<Iter, Type>,
	MatrixMulNumericOp<Type>,
	Type>, Type>
	operator*(const Num& num, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		MatrixExpr<Iter, Type>,
		MatrixMulNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs,
		rhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::NUMERICAL_MUL };
}

// Numerical division operators
template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	NumericIteratorWrapper<Num>,
	MatrixDivNumericOp<Type>,
	Type>, Type>
	operator/(const Matrix<Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		NumericIteratorWrapper<Num>,
		MatrixDivNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs.begin(),
		NumericIteratorWrapper<Num>{ num },
		lhs.rows(),
		lhs.rows(),
		lhs.cols() },
		MatrixOpBase::NUMERICAL_DIV };
}

template<class Type, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	typename Matrix<Type>::const_iterator,
	MatrixDivNumericOp<Type>,
	Type>, Type>
	operator/(const Num& num, const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		typename Matrix<Type>::const_iterator,
		MatrixDivNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs.begin(),
		rhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::NUMERICAL_DIV };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	NumericIteratorWrapper<Num>,
	MatrixDivNumericOp<Type>,
	Type>, Type>
	operator/(const MatrixExpr<Iter, Type>& lhs, const Num& num) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		NumericIteratorWrapper<Num>,
		MatrixDivNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		lhs,
		NumericIteratorWrapper<Num>{ num },
		lhs.lhsRows(),
		lhs.lhsRows(),
		lhs.rhsCols() },
		MatrixOpBase::NUMERICAL_DIV };
}

template<class Type, class Iter, class Num>
inline MatrixExpr<MatBinExpr<
	NumericIteratorWrapper<Num>,
	MatrixExpr<Iter, Type>,
	MatrixDivNumericOp<Type>,
	Type>, Type>
	operator/(const Num& num, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		NumericIteratorWrapper<Num>,
		MatrixExpr<Iter, Type>,
		MatrixDivNumericOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		NumericIteratorWrapper<Num>{ num },
		rhs,
		rhs.lhsRows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::NUMERICAL_DIV };
}

template<class Type>
inline Matrix<Type>& operator*=(Matrix<Type>& lhs, const Matrix<Type>& rhs) noexcept
{
	auto rit = rhs.begin();
	for (auto lit = std::begin(lhs); lit != std::end(lhs); ++lit, ++rit)
		*lit *= *rit;
	return lhs;
}