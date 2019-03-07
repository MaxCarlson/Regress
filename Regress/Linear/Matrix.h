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
template<class Type, bool ColOrder = false>
class Matrix 
{
public:
	using Storage				= std::vector<Type>;
	using size_type				= int;

	template<bool>				
	class nonMajorOrderIteratorBase;	// TODO: Both of these names are long
	template<bool>
	class arrayOrderIteratorBase;

	using col_iterator			= std::conditional_t<ColOrder, 
		arrayOrderIteratorBase<false>, nonMajorOrderIteratorBase<false>>;
	using col_const_iterator	= std::conditional_t<ColOrder,
		arrayOrderIteratorBase<true>, nonMajorOrderIteratorBase<true>>;

	using row_iterator			= std::conditional_t<ColOrder, 
		nonMajorOrderIteratorBase<false>, arrayOrderIteratorBase<false>>;
	using row_const_iterator	= std::conditional_t<ColOrder,
		nonMajorOrderIteratorBase<true>, arrayOrderIteratorBase<true>>;

	using iterator				= std::conditional_t<ColOrder, col_iterator, row_iterator>;
	using const_iterator		= std::conditional_t<ColOrder, col_const_iterator, row_const_iterator>;

private:
	size_type	nrows;
	size_type	ncols;
	size_type	lastRowIdx;		// TODO: Get rid of this
	Storage		vals;			


public:
	using value_type	= Type;
	using ThisType		= Matrix<Type, ColOrder>;

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



	iterator		begin()			  noexcept { return { 0,		this }; }
	iterator		end()			  noexcept { return { size(),	this }; }
	const_iterator	begin()		const noexcept { return { 0,		this }; }
	const_iterator	end()		const noexcept { return { size(),	this }; }
	const_iterator	cbegin()	const noexcept { return { 0,		this }; }
	const_iterator	cend()		const noexcept { return { size(),	this }; }

	col_iterator		col_begin()			  noexcept { return { 0,		this }; }
	col_iterator		col_end()			  noexcept { return { size(),	this }; }
	col_const_iterator	col_begin()		const noexcept { return { 0,		this }; }
	col_const_iterator	col_end()		const noexcept { return { size(),	this }; }
	col_const_iterator	ccol_begin()	const noexcept { return { 0,		this }; }
	col_const_iterator	ccol_end()		const noexcept { return { size(),	this }; }

	row_iterator		row_begin()			  noexcept { return { 0,		this }; }
	row_iterator		row_end()			  noexcept { return { size(),	this }; }
	row_const_iterator	row_begin()		const noexcept { return { 0,		this }; }
	row_const_iterator	row_end()		const noexcept { return { size(),	this }; }
	row_const_iterator	crow_begin()	const noexcept { return { 0,		this }; }
	row_const_iterator	crow_end()		const noexcept { return { size(),	this }; }

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

	template<class T, bool ColOrder>
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
	class nonMajorOrderIteratorBase : public IteratorBase<isConst>
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
		nonMajorOrderIteratorBase() = default;
		nonMajorOrderIteratorBase(size_type idx, ContainerPtr cont) :
			idx{ idx },
			cont{ cont }
		{}

		ContainerPtr matPtr() const { return cont; }

		// TODO: Perhaps iterating shouldn't do these tests, but dereferncing should!
		// would be much more effeciant
		nonMajorOrderIteratorBase& operator++()
		{
			++idx;
			return *this;
		}

		// TODO: Revist for optimizations!
		nonMajorOrderIteratorBase& operator+=(size_type i)
		{
			idx += i;
			return *this;
		}

		nonMajorOrderIteratorBase& operator+(size_type i)
		{
			idx += i;
			return *this;
		}

		nonMajorOrderIteratorBase& operator--()
		{
			--idx;
			return *this;
		}

		// TODO: Revist for optimizations!
		nonMajorOrderIteratorBase& operator-=(size_type i)
		{
			idx -= i;
			return *this;
		}

		nonMajorOrderIteratorBase& operator-(size_type i)
		{
			idx -= i;
			return *this;
		}

	private:
		reference index() const noexcept
		{
			if (ColOrder)
				return cont->operator()(idx / cont->ncols, idx % cont->ncols);
			
			return cont->operator()(idx % cont->nrows, idx / cont->nrows);
		}
	public:
		reference operator*() const
		{
			return index();
		}

		pointer operator->() const
		{
			return &index();
		}

		bool operator==(const nonMajorOrderIteratorBase& other) const
		{
			return idx == other.idx;
		}

		bool operator!=(const nonMajorOrderIteratorBase& other) const
		{
			return !(*this == other);
		}

		bool operator>(const nonMajorOrderIteratorBase& other) const
		{
			return idx > other.idx;
		}
		bool operator<(const nonMajorOrderIteratorBase& other) const
		{
			return idx < other.idx;
		}

		bool operator>=(const nonMajorOrderIteratorBase& other) const
		{
			return idx >= other.idx;
		}

		bool operator<=(const nonMajorOrderIteratorBase& other) const
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
		arrayOrderIteratorBase(size_type idx, ContainerPtr cont) :
			it{ cont->vals.begin() + idx },
			cont{ cont }
		{}

		ContainerPtr matPtr() const { return cont; }

		ThisType& operator++()
		{
			++it;
			return *this;
		}

		ThisType& operator+=(size_type i)
		{
			it += i;
			return *this;
		}

		ThisType& operator+(size_type i)
		{
			it += i;
			return *this;
		}

		ThisType& operator--()
		{
			--it;
			return *this;
		}

		ThisType& operator-=(size_type i)
		{
			it -= i;
			return *this;
		}

		ThisType& operator-(size_type i)
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

template<class Type, bool ColOrder>
inline Matrix<Type, ColOrder>::Matrix(size_type nrows, size_type ncols) :
	nrows{ nrows },
	ncols{ ncols },
	lastRowIdx{ (nrows - 1) * ncols },
	vals(nrows * ncols)
{
}

template<class Type, bool ColOrder>
inline Matrix<Type, ColOrder>::Matrix(const std::initializer_list<std::initializer_list<Type>>& m) :
	nrows{ static_cast<size_type>(m.size()) },
	ncols{ static_cast<size_type>(m.begin()->size()) },
	lastRowIdx{ (nrows - 1) * ncols }, 
	vals(nrows * ncols)
{
	auto v = std::begin(vals);

	if(ColOrder)
		for(int i = 0; i < ncols; ++i)
			for (auto it = std::begin(m); it != std::end(m); ++it, ++v)
				*v = *(std::begin(*it) + i);
	else
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt, ++v)
				*v = *jt;
}

template<class Type, bool ColOrder>
template<class Expr>
inline Matrix<Type, ColOrder>::Matrix(Expr expr)
{
	ExprAnalyzer ea{ *this };
	expr.analyzeExpr(ea);
	expr.assign(*this);
}

template<class Type, bool ColOrder>
inline Type & Matrix<Type, ColOrder>::operator()(size_type row, size_type col)
{	
	return ColOrder ? vals[col * nrows + row] : vals[row * ncols + col];
}

template<class Type, bool ColOrder>
inline const Type & Matrix<Type, ColOrder>::operator()(size_type row, size_type col) const
{
	return ColOrder ? vals[col * nrows + row] : vals[row * ncols + col];
}

template<class Type, bool ColOrder>
inline bool Matrix<Type, ColOrder>::operator==(const Matrix& other) const
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

template<class Type, bool ColOrder>
inline bool Matrix<Type, ColOrder>::operator!=(const Matrix & other) const
{
	return !(*this == other);
}

template<class Type, bool ColOrder>
template<class Func>
inline void Matrix<Type, ColOrder>::unaryExpr(Func && func)
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool ColOrder>
template<class Func>
inline void Matrix<Type, ColOrder>::unaryExpr(Func && func) const
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool ColOrder>
template<class Func>
inline void Matrix<Type, ColOrder>::unaryExprPara(Func && func)
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool ColOrder>
template<class Func>
inline void Matrix<Type, ColOrder>::unaryExprPara(Func && func) const
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool ColOrder>
inline void Matrix<Type, ColOrder>::resize(size_type numRows, size_type numCols) 
{
	nrows = numRows;
	ncols = numCols;
	lastRowIdx = (numRows - 1) * numCols;
	vals.resize(numRows * numCols);
}

template<class Type, bool ColOrder>
inline Type Matrix<Type, ColOrder>::sum() const
{	// TODO: Test unrolling
	Type sum = 0;
	for (int i = 0; i < size(); ++i)
		sum += vals[i];
	return sum;
}

// TODO: This will need split code when we have columnMajorOrder template param
// TODO: This is a seriously non-optimal algorithm
template<class Type, bool ColOrder>
inline void Matrix<Type, ColOrder>::addColumn(size_type idx, Type val)
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

template<class Type, bool ColOrder>
inline Matrix<Type, ColOrder> Matrix<Type, ColOrder>::transpose() const
{
	return Matrix<Type>{ ~(*this) };
}

// All operations below this point return a Matrix Expression
// (only Expressions that can't be handled outside are here,
// the rest are in ExprOperators)

template<class Type, bool ColOrder>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	typename Matrix<Type>::const_iterator,
	MatrixCwiseProductOp<Type>, Type>, Type> Matrix<Type, ColOrder>::cwiseProduct(const Matrix<Type>& rhs) noexcept
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

template<class Type, bool ColOrder>
template<class Iter>
inline MatrixExpr<MatBinExpr<
	typename Matrix<Type>::const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixCwiseProductOp<Type>, Type>, Type> Matrix<Type, ColOrder>::cwiseProduct(const Matrix<Type>& rhs) noexcept
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

template<class Type, bool ColOrder>
inline std::ostream & operator<<(std::ostream & out, const Matrix<Type, ColOrder>& m)
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
