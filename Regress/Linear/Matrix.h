#pragma once
#include <vector>
#include <iostream>
#include "Expr.h"
#include "ExprOperators.h" // Matrix doesn't rely on ExprOperators, it's here to make it easy to use Matrix

template<class E>
struct MatrixBase
{ };

/*
template<class Iter, class Type>
class MatrixExpr;
template<class LIt, class RIt, class Op, class Type>
class MatBinExpr;
template<class Type>
class MatrixCwiseProductOp;
*/

// TODO: Add a storage abstraction so we can have multiple things point to the same memory, such as view slices
// TODO: Add template param for using Column Major Order

// without duplications
template<class Type>
class Matrix 
{
public:
	using Storage				= std::vector<Type>;
	using size_type				= int;

	template<bool> // TODO: This is all setup so we can use std::conditional with these when
	class col_iterator_base; // we have a template param for column-major-order vs row
	using col_iterator			= col_iterator_base<false>;
	using col_const_iterator	= col_iterator_base<true>;
	template<bool>
	class arrayOrderIteratorBase;
	using row_iterator			= arrayOrderIteratorBase<false>;
	using row_const_iterator	= arrayOrderIteratorBase<true>;
	using iterator				= row_iterator;
	using const_iterator		= row_const_iterator;

private:
	size_type	nrows;
	size_type	ncols;
	size_type	lastRowIdx;
	Storage		vals;			// TODO: This should really be a column-order matrix 


public:
	using value_type	= Type;
	using ThisType		= Matrix<Type>;

	Matrix() = default;
	Matrix(size_type nrows, size_type ncols);
	Matrix(const std::initializer_list<std::initializer_list<Type>>& m);

	// Handles assignment from expressions
	template<class Expr>
	Matrix(Expr expr); // TODO: Look into expense of this copy

	size_type size() const noexcept { return vals.size(); }
	size_type rows() const noexcept { return nrows; }
	size_type cols() const noexcept { return ncols; }

	Type& operator()(size_type row, size_type col);
	const Type& operator()(size_type row, size_type col) const;

	bool operator==(const Matrix& other) const;
	bool operator!=(const Matrix& other) const;

	iterator		begin()			  noexcept { return { vals.begin(),		this }; }
	iterator		end()			  noexcept { return { vals.end(),		this }; }
	const_iterator	begin()		const noexcept { return { vals.cbegin(),	this }; }
	const_iterator	end()		const noexcept { return { vals.cend(),		this }; }
	const_iterator	cbegin()	const noexcept { return { vals.cbegin(),	this }; }
	const_iterator	cend()		const noexcept { return { vals.cend(),		this }; }

	col_iterator		col_begin()			  noexcept { return { 0,		this }; }
	col_iterator		col_end()			  noexcept { return { size(),	this }; }
	col_const_iterator	col_begin()		const noexcept { return { 0,		this }; }
	col_const_iterator	col_end()		const noexcept { return { size(),	this }; }
	col_const_iterator	ccol_begin()	const noexcept { return { 0,		this }; }
	col_const_iterator	ccol_end()		const noexcept { return { size(),	this }; }

	// Apply a function to every member of the Matrix
	// [](Type& t) { ...do something... return; }
	template<class Func>
	void unaryExpr(Func&& func);
	template<class Func>
	void unaryExpr(Func&& func) const;
	template<class Func>
	void unaryExprPara(Func&& func);
	template<class Func>
	void unaryExprPara(Func&& func) const;

	void resize(size_type numRows, size_type numCols);
	Matrix transpose() const;
	Type sum() const;

	void addColumn(size_type idx, Type val = {});

	// These return Matrix Expressions so no temporary matricies are created
	inline MatrixExpr<MatBinExpr<
		typename Matrix<Type>::const_iterator,
		typename Matrix<Type>::const_iterator,
		MatrixCwiseProductOp<Type>,
		Type>, Type>
		cwiseProduct(const Matrix<Type>& rhs) noexcept;

	template<class Iter>
	inline MatrixExpr<MatBinExpr<
		typename Matrix<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixCwiseProductOp<Type>,
		Type>, Type>
		cwiseProduct(const Matrix<Type>& rhs) noexcept;

	template<class T>
	inline friend std::ostream& operator<<(std::ostream& out, const Matrix<T>& m);

	template<bool isConst>
	class IteratorBase
	{
	public:
		using iterator_category = std::random_access_iterator_tag;

		using ContainerType = std::conditional_t<isConst,
			const ThisType, ThisType>;

		using ContainerPtr = std::conditional_t<isConst,
			const ThisType*, ThisType*>;

		using reference = std::conditional_t<isConst,
			const Type&, Type&>;

		using pointer = std::conditional_t<isConst,
			const Type*, Type*>;

		using Siterator = std::conditional_t<isConst, 
			typename Storage::const_iterator,
			typename Storage::iterator>;

		inline void analyzeExpr(ExprAnalyzer<Type>& ea) {}
	};

	// An iterator that iterates through columns instead of rows
	template<bool isConst>
	class col_iterator_base : public IteratorBase<isConst>
	{
	public:
		using Base				= IteratorBase<isConst>;
		using iterator_category = typename Base::iterator_category;
		using ContainerType		= typename Base::ContainerType;
		using ContainerPtr		= typename Base::ContainerPtr;
		using reference			= typename Base::reference;
		using pointer			= typename Base::pointer;
			
	private:
		size_type		idx;
		ContainerPtr	cont;

	public:
		col_iterator_base() = default;
		col_iterator_base(size_type idx, ContainerPtr cont) :
			idx{ idx },
			cont{ cont }
		{}

		ContainerPtr matPtr() const { return cont; }

		// TODO: Perhaps iterating shouldn't do these tests, but dereferncing should!
		// would be much more effeciant
		col_iterator_base& operator++()
		{
			if (idx >= cont->size() - 1)
				++idx;
			else if (idx < cont->lastRowIdx)
				idx += cont->cols();
			else
				++idx %= cont->cols();
			
			return *this;
		}

		// TODO: Revist for optimizations!
		col_iterator_base& operator+=(size_type i)
		{
			while (i--)
				++(*this);
			return *this;
		}

		col_iterator_base& operator--()
		{
			if (idx <= 0 || idx >= cont->size())
				--idx;
			else if (idx < cont->cols())
			{
				--idx;
				idx += cont->lastRowIdx;
			}
			else
				idx -= cont->cols();
			return *this;
		}

		// TODO: Revist for optimizations!
		col_iterator_base& operator-=(size_type i)
		{
			while (i--)
				--(*this);
			return *this;
		}

		reference operator*() const
		{
			return cont->vals[idx];
		}

		pointer operator->() const
		{
			return &cont->vals[idx];
		}

		bool operator==(const col_iterator_base& other) const
		{
			return &cont == &other.cont
				&& idx == other.idx;
		}

		bool operator!=(const col_iterator_base& other) const
		{
			return !(*this == other);
		}

		bool operator>(const col_iterator_base& other) const
		{
			return idx > other.idx;
		}
		bool operator<(const col_iterator_base& other) const
		{
			return idx < other.idx;
		}

		bool operator>=(const col_iterator_base& other) const
		{
			return idx >= other.idx;
		}

		bool operator<=(const col_iterator_base& other) const
		{
			return idx <= other.idx;
		}
	};

	// A wrapper class so we can get the Matrix from the iterator
	template<bool isConst>
	class arrayOrderIteratorBase : public IteratorBase<isConst>
	{
	public:
		using Base				= IteratorBase<isConst>;
		using iterator_category = typename Base::iterator_category;
		using ContainerType		= typename Base::ContainerType;
		using ContainerPtr		= typename Base::ContainerPtr;
		using reference			= typename Base::reference;
		using pointer			= typename Base::pointer;
		using It				= typename Base::Siterator;
		using ThisType			= arrayOrderIteratorBase<isConst>;
		
	private:
		It				it;
		ContainerPtr	cont;

	public:

		arrayOrderIteratorBase() = default;
		arrayOrderIteratorBase(It it, ContainerPtr cont) :
			it{ it },
			cont{ cont }
		{}

		ContainerPtr matPtr() const { return cont; }

		ThisType& operator++()
		{
			++it;
			return *this;
		}

		ThisType& operator--()
		{
			--it;
			return *this;
		}

		ThisType& operator+=(size_type i)
		{
			it += i;
			return *this;
		}

		ThisType& operator-=(size_type i)
		{
			it -= i;
			return *this;
		}
		
		reference operator*() const
		{
			return *it;
		}

		pointer operator->() const
		{
			return &(*it);
		}

		bool operator==(const ThisType& other) const
		{
			return it == other.it;
		}

		bool operator!=(const ThisType& other) const
		{
			return it != other.it;
		}

		bool operator>(const ThisType& other) const
		{
			return it > other.it;
		}
		bool operator<(const ThisType& other) const
		{
			return it < other.it;
		}

		bool operator>=(const ThisType& other) const
		{
			return it >= other.it;
		}

		bool operator<=(const ThisType& other) const
		{
			return it <= other.it;
		}
	};
};

template<class Type>
inline Matrix<Type>::Matrix(size_type nrows, size_type ncols) :
	nrows{ nrows },
	ncols{ ncols },
	lastRowIdx{ (nrows - 1) * ncols },
	vals(nrows * ncols)
{
}

template<class Type>
inline Matrix<Type>::Matrix(const std::initializer_list<std::initializer_list<Type>>& m) :
	nrows{ static_cast<size_type>(m.size()) },
	ncols{ static_cast<size_type>(m.begin()->size()) },
	lastRowIdx{ (nrows - 1) * ncols }, 
	vals(nrows * ncols)
{
	int i = 0;
	for (auto it = std::begin(m); it != std::end(m); ++it)
		for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
			vals[i++] = *jt;
}

template<class Type>
template<class Expr>
inline Matrix<Type>::Matrix(Expr expr)
{
	ExprAnalyzer ea{ *this };
	expr.analyzeExpr(ea);
	expr.assign(*this);
}

template<class Type>
inline Type & Matrix<Type>::operator()(size_type row, size_type col)
{	// TODO: Changing access methods would allow us to change this to a column major order matrix
	// (as well as initilizer list init)
	return vals[row * ncols + col]; 
}

template<class Type>
inline const Type & Matrix<Type>::operator()(size_type row, size_type col) const
{
	return vals[row * ncols + col];
}

template<class Type>
inline bool Matrix<Type>::operator==(const Matrix& other) const
{
	if (rows() != other.rows()
		|| cols() != other.cols())
		return false;

	// TODO: Revisit and paralellize
	for (int i = 0; i < vals.size(); ++i)
		if (vals[i] != other.vals[i])
			return false;
	return true;
}

template<class Type>
inline bool Matrix<Type>::operator!=(const Matrix & other) const
{
	return !(*this == other);
}

template<class Type>
template<class Func>
inline void Matrix<Type>::unaryExpr(Func && func)
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type>
template<class Func>
inline void Matrix<Type>::unaryExpr(Func && func) const
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type>
template<class Func>
inline void Matrix<Type>::unaryExprPara(Func && func)
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type>
template<class Func>
inline void Matrix<Type>::unaryExprPara(Func && func) const
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type>
inline void Matrix<Type>::resize(size_type numRows, size_type numCols) 
{
	nrows = numRows;
	ncols = numCols;
	lastRowIdx = (numRows - 1) * numCols;
	vals.resize(numRows * numCols);
}

template<class Type>
inline Type Matrix<Type>::sum() const
{	// TODO: Test unrolling
	Type sum = 0;
	for (int i = 0; i < size(); ++i)
		sum += vals[i];
	return sum;
}

// TODO: This will need split code when we have columnMajorOrder template param
// TODO: This is a seriously non-optimal algorithm
template<class Type>
inline void Matrix<Type>::addColumn(size_type idx, Type val)
{
	Matrix<Type> tmp(nrows, ncols + 1);

	if (idx > ncols + 1 || idx < 0)
		throw std::runtime_error("Invalid column coordinate");

	int i	= 0;
	auto it = std::begin(tmp);
	for (auto v = std::begin(vals);;)
	{
		if (i == idx)
		{
			if (idx < ncols && v >= std::end(vals))
				break;
			*it = val;
		}
		else
		{
			if (idx >= ncols && v >= std::end(vals))
				break;

			*it = *v;
			++v;
		}
	
		if (i++ >= ncols)
			i = 0;
		++it;
	}
	*this = std::move(tmp);
}

template<class Type>
inline Matrix<Type> Matrix<Type>::transpose() const
{
	Matrix<Type> m(ncols, nrows);
	m = ~(*this);
	return m;
}

// All operations below this point return a Matrix Expression
// (only Expressions that can't be handled outside are here,
// the rest are in ExprOperators)

template<class Type>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	typename Matrix<Type>::const_iterator,
	MatrixCwiseProductOp<Type>, Type>, Type> Matrix<Type>::cwiseProduct(const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		typename Matrix<Type>::const_iterator,
		MatrixCwiseProductOp<Type>, Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		cbegin(),
		rhs.begin(),
		rows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::CWISE_PRODUCT};
}

template<class Type>
template<class Iter>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixCwiseProductOp<Type>, Type>, Type> Matrix<Type>::cwiseProduct(const Matrix<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename Matrix<Type>::const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixCwiseProductOp<Type>, Type>;
	return MatrixExpr<ExprType, Type>{ExprType{
		cbegin(),
		rhs,
		rows(),
		rhs.lhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::CWISE_PRODUCT};
}

template<class Type>
inline std::ostream & operator<<(std::ostream & out, const Matrix<Type>& m)
{
	int i = 0;
	for (const auto& v : m)
	{
		if (i++ % m.cols() == 0)
			out << '\n';
		out << v << ' ';
	}
	out << '\n';
	return out;
}
