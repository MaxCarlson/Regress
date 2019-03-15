#pragma once
#include "ForwardDeclarations.h"
#include "ExprIterator.h"

template<class Expr>
struct Traits<TransposeOp<Expr>>
{
	using ExprType		= Expr;
	using value_type	= typename Expr::value_type;
	static constexpr bool MajorOrder	= ExprType::MajorOrder;
};

template<class Expr>
class TransposeOp : public MatrixBase<TransposeOp<Expr>>
{
public:
	using ThisType			= TransposeOp<Expr>;

	static constexpr bool MajorOrder	= Traits<ThisType>::MajorOrder;
	static constexpr bool IsExpr		= true;
	static constexpr bool IsMatrix		= !Expr::IsExpr;
	
	using Base				= MatrixBase<TransposeOp<Expr>>;
	using ExprType			= typename RefSelector<Expr>::type;
	using size_type			= typename Base::size_type;
	using value_type		= typename Traits<ThisType>::value_type;
	using Type				= value_type;
	using It				= std::conditional_t<IsMatrix, 
		typename Expr::minor_const_iterator, 
		typename Expr::const_iterator>;


	using const_iterator	= impl::ExprIterator<ThisType&, MajorOrder>;
	using TmpMatrix			= std::shared_ptr<MatrixT<value_type, MajorOrder>>;

	ExprType	expr;
	It			it;
	TmpMatrix	tmpMat;

	TransposeOp(const Expr& expr) :
		expr{ expr },
		it{ expr.begin() }, 
		tmpMat{ nullptr }
	{}

	inline size_type lhsRows()		const noexcept { return expr.rows(); }
	inline size_type rhsRows()		const noexcept { return expr.rows(); }
	inline size_type rhsCols()		const noexcept { return expr.cols(); }
	inline size_type rows()			const noexcept { return lhsRows(); }
	inline size_type cols()			const noexcept { return rhsCols(); }
	inline size_type resultRows()	const noexcept { return lhsRows(); }
	inline size_type resultCols()	const noexcept { return rhsCols(); }

	inline void lhsInc(size_type i)  noexcept { it += i; }
	inline void lhsDec(size_type i)  noexcept { it -= i; }
	inline void rhsInc(size_type i)  noexcept { it += i; }
	inline void rhsDec(size_type i)  noexcept { it -= i; }

	Type evaluate() const noexcept
	{
		return 0;
	}

	ThisType& operator++() noexcept
	{
		lhsInc(1);
		return *this;
	}

	ThisType& operator+=(size_type i) noexcept
	{
		lhsInc(i);
		return *this;
	}

	ThisType& operator--() noexcept
	{
		lhsDec(1);
		return *this;
	}

	ThisType& operator-=(size_type i) noexcept
	{
		lhsDec(i);
		return *this;
	}

	const_iterator begin() noexcept { return const_iterator{ this }; }
};