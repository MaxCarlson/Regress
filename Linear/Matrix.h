#pragma once
#include <vector>
#include <iostream>
#include "MatrixBase.h"
#include "Exprs\CwiseBinaryOp.h"
#include "Exprs\ProductOp.h"
#include "Exprs\Transpose.h"
#include "Evaluators\Evaluators.h" // TODO: Move these to MatrixBase
#include "Evaluators\Assignment.h"

//namespace regress{ // TODO: Wrap proj in namespaces

// Partial specilization of traits that allows us
// to get matrix's Type inside MatrixBase
template<class Type, bool MajorOrder>
struct Traits<Matrix<Type, MajorOrder>>
{
	using value_type = Type;
	using MatrixType = Matrix<Type, MajorOrder>;
	static constexpr bool MajorOrder = MajorOrder;
};

// TODO: Add a storage abstraction so we can have multiple things point to the same memory, such as view slices
// TODO: Add template param for using Column Major Order

// without duplications
template<class Type, bool MajorOrder = false>
class Matrix : public MatrixBase<Matrix<Type, MajorOrder>>
{
public:
	using Base			= MatrixBase<Matrix<Type, MajorOrder>>;
	using ThisType		= Matrix<Type, MajorOrder>;
	using Storage		= std::vector<Type>;
	using size_type		= int;
	using value_type	= Type;

	template<bool>
	class nonMajorOrderIteratorBase;	// TODO: Both of these names are long
	template<bool>
	class arrayOrderIteratorBase;
	template<bool>
	class inOrderIterator;

	using inorder_iterator = inOrderIterator<false>;
	using inorder_const_iterator = inOrderIterator<true>;

	using col_iterator = std::conditional_t<MajorOrder,
		arrayOrderIteratorBase<false>, nonMajorOrderIteratorBase<false>>;
	using col_const_iterator = std::conditional_t<MajorOrder,
		arrayOrderIteratorBase<true>, nonMajorOrderIteratorBase<true>>;

	using row_iterator = std::conditional_t<MajorOrder,
		nonMajorOrderIteratorBase<false>, arrayOrderIteratorBase<false>>;
	using row_const_iterator = std::conditional_t<MajorOrder,
		nonMajorOrderIteratorBase<true>, arrayOrderIteratorBase<true>>;

	using iterator = std::conditional_t<MajorOrder,
		col_iterator, row_iterator>;
	using const_iterator = std::conditional_t<MajorOrder,
		col_const_iterator, row_const_iterator>;

	using minor_iterator = std::conditional_t<MajorOrder,
		row_iterator, col_iterator>;
	using minor_const_iterator = std::conditional_t<MajorOrder,
		row_const_iterator, col_const_iterator>;



	static constexpr bool MajorOrder = MajorOrder;
	static constexpr bool IsExpr = false;
private:
	size_type	nrows;
	size_type	ncols;
	Storage		vals;


public:

	Matrix() = default;
	Matrix(size_type nrows, size_type ncols);
	Matrix(const std::initializer_list<std::initializer_list<Type>>& m);

	// Handles assignment from expressions
	template<class Expr>
	Matrix(const Expr& expr); // TODO: Look into expense of this copy

	Type* data()				noexcept { return vals.data(); }
	const Type* data()	const	noexcept { return vals.data(); }
	size_type size()	const	noexcept { return vals.size(); }
	size_type rows()	const	noexcept { return nrows; }
	size_type cols()	const	noexcept { return ncols; }

	Type& operator()(size_type row, size_type col);
	const Type& operator()(size_type row, size_type col) const;

	Type& index(size_type i)				noexcept { return vals[i]; }
	const Type& index(size_type i) const	noexcept { return vals[i]; }

	bool operator==(const Matrix& other) const;
	bool operator!=(const Matrix& other) const;


	// iterator will follow the array order of the container, i.e. the 
	// fastest way to iterate the matrix
	iterator		begin()			  noexcept { return { 0,		this }; }
	iterator		end()			  noexcept { return { size(),	this }; }
	const_iterator	begin()		const noexcept { return { 0,		this }; }
	const_iterator	end()		const noexcept { return { size(),	this }; }
	const_iterator	cbegin()	const noexcept { return { 0,		this }; }
	const_iterator	cend()		const noexcept { return { size(),	this }; }

	// This iterator follows the opposite major order iteration
	// compared to iterator. 
	// EX: if this is a row major order matrix, minor_iterator is a column iterator
	minor_iterator			m_begin()		  noexcept { return { 0,		this }; }
	minor_iterator			m_end()			  noexcept { return { size(),	this }; }
	minor_const_iterator	m_begin()	const noexcept { return { 0,		this }; }
	minor_const_iterator	m_end()		const noexcept { return { size(),	this }; }
	minor_const_iterator	m_cbegin()	const noexcept { return { 0,		this }; }
	minor_const_iterator	m_cend()	const noexcept { return { size(),	this }; }

	// inorder_iterator will always traverse the matrix left-right, top-bottom
	inorder_iterator		ino_begin()			  noexcept { return { 0,		this }; }
	inorder_iterator		ino_end()			  noexcept { return { size(),	this }; }
	inorder_const_iterator	ino_begin()		const noexcept { return { 0,		this }; }
	inorder_const_iterator	ino_end()		const noexcept { return { size(),	this }; }
	inorder_const_iterator	ino_cbegin()	const noexcept { return { 0,		this }; }
	inorder_const_iterator	ino_cend()		const noexcept { return { size(),	this }; }

	col_iterator		col_begin()			  noexcept { return { 0,		this }; }
	col_iterator		col_end()			  noexcept { return { size(),	this }; }
	col_const_iterator	col_begin()		const noexcept { return { 0,		this }; }
	col_const_iterator	col_end()		const noexcept { return { size(),	this }; }
	col_const_iterator	col_cbegin()	const noexcept { return { 0,		this }; }
	col_const_iterator	col_cend()		const noexcept { return { size(),	this }; }

	row_iterator		row_begin()			  noexcept { return { 0,		this }; }
	row_iterator		row_end()			  noexcept { return { size(),	this }; }
	row_const_iterator	row_begin()		const noexcept { return { 0,		this }; }
	row_const_iterator	row_end()		const noexcept { return { size(),	this }; }
	row_const_iterator	row_cbegin()	const noexcept { return { 0,		this }; }
	row_const_iterator	row_cend()		const noexcept { return { size(),	this }; }

private:
	arrayOrderIteratorBase<false>		ao_begin()			noexcept { return { 0, this }; }
	arrayOrderIteratorBase<true>		ao_begin()	const	noexcept { return { 0, this }; }
	nonMajorOrderIteratorBase<false>	nmo_begin()			noexcept { return { 0, this }; }
	nonMajorOrderIteratorBase<true>		nmo_begin()	const	noexcept { return { 0, this }; }
public:

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
	Type sum() const;

	void addColumn(size_type idx, Type val = {});

	template<class T, bool MajorOrder>
	inline friend std::ostream& operator<<(std::ostream& out, const Matrix& m);

	template<bool isConst>
	class IteratorBase
	{
	public:
		static constexpr bool MajorOrder = MajorOrder;

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

		using value_type = Type;
	};

	// An iterator that iterates through columns instead of rows
	template<bool isConst>
	class nonMajorOrderIteratorBase : public IteratorBase<isConst>
	{
	public:
		using Base				= IteratorBase<isConst>;
		using ThisType			= nonMajorOrderIteratorBase<isConst>;
		using iterator_category = typename Base::iterator_category;
		using ContainerType		= typename Base::ContainerType;
		using ContainerPtr		= typename Base::ContainerPtr;
		using value_type		= typename Base::value_type;
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

		template<bool isConst>
		nonMajorOrderIteratorBase(const arrayOrderIteratorBase<isConst>& it) :
			idx{it - it.matPtr()->ao_begin()},
			cont{it.matPtr()}
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

		size_type operator-(const nonMajorOrderIteratorBase& other) const
		{
			return idx - other.idx;
		}

	private:
		reference index() const noexcept
		{
			if (MajorOrder)
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
		using value_type		= typename Base::value_type;
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

		template<bool isConst>
		arrayOrderIteratorBase(const nonMajorOrderIteratorBase<isConst>& it) :
			it{ cont->vals.begin() + (it - it.matPtr()->nmo_begin()) },
			cont{ it.matPtr() }
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

		size_type operator-(const ThisType& other) const
		{
			return it - other.it;
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

	// An iterator that always traverses the Matrix left-right, top-bottom
	template<bool isConst>
	class inOrderIterator : public std::conditional_t<MajorOrder,
		nonMajorOrderIteratorBase<isConst>,
		arrayOrderIteratorBase<isConst>>
	{
	public:
		using Base = std::conditional_t<MajorOrder,
			nonMajorOrderIteratorBase<isConst>,
			arrayOrderIteratorBase<isConst>>;

		using iterator_category = typename Base::iterator_category;
		using ContainerPtr = typename Base::ContainerPtr;
		using value_type = typename Base::value_type;

		inOrderIterator(size_type idx, ContainerPtr ptr) :
			Base{ idx, ptr }
		{}
	};
};

template<class Type, bool MajorOrder>
inline Matrix<Type, MajorOrder>::Matrix(size_type nrows, size_type ncols) :
	nrows{ nrows },
	ncols{ ncols },
	vals(nrows * ncols)
{}

template<class Type, bool MajorOrder>
inline Matrix<Type, MajorOrder>::Matrix(const std::initializer_list<std::initializer_list<Type>>& m) :
	nrows{ static_cast<size_type>(m.size()) },
	ncols{ static_cast<size_type>(m.begin()->size()) },
	vals(nrows * ncols)
{
	auto v = std::begin(vals);

	if (MajorOrder)
		for (int i = 0; i < ncols; ++i)
			for (auto it = std::begin(m); it != std::end(m); ++it, ++v)
				*v = *(std::begin(*it) + i);
	else
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt, ++v)
				*v = *jt;
}

template<class Type, bool MajorOrder>
template<class Expr>
inline Matrix<Type, MajorOrder>::Matrix(const Expr& expr)
{
	expr.assign(*this);
}

template<class Type, bool MajorOrder>
inline Type & Matrix<Type, MajorOrder>::operator()(size_type row, size_type col)
{
	return MajorOrder ? vals[col * nrows + row] : vals[row * ncols + col];
}

template<class Type, bool MajorOrder>
inline const Type & Matrix<Type, MajorOrder>::operator()(size_type row, size_type col) const
{
	return MajorOrder ? vals[col * nrows + row] : vals[row * ncols + col];
}

template<class Type, bool MajorOrder>
inline bool Matrix<Type, MajorOrder>::operator==(const Matrix& other) const
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

template<class Type, bool MajorOrder>
inline bool Matrix<Type, MajorOrder>::operator!=(const Matrix & other) const
{
	return !(*this == other);
}

template<class Type, bool MajorOrder>
template<class Func>
inline void Matrix<Type, MajorOrder>::unaryExpr(Func && func)
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool MajorOrder>
template<class Func>
inline void Matrix<Type, MajorOrder>::unaryExpr(Func && func) const
{
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool MajorOrder>
template<class Func>
inline void Matrix<Type, MajorOrder>::unaryExprPara(Func && func)
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool MajorOrder>
template<class Func>
inline void Matrix<Type, MajorOrder>::unaryExprPara(Func && func) const
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type, bool MajorOrder>
inline void Matrix<Type, MajorOrder>::resize(size_type numRows, size_type numCols)
{
	nrows = numRows;
	ncols = numCols;
	vals.resize(numRows * numCols);
}

template<class Type, bool MajorOrder>
inline Type Matrix<Type, MajorOrder>::sum() const
{	// TODO: Test unrolling
	Type sum = 0;
	for (int i = 0; i < size(); ++i)
		sum += vals[i];
	return sum;
}

// TODO: This is a seriously non-optimal algorithm
template<class Type, bool MajorOrder>
inline void Matrix<Type, MajorOrder>::addColumn(size_type idx, Type val)
{
	if (idx > ncols + 1 || idx < 0) // TODO: Test only on debug
		throw std::runtime_error("Invalid column coordinate");

	Matrix<Type, MajorOrder> tmp(nrows, ncols + 1);

	if (MajorOrder)
	{
		size_type ffill = idx * nrows;
		auto fit = std::begin(vals) + ffill;
		auto tb = std::begin(tmp.vals);

		std::move(vals.begin(), fit, tb);
		std::fill(tb + ffill, tb + (ffill + nrows), val);
		std::move(fit, vals.end(), tb + (ffill + nrows));

		*this = std::move(tmp);
		return;
	}

	int i = 0;
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

			*it = std::move(*v);
			++v;
		}

		if (i++ >= ncols)
			i = 0;
		++it;
	}
	*this = std::move(tmp);
}

template<class Type, bool MajorOrder>
inline std::ostream & operator<<(std::ostream & out, const Matrix<Type, MajorOrder>& m)
{
	using Mat = Matrix<Type, MajorOrder>;
	using It = typename Mat::inorder_const_iterator;

	int i = 0;
	for (It it = m.ino_cbegin(); it != m.ino_cend(); ++it)
	{
		if (i++ % m.cols() == 0)
			out << '\n';
		out << *it << ' ';
	}

	return out;
}

//} // End: namespace::regress

