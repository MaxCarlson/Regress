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
public:
	inline MatrixMultOp(size_type) {}

	template<class Lit, class Rit>
	inline Type operator()(Lit lit, Rit rit, size_type rhsRows, size_type rhsCols) const noexcept
	{
		Type result = 0;
		for (;; --rhsRows, ++lit, rit += rhsCols)
		{
			result += *lit * *rit;
			if (rhsRows <= 1)
				break;
		}
		return result;
	}
};

//     +---+---+
//     | A | B |
//     +---+---+
// n = | C | D |
//     +---+---+
//     | E | F |
//     +---+---+
//
//     +---+---+---+---+
//     | Z | Y | X | W |
// m = +---+---+---+---+
//     | V | U | T | S |
//     +---+---+---+---+
//
//       +---------------+---------------+---------------+---------------+
//       | (A*Z) + (B*V) | (A*Y) + (B*U) | (A*X) + (B*T) | (A*W) + (B*S) |
//       +---------------+---------------+---------------+---------------+
// m*n = | (C*Z) + (D*V) | (C*Y) + (D*U) | (C*X) + (D*T) | (C*W) + (D*S) |
//       +---------------+---------------+---------------+---------------+
//       | (E*Z) + (F*V) | (E*Y) + (F*U) | (E*X) + (F*T) | (E*W) + (F*S) |
//       +---------------+---------------+---------------+---------------+
//

template<class Iter, class Type>
class MatrixExpr
{
	using size_type		= typename MatrixOpBase::size_type;
	using MatrixType	= MatrixT<Type>;
	using ThisType		= MatrixExpr<Iter, Type>;

	Iter exprOp;
	const size_type op;		// TODO: op should be compile time so we can use constexpr
	size_type multiCount;	// Number of times the multiply op has been applied (if applicable)

public:

	inline MatrixExpr(const Iter& exprOp, size_type op) noexcept :
		exprOp{ exprOp },
		op{ op },
		multiCount{ 0 }
	{}

	inline Iter getExprOp()		const noexcept { return exprOp; }
	inline size_type opType()	const noexcept { return op; }
	inline size_type lhsRows()	const noexcept { return exprOp.lhsRows(); }
	inline size_type rhsRows()	const noexcept { return exprOp.rhsRows(); }
	inline size_type rhsCols()	const noexcept { return exprOp.rhsCols(); }

	inline Type operator*() const noexcept { return *exprOp; }

	inline MatrixExpr& operator++() noexcept
	{
		if (op == MatrixOpBase::MULTIPLY)
		{
			exprOp.rhsInc(1);
			if (++multiCount % rhsCols() == 0)
			{
				exprOp.rhsDec(rhsCols());
				exprOp.lhsInc(rhsRows()); // Same as lhsCols
			}
		}
		else
			++exprOp;

		return *this;
	}

	inline MatrixExpr& operator+=(size_type i) noexcept
	{
		while (i--) // Note: This checks expression branch each time (could be optimized)
			++(*this);
		return *this;
	}


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

	void assign(MatrixType& to) const
	{
		to.resize(rowSize(), colSize());

		// Fill the matrix 'to' column by column
		// Evaluating the expression tree the same way
		const_iterator rIt = begin();
		for (auto lIt = to.begin(); lIt != std::end(to); ++lIt, ++rIt)
			*lIt = *rIt;
	}

	class const_iterator
	{
		using MatrixExpr = ThisType;

		const MatrixExpr* expr; // Container class pointer
		Iter exprOp;			// Expression iterator of expr
		size_type multiCount;

	public:
		const_iterator(const MatrixExpr* expr) noexcept :
			expr{ expr },
			exprOp{ expr->getExprOp() },
			multiCount{ expr->multiCount }
		{}

		inline bool operator==(const const_iterator& other) noexcept
		{
			return expr == other.expr;
		}

		inline bool operator!=(const const_iterator& other) noexcept
		{
			return !(*this == other);
		}

		inline Type operator*() const noexcept
		{
			return *exprOp;
		}

		inline const_iterator& operator++() noexcept
		{
			if (expr->opType() == MatrixOpBase::Op::MULTIPLY)
			{
				exprOp.rhsInc(1);
				if (++multiCount % expr->rhsCols() == 0)
				{
					exprOp.rhsDec(expr->rhsCols());
					exprOp.lhsInc(expr->rhsRows());
				}
			}
			else
				++exprOp;
			return *this;
		}
	};

	const_iterator begin() const noexcept { return const_iterator{ this }; }
};

template<class LIt, class RIt, class Op, class Type>
class MatBinExpr
{
	using size_type = typename MatrixOpBase::size_type;
	using ThisType	= MatBinExpr<LIt, RIt, Op, Type>;

	LIt lit;
	RIt rit;
	const Op op;
	const size_type nlhsRows;
	const size_type nrhsRows;
	const size_type nrhsCols;

public:


	MatBinExpr(LIt lit, RIt rit, size_type nlhsRows, size_type nrhsRows, size_type nrhsCols) :
		lit{ lit },
		rit{ rit },
		nlhsRows{ nlhsRows },
		nrhsRows{ nrhsRows },
		nrhsCols{ nrhsCols },
		op{ 0 }
	{}

	inline void lhsInc(int i)  noexcept { lit += i; }
	inline void lhsDec(int i)  noexcept { lit -= i; }
	inline void rhsInc(int i)  noexcept { rit += i; }
	inline void rhsDec(int i)  noexcept { rit -= i; }

	inline size_type lhsRows() const noexcept { return nlhsRows; }
	inline size_type rhsRows() const noexcept { return nrhsRows; }
	inline size_type rhsCols() const noexcept { return nrhsCols; }


	Type operator*() const noexcept
	{
		return op(lit, rit, nrhsRows, nrhsCols);
	}

	ThisType& operator++() noexcept
	{
		lhsInc(1);
		rhsInc(1);
		return *this;
	}

	ThisType& operator--() noexcept
	{
		lhsDec(1);
		rhsDec(1);
		return *this;
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
		rhs.cols()},
		MatrixOpBase::Op::MULTIPLY };
}

template<class Type, class Iter>
MatrixExpr<MatBinExpr<
	MatrixExpr<Iter, Type>,
	typename MatrixT<Type>::col_const_iterator,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixExpr<Iter, Type>& lhs, const MatrixT<Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		MatrixExpr<Iter, Type>,
		typename MatrixT<Type>::col_const_iterator,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs,
		rhs.begin(),
		lhs.lhsRows(),
		rhs.rows(),
		rhs.cols() },
		MatrixOpBase::Op::MULTIPLY };
}

template<class Type, class Iter>
MatrixExpr<MatBinExpr<
	typename MatrixT<Type>::col_const_iterator,
	MatrixExpr<Iter, Type>,
	MatrixMultOp<Type>,
	Type>, Type>
	operator*(const MatrixT<Type>& lhs, const MatrixExpr<Iter, Type>& rhs) noexcept
{
	using ExprType = MatBinExpr<
		typename MatrixT<Type>::col_const_iterator,
		MatrixExpr<Iter, Type>,
		MatrixMultOp<Type>,
		Type>;
	return MatrixExpr<ExprType, Type>{ ExprType{
		lhs.begin(),
		rhs,
		lhs.rows(),
		rhs.rhsRows(),
		rhs.rhsCols() },
		MatrixOpBase::Op::MULTIPLY };
}