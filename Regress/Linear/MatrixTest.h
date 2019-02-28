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
	using Storage				= std::vector<Type>;
	using col_iterator			= typename Storage::iterator;
	using col_const_iterator	= typename Storage::const_iterator;
	using size_type				= int;

private:
	size_type nrows;
	size_type ncols;
	Storage vals;


public:
	using value_type	= Type;
	using ThisType		= MatrixT<Type>;
	using SubType		= Type;

	MatrixT() = default;

	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
		nrows{ static_cast<size_type>(m.size()) },
		ncols{ static_cast<size_type>(m.begin()->size()) },
		vals(nrows * ncols)
	{
		int i = 0;
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
				vals[i++] = *jt;
	}

	template<class Expr>
	MatrixT(const Expr& expr)
	{
		expr.assign(*this);
	}

	size_type size() const { return vals.size(); }
	size_type rows() const { return nrows; }
	size_type cols() const { return ncols; }
	void resize(size_type numRows, size_type numCols)
	{
		nrows = numRows;
		ncols = numCols;
		vals.resize(numRows * numCols);
	}

	col_const_iterator begin() const	{ return vals.cbegin(); }
	col_const_iterator end()   const	{ return vals.cend(); }
	col_iterator begin()				{ return vals.begin(); }
	col_iterator end()					{ return vals.end(); }
};

