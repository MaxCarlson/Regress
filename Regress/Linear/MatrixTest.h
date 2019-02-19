#pragma once
#include "Expr.h"
template<class Type>
struct MatrixT
{
	int nrows;
	int ncolumns;
	std::vector<Type> vals;

	using ThisType = MatrixT<Type>;

	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
		nrows{ static_cast<int>(m.size()) },
		ncolumns{ static_cast<int>(m.begin()->size()) },
		vals(nrows * ncolumns)
	{
		int i = 0;
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
				vals[i++] = *jt;
	}

	MatrixT& operator=(BaseExpr* expr)
	{

	}

	template<class T>
	BaseExpr* operator+(MatrixT<T>& other)
	{
		return new AddExpr<ThisType, decltype(other)>(this, &other);
	}


};