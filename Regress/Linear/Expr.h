#pragma once
#include <vector>

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)




template<class E>
struct MatrixTBase
{ };

template<class Type>
class MatrixT : public MatrixTBase<Type>
{
	using Storage = std::vector<Type>;

	int nrows;
	int ncols;
	Storage vals;


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


	struct col_iterator 
	{
		using iterator_category = std::random_access_iterator_tag;

		col_iterator& operator++()
		{
			++it
			return *this;
		}

		Storage::iterator it;
	};
};

struct MatrixOpBase
{
	enum Op
	{
		PLUS,
		MINUS,
		MUL
	};
};

template<class Type>
class MatrixMultOp : public MatrixOpBase
{

};

template<class Iter>
class MatrixExpr
{

};

template<class LIt, class RIt, class Op>
class MatBinExpr
{
	LIt lit;
	RIt rit;
	Op op;


	const int lhsRows;
	const int rhsRows;
	const int lhsCols;
	const int rhsCols;

public:

	MatBinExpr(LIt lit, RIt rit, int lhsRows, int rhsRows, int lhsCols, int rhsCols, Op op) :
		lit{ lit },
		rit{ rit },
		lhsRows{ lhsRows },
		rhsRows{ rhsRows },
		lhsCols{ lhsCols },
		rhsCols{ rhsCols },
		op{ op }
	{}
};

template<class Type>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::col_iterator, 
	typename MatrixT<Type>::col_iterator,
	MatrixMultOp<Type>>>
	operator*(const MatrixT<Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using RetType = MatrixExpr<MatBinExpr<
		typename MatrixT<Type>::col_iterator,
		typename MatrixT<Type>::col_iterator,
		MatrixMultOp<Type>>>;
	return RetType{MatBinExpr{ 
		
	}
	
	}
}


