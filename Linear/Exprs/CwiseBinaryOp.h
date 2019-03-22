#pragma once
#include "ForwardDeclarations.h"

template<class Op, class Lhs, class Rhs>
struct Traits<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using value_type	= typename Lhs::value_type;
	using ExprType		= Lhs;
	enum
	{
		MajorOrder = ExprType::MajorOrder
	};
};

template<class Type, class Expr>
struct Traits<impl::Constant<Type, Expr>>
{
	using value_type	= Type;
	enum { MajorOrder	= Expr::MajorOrder };
};

template<class Op, class Lhs, class Rhs>
class CwiseBinaryOp : public MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
public:
	enum
	{
		MajorOrder = Traits<Lhs>::MajorOrder
	};

	using Base				= MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>;
	using ThisType			= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type			= typename Base::size_type;
	using value_type		= typename Traits<ThisType>::value_type;
	using Type				= typename Op::value_type;

	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

private:

	LhsT		lhs;
	RhsT		rhs;
	const Op	op;

public:

	CwiseBinaryOp(Op op, const Lhs& lhs, const Rhs& rhs) :
		op{ op },
		lhs{ lhs },
		rhs{ rhs }
	{}

	const Op& getOp() const { return op; }
	const Lhs& getLhs() const { return lhs; }
	const Rhs& getRhs() const { return rhs; }

	inline size_type rows()			const noexcept { return lhs.rows(); }
	inline size_type cols()			const noexcept { return rhs.cols(); }
	inline size_type resultRows()	const noexcept { return rows(); }
	inline size_type resultCols()	const noexcept { return cols(); }
};

// Different CwiseBinaryOp's
namespace impl
{
template<class Type>
class AddOp
{
public:
	using value_type = Type;

	enum
	{
		Packetable = PacketTraits<Type>::Packetable
	};

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit + rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return impl::padd(p1, p2);
	}
};

template<class Type>
class SubOp
{
public:
	using value_type = Type;

	enum
	{
		Packetable = PacketTraits<Type>::Packetable
	};

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit - rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return impl::psub(p1, p2);
	}
};

template<class Type>
class MulOp
{
public:
	using value_type = Type;

	enum
	{
		Packetable = PacketTraits<Type>::Packetable
	};

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit * rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return impl::pmul(p1, p2);
	}
};

template<class Type>
class DivOp
{
public:
	using value_type = Type;

	enum
	{
		Packetable = PacketTraits<Type>::Packetable && PacketTraits<Type>::HasDivide
	};

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit / rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return impl::pdiv(p1, p2);
	}
};

template<class Type>
class CwiseProductOp
{
public:
	using value_type = Type;

	enum
	{
		Packetable = PacketTraits<Type>::Packetable
	};

	CwiseProductOp() {}

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit * rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return impl::pmul(p1, p2);
	}
};

// Simple wrapper so we can perform 
// Matrix m; m operator(x) constant 
// (e.g.) m + 2; m / 2; etc
template<class Type, class Expr>
class Constant
{
public:
	using size_type			= typename Expr::size_type;
	using value_type		= Type;
	using ThisPacketTraits	= PacketTraits<Type>;
	using PacketType		= typename ThisPacketTraits::type;

	enum
	{
		IsExpr		= true,
		Packetable	= PacketTraits<Type>::Packetable,
		MajorOrder	= Expr::MajorOrder
	};

	Constant(const Type& t, const Expr& expr) :
		t{ t },
		expr{ expr },
		mem( ThisPacketTraits::Stride )
	{
		if constexpr (Packetable)
		{
			std::fill(mem.begin(), mem.end(), t);
			p = pload<PacketType, Type>(mem.data());
		}
	}

	const Type& evaluate() const { return t; }

	// TODO: Benchmark just using p or loading on each call
	PacketType packet() const
	{
		return p;
	}

	inline size_type rows()	const noexcept { return expr.rows(); }
	inline size_type cols()	const noexcept { return expr.cols(); }


private:

	// TODO: Remove packet & mem from struct when not in use
	// TODO: Will have to benchmark this impl vs. loads on each call to packet

	/*
	template<bool isPacketable>
	struct Helper
	{};

	struct Helper<false>
	{
		const Type& t;
		const Expr& expr;
	};

	struct Helper<true>
	{
		const Type& t;
		const Expr& expr;
		PacketType p;
		std::vector<Type> mem;
	};

	Helper<Packetable> vars;
	*/

	const Type& t;
	const Expr& expr;
	PacketType p;
	std::vector<Type> mem;
};

} // End impl::
