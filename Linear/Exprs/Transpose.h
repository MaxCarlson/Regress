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
	using const_iterator	= impl::ExprIterator<ThisType&, MajorOrder>;
	using TmpMatrix			= std::shared_ptr<MatrixT<value_type, MajorOrder>>;

	ExprType		expr;
	const_iterator	it;
	TmpMatrix		tmpMat;

	TransposeOp(const Expr& expr) :
		expr{ expr },
		it{ expr.begin() }, 
		tmpMat{ nullptr }
	{}

	Type evaluate() const noexcept
	{
		return ;
	}
};