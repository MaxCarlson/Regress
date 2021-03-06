#pragma once
#include "ForwardDeclarations.h"

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

	enum
	{
		MajorOrder = Traits<ThisType>::MajorOrder
	};
	
	using Base				= MatrixBase<TransposeOp<Expr>>;
	using size_type			= typename Base::size_type;
	using value_type		= typename Traits<ThisType>::value_type;
	using ExprT				= typename RefSelector<Expr>::type;

private:

	ExprT expr;

public:

	TransposeOp(const Expr& expr) : 
		expr{ expr }
	{}

	const Expr& getExpr() const { return expr; }

	inline size_type rows()			const noexcept { return expr.cols(); }
	inline size_type cols()			const noexcept { return expr.rows(); }
	inline size_type resultRows()	const noexcept { return rows(); }
	inline size_type resultCols()	const noexcept { return cols(); }
};