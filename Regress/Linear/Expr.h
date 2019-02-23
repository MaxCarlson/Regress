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

	struct col_const_iterator : public col_iterator_base<true>
	{
		using MyBase = col_iterator_base<true>;
		using ItType = typename MyBase::ItType;
		using ContainerType = typename MyBase::ContainerType;

		col_const_iterator(ItType it, ContainerType& cont) :
			MyBase{it, cont}
		{}
	};

	col_const_iterator begin() const { return col_const_iterator{ vals.cbegin(), *this }; }
};

struct MatrixOpBase
{
	using size_type = int;

	enum Op : size_type
	{
		PLUS,
		MINUS,
		MULTIPLY
	};
};

template<class Type>
class MatrixMultOp : public MatrixOpBase
{
	inline MatrixMultOp(size_type) {}
};

template<class Iter>
class MatrixExpr
{
	using size_type = typename MatrixOpBase::size_type;

	const Iter& expr;
	size_type op;

public:

	inline MatrixExpr(const Iter& expr, size_type op) noexcept :
		expr{expr},
		op{op}
	{}
};

template<class LIt, class RIt, class Op>
class MatBinExpr
{
	using size_type = typename MatrixOpBase::size_type;

	LIt lit;
	RIt rit;
	const Op op;
	const size_type lhsRows;
	const size_type rhsRows;
	const size_type lhsCols;
	const size_type rhsCols;

public:


	MatBinExpr(LIt lit, RIt rit, size_type lhsRows, size_type rhsRows, size_type lhsCols, size_type rhsCols) :
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
	typename MatrixT<Type>::col_const_iterator,
	typename MatrixT<Type>::col_const_iterator,
	MatrixMultOp<Type>>>
	operator*(const MatrixT<Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::col_const_iterator,
		typename MatrixT<Type>::col_const_iterator,
		MatrixMultOp<Type>>;
	return MatrixExpr<ExprType>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		lhs.cols(),
		rhs.cols()},
		MatrixOpBase::Op::MULTIPLY };
}


