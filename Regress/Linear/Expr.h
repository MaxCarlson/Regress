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
		ContainerType&	cont;

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

		col_iterator_base& operator+=(size_type i)
		{
			it += i;
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

		bool operator==(const col_iterator_base& other) const
		{
			return it == other.it;
		}

		bool operator!=(const col_iterator_base& other) const
		{
			return !(*this == other);
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

	struct col_iterator : public col_iterator_base<false>
	{
		using MyBase = col_iterator_base<false>;
		using ItType = typename MyBase::ItType;
		using ContainerType = typename MyBase::ContainerType;

		col_iterator(ItType it, ContainerType& cont) :
			MyBase{it, cont}
		{}
	};

	col_const_iterator begin() const { return col_const_iterator{ vals.cbegin(), *this }; }
	col_const_iterator end()   const { return col_const_iterator{ vals.cend(),   *this }; }
	col_iterator begin()  { return col_iterator{ vals.begin(), *this }; }
	col_iterator end()   { return col_iterator{ vals.end(),   *this }; }
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
		while (--rhsRows >= 0) // TODO: I this code is wrong, we need rit += rhsCols 
		{
			result += *lit * *rit;
			++lit;
			rit += rhsCols;
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
	inline size_type lhsRows()	const noexcept { return exprOp.lhsRows(); }
	inline size_type rhsRows()	const noexcept { return exprOp.rhsRows(); }
	inline size_type lhsCols()	const noexcept { return exprOp.lhsCols(); }
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
				exprOp.lhsInc(lhsCols());
			}
		}
		else
			++exprOp;
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

		const MatrixExpr* expr;
		Iter exprOp;

	public:
		const_iterator(const MatrixExpr& expr) noexcept :
			expr{ &expr },
			exprOp{ expr.getExprOp() }
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
			++expr;
			return *this;
		}
	};

	const_iterator begin() const noexcept { return const_iterator{ *this }; }
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
		op{ 0 }
	{}

	inline void lhsInc(int i)  noexcept { lit += i; }
	inline void lhsDec(int i)  noexcept { lit -= i; }
	inline void rhsInc(int i)  noexcept { rit += i; }
	inline void rhsDec(int i)  noexcept { rit -= i; }

	inline size_type lhsRows() const noexcept { return nlhsRows; }
	inline size_type rhsRows() const noexcept { return nrhsRows; }
	inline size_type lhsCols() const noexcept { return nlhsCols; }
	inline size_type rhsCols() const noexcept { return nrhsCols; }


	Type operator*() const noexcept
	{
		return op(lit, rit, nrhsRows, nrhsCols);
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


