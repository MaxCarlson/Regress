#pragma once
#include <vector>
#include "MatrixTest.h"

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
class MatrixAddOp : public MatrixOpBase
{
public:
	inline MatrixAddOp(size_type) {}

	template<class Lit, class Rit>
	inline Type operator()(Lit lit, Rit rit, size_type rhsRows, size_type rhsCols) const noexcept
	{
		return *lit + *rit;
	}
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
		for (;; --rhsRows, ++lit, rit += rhsCols) // Current issues stems from rit += rhsCols (iterators are in wrong spot after if rit is an expression
		{
			result += *lit * *rit;
			if (rhsRows <= 1)
				break;
		}
		return result;
	}
};

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

private:

	template<bool Inc>
	inline void incrementSelf(size_type i) noexcept
	{
		if (op != MatrixOpBase::MULTIPLY)
		{
			if constexpr (Inc)
				exprOp += i;
			else
				exprOp -= i;
			return;
		}

		if constexpr (Inc)
			exprOp.rhsInc(i);
		else
			exprOp.rhsDec(i);

		while (i--)
		{
			if constexpr (Inc)
				++multiCount;
			else
				--multiCount;

			if (multiCount % rhsCols() == 0)
			{
				if constexpr (Inc)
				{
					exprOp.rhsDec(rhsCols());
					exprOp.lhsInc(rhsRows()); // Same as lhsCols
				}
				else
				{
					exprOp.rhsInc(rhsCols());
					exprOp.lhsDec(rhsRows());
				}
				break;
			}
		}
	}

public:

	inline MatrixExpr& operator++() noexcept
	{
		incrementSelf<true>(1);
		return *this;
	}

	inline MatrixExpr& operator+=(size_type i) noexcept
	{
		incrementSelf<true>(i);
		return *this;
	}

	inline MatrixExpr& operator--() noexcept
	{
		incrementSelf<false>(1);
		return *this;
	}

	inline MatrixExpr& operator-=(size_type i) noexcept
	{
		incrementSelf<false>(i);
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

	inline void lhsInc(size_type i)  noexcept { lit += i; }
	inline void lhsDec(size_type i)  noexcept { lit -= i; }
	inline void rhsInc(size_type i)  noexcept { rit += i; }
	inline void rhsDec(size_type i)  noexcept { rit -= i; }

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

	ThisType& operator+=(size_type i) noexcept
	{
		lhsInc(i);
		rhsInc(i);
		return *this;
	}

	ThisType& operator--() noexcept
	{
		lhsDec(1);
		rhsDec(1);
		return *this;
	}

	ThisType& operator-=(size_type i) noexcept
	{
		lhsDec(i);
		rhsDec(i);
		return *this;
	}
};
