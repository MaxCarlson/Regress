#pragma once
#include <vector>

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)


template<class E>
class BaseExpr
{

};

template<class E>
struct MatrixTBase : public BaseExpr<E>
{ };

template<class Type>
class MatrixT : public MatrixTBase<MatrixT<Type>>
{
	int nrows;
	int ncols;
	std::vector<Type> vals;

public:
	using value_type	= Type;
	using MyType		= MatrixT<Type>;
	using SubType		= Type;

	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
		nrows{ static_cast<int>(m.size()) },
		ncols{ static_cast<int>(m.begin()->size()) },
		vals(nrows * ncols)
	{
		int i = 0;
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
				vals[i++] = *jt;
	}

	template<class E>
	MatrixT(BaseExpr<E> expr) :
		nrows{},
		ncols{},
		vals{}
	{
		ExprAnalyzer ea;
		expr.analyzeLeft(ea);

		int idx = 0;
		for(int i = 0; i < nrows; ++i)
			for (int j = 0; j < ncols; ++j, ++idx)
			{
				vals[idx] = expr.evalExpr(i, j, idx);
			}
	}



	int size() const { return vals.size(); }
	int rows() const { return nrows; }
	int cols() const { return ncols; }
};


