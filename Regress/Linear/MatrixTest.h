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

	// TODO: Column iterator

private:
	size_type	nrows;
	size_type	ncols;
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

};

template<class Type>
inline MatrixT<Type>::MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
	nrows{ static_cast<size_type>(m.size()) },
	ncols{ static_cast<size_type>(m.begin()->size()) },
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
	vals.resize(numRows * numCols);
}


