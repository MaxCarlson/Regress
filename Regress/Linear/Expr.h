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
	using size_type = int;

	size_type nrows;
	size_type ncols;
	Storage vals;


public:
	using value_type	= Type;
	using ThisType		= MatrixT<Type>;
	using SubType		= Type;

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

	size_type size() const { return vals.size(); }
	size_type rows() const { return nrows; }
	size_type cols() const { return ncols; }
	void resize(size_type numRows, size_type numCols)
	{
		nrows = numRows;
		ncols = numCols;
		vals.reserve(numRows * numCols);
	}

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

	template<class Lit, class Rit>
	inline Type operator()(Lit lit, Rit rit, size_type lhsRows, size_type rhsRows)
	{
		Type result = 0;
		while (--rhsRows >= 0)
		{
			result += *lit * *rit;
			++lit;
			rit += rhsRows;
		}
		return result;
	}
};

template<class Iter, class Type>
class MatrixExpr
{
	using size_type		= typename MatrixOpBase::size_type;
	using MatrixType	= MatrixT<Type>;

	const Iter& expr;
	size_type op;

public:

	inline MatrixExpr(const Iter& expr, size_type op) noexcept :
		expr{expr},
		op{op}
	{}

	inline size_type lhsRows() const noexcept { return expr.lhsRows(); }
	inline size_type rhsRows() const noexcept { return expr.rhsRows(); }
	inline size_type lhsCols() const noexcept { return expr.lhsCols(); }
	inline size_type rhsCols() const noexcept { return expr.rhsCols(); }


	// TODO: Will need to be modified as more operations are added!
	size_type rowSize() const noexcept
	{
		return lhsRows();
	}

	// TODO: Will possibly need to be modified as more operations are added!
	size_type colSize() const noexcept
	{
		return rhsCols();
	}

	void assign(MatrixType& to)
	{

		to.resize(rowSize(), colSize());

	}
};

template<class LIt, class RIt, class Op, class Type>
class MatBinExpr
{
	using size_type = typename MatrixOpBase::size_type;

	LIt lit;
	RIt rit;
	const Op op;
	const size_type nlhsRows;
	const size_type nrhsRows;
	const size_type nlhsCols;
	const size_type nrhsCols;

public:


	MatBinExpr(LIt lit, RIt rit, size_type nlhsRows, size_type nrhsRows, size_type nlhsCols, size_type nrhsCols) :
		lit{ lit },
		rit{ rit },
		nlhsRows{ nlhsRows },
		nrhsRows{ nrhsRows },
		nlhsCols{ nlhsCols },
		nrhsCols{ nrhsCols },
		op{ op }
	{}

	inline void left_inc(int i)  noexcept { lit += i; }
	inline void left_dec(int i)  noexcept { lit -= i; }
	inline void right_inc(int i) noexcept { rit += i; }
	inline void right_dec(int i) noexcept { rit -= i; }

	inline size_type lhsRows() const noexcept { return nlhsRows; }
	inline size_type rhsRows() const noexcept { return nrhsRows; }
	inline size_type lhsCols() const noexcept { return nlhsCols; }
	inline size_type rhsCols() const noexcept { return nrhsCols; }


	Type operator*() const noexcept
	{
		return op(lit, rit, nlhsRows, nrhsRows);
	}
};

template<class Type>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::col_const_iterator,
	typename MatrixT<Type>::col_const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixT<Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::col_const_iterator,
		typename MatrixT<Type>::col_const_iterator,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs.begin(),
		lhs.rows(),
		rhs.rows(),
		lhs.cols(),
		rhs.cols()},
		MatrixOpBase::Op::MULTIPLY };
}


