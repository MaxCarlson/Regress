#pragma once
#include "ForwardDeclarations.h"

template<class Type, bool MOrder>
class ExprAnalyzer;

template<class Op, class Lhs, class Rhs>
class BinaryOp
{
	const Lhs& lhs;
	const Rhs& rhs;
	Op op;

public:

	BinaryOp(Op op, const Lhs& lhs, const Rhs& rhs) :
		op{ op },
		lhs{ lhs },
		rhs{ rhs }
	{}
};

template<class Type>
class MulOp
{
public:
	MulOp(Type){}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit, int lhsRows, int rhsRows, int rhsCols) const
	{

	}
};

template<class Derived>
class MatrixBase
{
public:
	using ThisType		= MatrixBase<Derived>;
	using value_type	= typename Traits<Derived>::value_type;
	using size_type		= int;
	static constexpr bool MajorOrder = Traits<Derived>::MajorOrder;


	inline void analyzeExpr(ExprAnalyzer<value_type, MajorOrder>&, size_type pOp) {}
	void assign(Derived& to) const {}

	template<class OtherDerived>
	const BinaryOp<MulOp<value_type>, Derived, OtherDerived>
		operator/(const OtherDerived& other) const
	{
		return BinaryOp{ MulOp<value_type>{0} , static_cast<const Derived&>(*this), other };
	}
};