#pragma once
#include "ForwardDeclarations.h"

template<class Lhs, class Rhs>
struct Traits<ProductOp<Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using ExprType = Lhs;
	static constexpr bool MajorOrder = ExprType::MajorOrder;
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
// n*m = | (C*Z) + (D*V) | (C*Y) + (D*U) | (C*X) + (D*T) | (C*W) + (D*S) |
//       +---------------+---------------+---------------+---------------+
//       | (E*Z) + (F*V) | (E*Y) + (F*U) | (E*X) + (F*T) | (E*W) + (F*S) |
//       +---------------+---------------+---------------+---------------+
//

template<class Lhs, class Rhs>
class ProductOp : public MatrixBase<ProductOp<Lhs, Rhs>>
{
public:
	enum
	{
		// TODO: Lhs/Rhs MajorOrders
		MajorOrder = Traits<Lhs>::MajorOrder
	};

	using Base				= MatrixBase<ProductOp<Lhs, Rhs>>;
	using ThisType			= ProductOp<Lhs, Rhs>;
	using size_type			= typename Base::size_type;
	using Type				= typename Traits<ThisType>::value_type;

private:
	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

	LhsT lhs;
	RhsT rhs;
public:

	ProductOp(const Lhs& lhsa, const Rhs& rhsa) :
		lhs{ lhsa },
		rhs{ rhsa }
	{}

	const Lhs& getLhs() const { return lhs; }
	const Rhs& getRhs() const { return rhs; }

	inline size_type rows()		const noexcept { return lhs.rows(); }
	inline size_type cols()		const noexcept { return rhs.cols(); }
	inline size_type resultRows() const noexcept { return lhs.rows(); }
	inline size_type resultCols() const noexcept { return rhs.cols(); }
};