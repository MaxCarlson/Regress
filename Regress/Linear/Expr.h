#pragma once
#include <vector>

template<class Type>
class Matrix;

struct MatrixOpBase
{
	using size_type = int;
	static_assert(std::is_signed_v<size_type>);

	enum Op : size_type
	{
		ADD,
		SUB,
		MULTIPLY,
		DIV,
		CWISE_PRODUCT,
		NUMERICAL_ADD,
		NUMERICAL_SUB,
		NUMERICAL_MUL,
		NUMERICAL_DIV,
		DOT,
		TRANSPOSE
	};
};

// This is the basic expression that encompasses all other tyes of expressions
// It is also reasponsible for assigning the final values of expressions to the Matrix

template<class Iter, class Type>
class MatrixExpr
{
	using size_type		= typename MatrixOpBase::size_type;
	using MatrixType	= Matrix<Type>;
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
		if (exprOp.getOp() != MatrixOpBase::MULTIPLY)
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
		for (auto lIt = std::begin(to); lIt != std::end(to); ++lIt, ++rIt)
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
			if (exprOp.getOp() == MatrixOpBase::Op::MULTIPLY)
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

// A type of Matrix Expression that takes a left and right iterator 
// (to either a Matrix or another expression)

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


	MatBinExpr(LIt lit, RIt rit, size_type nlhsRows, size_type nrhsRows, size_type nrhsCols) noexcept :
		lit{ lit },
		rit{ rit },
		op{ 0 },
		nlhsRows{ nlhsRows },
		nrhsRows{ nrhsRows },
		nrhsCols{ nrhsCols }
	{}

	inline void lhsInc(size_type i)  noexcept { lit += i; }
	inline void lhsDec(size_type i)  noexcept { lit -= i; }
	inline void rhsInc(size_type i)  noexcept { rit += i; }
	inline void rhsDec(size_type i)  noexcept { rit -= i; }

	inline size_type lhsRows() const noexcept { return nlhsRows; }
	inline size_type rhsRows() const noexcept { return nrhsRows; }
	inline size_type rhsCols() const noexcept { return nrhsCols; }

	inline static constexpr size_type getOp() { return Op::type; }

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

// A Unary expression. Takes an iterator to only one Matrix or Expression

template<class It, class Op, class Type>
class MatUnaExpr
{
	using size_type = typename MatrixOpBase::size_type;
	using ThisType	= MatUnaExpr<It, Op, Type>;

	It it;
	const Op op;
	size_type nrows;
	size_type ncols;

public:

	MatUnaExpr(It it, size_type nrows, size_type ncols) noexcept :
		it{ it },
		op{ ncols },
		nrows{ nrows },
		ncols{ ncols }
	{}

	inline void lhsInc(size_type)  { throw std::runtime_error("Cannot inc lhs in Unary Expr"); }
	inline void lhsDec(size_type)  { throw std::runtime_error("Cannot dec lhs in Unary Expr"); }
	inline void rhsInc(size_type i)  noexcept { it += i; }
	inline void rhsDec(size_type i)  noexcept { it -= i; }

	inline static constexpr size_type getOp()  { return Op::type; }


	inline size_type lhsRows() const noexcept { return op.lhsRows(); }
	
	inline size_type rhsRows() const noexcept 
	{ 
		return Op::type == MatrixOpBase::TRANSPOSE ? nrows : ncols;
	}
	
	inline size_type rhsCols() const noexcept 
	{ 
		return Op::type == MatrixOpBase::TRANSPOSE ? nrows : ncols;
	}

	Type operator*() const noexcept
	{
		return op(it, nrows, ncols);
	}

	ThisType& operator++() noexcept
	{
		rhsInc(1);
		return *this;
	}

	ThisType& operator+=(size_type i) noexcept
	{
		rhsInc(i);
		return *this;
	}

	ThisType& operator--() noexcept
	{
		rhsDec(1);
		return *this;
	}

	ThisType& operator-=(size_type i) noexcept
	{
		rhsDec(i);
		return *this;
	}
};


// Different Matrix expression operations

// Types of Binary Expressions

template<class Type>
class MatrixAddOp : public MatrixOpBase
{
public:
	static constexpr Op type = ADD;

	inline MatrixAddOp(size_type) {}

	template<class Lit, class Rit>
	inline Type operator()(Lit lit, Rit rit, size_type rhsRows, size_type rhsCols) const noexcept
	{
		return *lit + *rit;
	}
};

template<class Type>
class MatrixSubOp : public MatrixOpBase
{
public:
	static constexpr Op type = SUB;

	inline MatrixSubOp(size_type) {}

	template<class Lit, class Rit>
	inline Type operator()(Lit lit, Rit rit, size_type rhsRows, size_type rhsCols) const noexcept
	{
		return *lit - *rit;
	}
};

template<class Type>
class MatrixMultOp : public MatrixOpBase
{
public:
	static constexpr Op type = MULTIPLY;

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

// This expression is handled by calling the cwiseProduct() function
// from a matrix instance itself since rest of operators are unclear
// ex Matrix m, n; m.cwiseProduct(n);
template<class Type>
class MatrixCwiseProductOp : public MatrixOpBase
{
public:
	static constexpr Op type = CWISE_PRODUCT;

	inline MatrixCwiseProductOp(size_type) {}

	template<class LIt, class RIt>
	inline Type operator()(LIt lit, RIt rit, size_type, size_type) const noexcept
	{
		return *lit * *rit;
	}
};

// A wrapper class for numerical operations so we can conform to
// the binary expressions interface
template<class Num>
class NumericIteratorWrapper
{
	const Num& num;
	static_assert(std::is_arithmetic_v<Num>);
public:
	using size_type = typename MatrixOpBase::size_type;
	using ThisType = NumericIteratorWrapper<Num>;

	inline NumericIteratorWrapper(const Num& num) :
		num{ num }
	{}

	inline ThisType& operator++()			noexcept { return *this; }
	inline ThisType& operator--()			noexcept { return *this; }
	inline ThisType& operator+=(size_type)	noexcept { return *this; }
	inline ThisType& operator-=(size_type)	noexcept { return *this; }

	inline const Num& operator*()  const noexcept { return num; }
	inline const Num* operator->() const noexcept { return &num; }
};

//
// Binary expressions involving numerical values wrapped in NumericIteratorWrapper
//

template<class Type>
class MatrixAddNumericOp : public MatrixOpBase
{
public:
	static constexpr Op type = NUMERICAL_ADD;

	inline MatrixAddNumericOp(size_type) {}

	// In this case one of LIt/RIt is going to be a ConstantIteratorWrapper
	template<class LIt, class RIt>
	inline Type operator()(LIt lit, RIt rit, size_type, size_type) const noexcept
	{
		return *lit + *rit;
	}
};

template<class Type>
class MatrixSubNumericOp : public MatrixOpBase
{
public:
	static constexpr Op type = NUMERICAL_SUB;

	inline MatrixSubNumericOp(size_type) {}

	// In this case one of LIt/RIt is going to be a ConstantIteratorWrapper
	template<class LIt, class RIt>
	inline Type operator()(LIt lit, RIt rit, size_type, size_type) const noexcept
	{
		return *lit - *rit;
	}
};

template<class Type>
class MatrixMulNumericOp : public MatrixOpBase
{
public:
	static constexpr Op type = NUMERICAL_MUL;

	inline MatrixMulNumericOp(size_type) {}

	// In this case one of LIt/RIt is going to be a ConstantIteratorWrapper
	template<class LIt, class RIt>
	inline Type operator()(LIt lit, RIt rit, size_type, size_type) const noexcept
	{
		return *lit * *rit;
	}
};

template<class Type>
class MatrixDivNumericOp : public MatrixOpBase
{
public:
	static constexpr Op type = NUMERICAL_DIV;

	inline MatrixDivNumericOp(size_type) {}

	// In this case one of LIt/RIt is going to be a ConstantIteratorWrapper
	template<class LIt, class RIt>
	inline Type operator()(LIt lit, RIt rit, size_type, size_type) const noexcept
	{
		return *lit / *rit;
	}
};

//
// Types of Unary Expressions
//

template<class Type>
class MatrixTransposeOp : public MatrixOpBase
{
public:
	static constexpr Op type = TRANSPOSE;
private:
	const size_type nlhsRows;
public:

	inline MatrixTransposeOp(size_type nlhsRows) :
		nlhsRows{ nlhsRows }
	{}

	inline size_type lhsRows() const noexcept { return nlhsRows; }

	template<class It>
	inline Type operator()(It it, size_type, size_type) const noexcept
	{
		return *it;
	}
};