#pragma once
#include <vector>

template<class E>
struct MatrixTBase
{ };

// TODO: Add a storage abstraction so we can have multiple things point to the same memory, such as view slices
// without duplications
template<class Type>
class MatrixT : public MatrixTBase<Type>
{
public:
	using Storage			= std::vector<Type>;
	using iterator			= typename Storage::iterator;
	using const_iterator	= typename Storage::const_iterator;
	using size_type			= int;

	

private:
	size_type	nrows;
	size_type	ncols;
	size_type	lastRowIdx;
	Storage		vals;			// TODO: This should really be a column-order matrix 


public:
	using value_type	= Type;
	using ThisType		= MatrixT<Type>;
	using SubType		= Type;

	MatrixT() = default;
	MatrixT(size_type nrows, size_type ncols);
	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m);

	// Handles assignment from expressions
	template<class Expr>
	MatrixT(const Expr& expr);

	size_type size() const noexcept { return vals.size(); }
	size_type rows() const noexcept { return nrows; }
	size_type cols() const noexcept { return ncols; }


	iterator		begin()		  noexcept { return vals.begin(); }
	iterator		end()		  noexcept { return vals.end(); }
	const_iterator	begin() const noexcept { return vals.cbegin(); }
	const_iterator	end()   const noexcept { return vals.cend(); }

	class col_iterator;
	class col_const_iterator;
	col_iterator		col_begin()		const noexcept { return { 0,		*this }; }
	col_iterator		col_end()		const noexcept { return { size(),	*this }; }
	col_const_iterator	ccol_begin()	const noexcept { return { 0,		*this }; }
	col_const_iterator	ccol_end()		const noexcept { return { size(),	*this }; }

	Type& operator()(int row, int col);
	const Type& operator()(int row, int col) const;

	bool operator==(const MatrixT& other) const;
	bool operator!=(const MatrixT& other) const;

	template<class Func>
	void unaryExpr(Func&& func);

	void resize(size_type numRows, size_type numCols);
	MatrixT<Type> transpose() const;

	template<bool isConst>
	class col_iterator_base
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		//using ItType = std::conditional_t<isConst,
		//	const_iterator,
		//	iterator>;
		using ContainerType = std::conditional_t<isConst,
			const ThisType,
			ThisType>;

		using reference = std::conditional_t<isConst,
			const Type&,
			Type&>;

		using pointer = std::conditional_t<isConst,
			const Type*,
			Type*>;
	private:
		size_type		idx;
		ContainerType& cont;

	public:
		col_iterator_base(size_type idx, ContainerType& cont) :
			idx{ idx },
			cont{ cont }
		{}

		// TODO: Perhaps iterating shouldn't do these tests, but dereferncing should!
		// would be much more effeciant
		col_iterator_base& operator++()
		{
			if (idx >= cont.size() - 1)
				++idx;
			else if (idx < cont.lastRowIdx)
				idx += cont.cols();
			else
				++idx %= cont.cols();
			
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
			if (idx <= 0 || idx >= cont.size())
				--idx;
			else if (idx < cont.cols())
			{
				--idx;
				idx += cont.lastRowIdx;
			}
			else
				idx -= cont.cols();
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
			return cont.vals[idx];
		}

		pointer operator->() const
		{
			return &cont.vals[idx];
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

	class col_const_iterator : public col_iterator_base<true>
	{
	public:
		using MyBase		= col_iterator_base<true>;
		using ContainerType = typename MyBase::ContainerType;

		col_const_iterator(size_type idx, ContainerType& cont) :
			MyBase{ idx, cont }
		{}
	};

	class col_iterator : public col_iterator_base<false>
	{
	public:
		using MyBase		= col_iterator_base<false>;
		using ContainerType = typename MyBase::ContainerType;

		col_iterator(size_type idx, ContainerType& cont) :
			MyBase{ idx, cont }
		{}
	};
};

template<class Type>
inline MatrixT<Type>::MatrixT(size_type nrows, size_type ncols) :
	nrows{ nrows },
	ncols{ ncols },
	lastRowIdx{ (nrows - 1) * ncols },
	vals(nrows * ncols)
{
}

template<class Type>
inline MatrixT<Type>::MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
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
inline MatrixT<Type>::MatrixT(const Expr & expr)
{
	expr.assign(*this);
}

template<class Type>
inline Type & MatrixT<Type>::operator()(int row, int col)
{	// TODO: Changing access methods would allow us to change this to a column major order matrix
	// (as well as initilizer list init)
	return vals[row * ncols + col]; 
}

template<class Type>
inline const Type & MatrixT<Type>::operator()(int row, int col) const
{
	return vals[row * ncols + col];
}

template<class Type>
inline bool MatrixT<Type>::operator==(const MatrixT& other) const
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
inline bool MatrixT<Type>::operator!=(const MatrixT & other) const
{
	return !(*this == other);
}

template<class Type>
template<class Func>
inline void MatrixT<Type>::unaryExpr(Func && func)
{
#pragma omp parallel for
	for (int i = 0; i < size(); ++i)
		func(vals[i]);
}

template<class Type>
inline void MatrixT<Type>::resize(size_type numRows, size_type numCols) 
{
	nrows = numRows;
	ncols = numCols;
	lastRowIdx = (numRows - 1) * numCols;
	vals.resize(numRows * numCols);
}

template<class Type>
inline MatrixT<Type> MatrixT<Type>::transpose() const
{
	MatrixT<Type> m(ncols, nrows);
	m = ~(*this);
	return m;
}



