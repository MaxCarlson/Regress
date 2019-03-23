#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Dest>
void assignmentLoopCoeff(Dest& dest)
{
	// TODO: Index based assignement
	// TODO: Correct major order assignement
	// TODO: OpenMp
	for (int i = 0; i < dest.outerSize(); ++i)
		for (int j = 0; j < dest.innerSize(); ++j)
			dest.assignOuterInner(i, j);
}

template<class Dest>
void assignmentLoopCoeffPacket(Dest& dest)
{
	using size_type		= typename Dest::size_type;
	using Traits		= PacketTraits<typename Dest::value_type>;
	using PacketType	= typename Traits::type;

	enum
	{
		Stride = Traits::Stride,
	};

	size_type outer = dest.outerSize();
	size_type inner = dest.innerSize();

	size_type maxInner = inner - inner % Stride;

	// TODO: Detect non-aligned memory / do non-aligned memory up to aligned block

	// Do aligned ops
	for (size_type i = 0; i < outer; ++i)
		for (size_type j = 0; j < maxInner; j += Stride)
			dest.assignPacketOuterInner<PacketType>(i, j);

	// Do leftovers
	for (size_type i = 0; i < outer; ++i)
		for (size_type j = maxInner; j < inner; ++j)
			dest.assignOuterInner(i, j);
}

template<class... Args>
struct Assignment {};

template<class DestImpl, class ExprImpl>
struct AssignmentKernel
{
	using size_type = typename DestImpl::size_type;
	using value_type = typename DestImpl::value_type;
	enum
	{
		MajorOrder = DestImpl::MajorOrder
	};

	AssignmentKernel(DestImpl& destImpl, ExprImpl& exprImpl) :
		destImpl{ destImpl },
		exprImpl{ exprImpl }
	{}

	size_type outerSize() const { return MajorOrder ? destImpl.cols() : destImpl.rows(); }
	size_type innerSize() const { return MajorOrder ? destImpl.rows() : destImpl.cols(); }

	static constexpr size_type rowIndex(size_type outer, size_type inner)
	{
		return MajorOrder ? inner : outer;
	}

	static constexpr size_type colIndex(size_type outer, size_type inner)
	{
		return MajorOrder ? outer : inner;
	}

	void assignOuterInner(size_type outer, size_type inner)
	{
		auto row = rowIndex(outer, inner);
		auto col = colIndex(outer, inner);
		destImpl.evaluateRef(row, col) = exprImpl.evaluate(row, col);
	}

	template<class Packet>
	void assignPacketOuterInner(size_type outer, size_type inner)
	{
		auto row = rowIndex(outer, inner);
		auto col = colIndex(outer, inner);
		destImpl.template writePacket<Packet>(row, col, exprImpl.template packet<Packet>(row, col));
	}

private:
	DestImpl& destImpl;
	ExprImpl& exprImpl;
};

template<class Dest, class ExprEval>
struct ActualDest
{
	using DestType = std::conditional_t<ExprEval::MajorOrder != Dest::MajorOrder,
		TransposeEvaluator<Dest>, Evaluator<Dest>>;
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;

	enum
	{
		MajorOrder = Dest::MajorOrder
	};


	ActualDest(Dest& dest) :
		dest{ dest }
	{}

	value_type& evaluateRef(size_type row, size_type col)
	{
		return dest.evaluateRef(row, col);
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p) 
	{
		dest.template writePacket<Packet>(row, col, p);
	}

	void set(Dest&& src)
	{
		dest.set(std::move(src));
	}

	size_type rows() const noexcept { return dest.rows(); }
	size_type cols() const noexcept { return dest.cols(); }
	//size_type outer() const noexcept { return MajorOrder ? cols() : rows(); }
	//size_type inner() const noexcept { return MajorOrder ? rows() : cols(); }

private:
	DestType dest;
};

template<class Dest, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;
		
		// Note:  No need to resize when we're just moving the temporary in below
		//dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	actDest{ dest };
		
		actDest.set(exprE.moveMatrix());
	}
};

template<class Dest, class Op, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, CwiseBinaryOp<Op, Lhs, Rhs>, Type>
{
	using ExprType = CwiseBinaryOp<Op, Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };
		AssignmentKernel			kernel{ destE, exprE };

		// TODO: Specialize AssignmentKernel for this stuff
		if constexpr(ExprEval::Packetable)
			assignmentLoopCoeffPacket(kernel);
		else
			assignmentLoopCoeff(kernel);
	}
};

template<class Dest, class Expr, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, TransposeOp<Expr>, Type>
{
	using ExprType = TransposeOp<Expr>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };
		AssignmentKernel			kernel{ destE, exprE };

		// TODO: Specialize AssignmentKernel for this stuff
		if constexpr (ExprEval::Packetable)
			assignmentLoopCoeffPacket(kernel);
		else
			assignmentLoopCoeff(kernel);
	}
};


} // End impl::
