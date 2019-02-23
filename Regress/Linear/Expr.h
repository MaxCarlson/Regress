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
	using ThisType		= MatrixT<Type>;
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

	int size() const { return vals.size(); }
	int rows() const { return nrows; }
	int cols() const { return ncols; }


	template<bool isConst>
	class col_iterator_base 
	{
	protected:
		using iterator_category = std::random_access_iterator_tag;
		using ItType = std::conditional_t<isConst, 
			typename Storage::const_iterator, 
			typename Storage::iterator>;
		using ContainerType = std::conditional_t<isConst,
			const ThisType,
			ThisType>;

		using reference = std::conditional_t<isConst,
			const Type&,
			Type&>;

		using pointer = std::conditional_t<isConst,
			const Type*,
			Type*>;

		ItType			it;
		ContainerType	cont;

	public:
		col_iterator_base(ItType it, ContainerType& cont) :
			it{ it },
			cont{ cont }
		{}

		col_iterator_base& operator++()
		{
			++it;
			return *this;
		}

		col_iterator_base& operator--()
		{
			--it;
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
	};

	struct const_col_iterator : public col_iterator_base<true>
	{
		using MyBase = col_iterator_base<true>;
		using ItType = typename MyBase::ItType;
		using ContainerType = typename MyBase::ContainerType;

		const_col_iterator(ItType it, ContainerType& cont) :
			MyBase{it, cont}
		{}
	};

	const_col_iterator begin() const { return const_col_iterator{ vals.cbegin(), *this }; }
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
/*
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
*/

