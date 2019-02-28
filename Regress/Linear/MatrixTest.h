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
	Storage		vals;


public:
	using value_type	= Type;
	using ThisType		= MatrixT<Type>;
	using SubType		= Type;

	MatrixT() = default;
	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m);

	// Handles assignment from expressions
	template<class Expr>
	MatrixT(const Expr& expr);

	size_type size() const noexcept { return vals.size(); }
	size_type rows() const noexcept { return nrows; }
	size_type cols() const noexcept { return ncols; }

	const_iterator begin() const noexcept { return vals.cbegin(); }
	const_iterator end()   const noexcept { return vals.cend(); }
	iterator begin()			 noexcept { return vals.begin(); }
	iterator end()				 noexcept { return vals.end(); }

	bool operator==(const MatrixT& other) const;

	void resize(size_type numRows, size_type numCols);

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

		col_iterator_base& operator++()
		{
			if (idx < cont.lastRowIdx)
				idx += cont.cols();
			else
				idx %= cont.cols();
			
			return *this;
		}

		col_iterator_base& operator+=(size_type i)
		{
			idx += i * cont.cols();
			if (idx >= cont.size())
			{

			}
			return *this;
		}

		col_iterator_base& operator--()
		{
			--it;
			return *this;
		}

		col_iterator_base& operator-=(size_type i)
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

		bool operator==(const col_iterator_base& other) const
		{
			return it == other.it;
		}

		bool operator!=(const col_iterator_base& other) const
		{
			return !(*this == other);
		}
	};
};

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
inline void MatrixT<Type>::resize(size_type numRows, size_type numCols) 
{
	nrows = numRows;
	ncols = numCols;
	lastRowIdx = (numRows - 1) * numCols;
	vals.resize(numRows * numCols);
}


