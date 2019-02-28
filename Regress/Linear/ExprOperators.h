#pragma once
#include "Expr.h"

// Addition operators

template<class Type>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::const_iterator,
	typename MatrixT<Type>::const_iterator,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const MatrixT<Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::const_iterator,
		typename MatrixT<Type>::const_iterator,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::PLUS };
}

template<class Type, class Iter>
MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename MatrixT<Type>::const_iterator,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const MatrixExpr<Iter, Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename MatrixT<Type>::const_iterator,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs.begin(),
		lhs.lhsRows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::PLUS };
}

template<class Type, class Iter>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixAddOp<Type>,
	Type>, Type>
	operator+(const MatrixT<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixAddOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs,
		lhs.rows(), 
		rhs.lhsRows(),		// << TODO: Issue happening here with mul expression
		rhs.rhsCols() },	// <<
		MatrixOpBase::Op::PLUS };
}

template<class Type, class Iter1, class Iter2>
MatrixExpr<MatBinExpr<
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
		MatrixOpBase::Op::PLUS };
}

// Multiply operators

template<class Type>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::const_iterator,
	typename MatrixT<Type>::const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixT<Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::const_iterator,
		typename MatrixT<Type>::const_iterator,
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
MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename MatrixT<Type>::const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixExpr<Iter, Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename MatrixT<Type>::const_iterator,
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
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixT<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::const_iterator,
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
MatrixExpr<MatBinExpr<
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