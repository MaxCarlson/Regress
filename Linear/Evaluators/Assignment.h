#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

enum class LoopTraits { Default, Packet, Index};

template<class Kernel, LoopTraits trait>
struct AssignmentLoop
{};

template<class Kernel>
struct AssignmentLoop<Kernel, LoopTraits::Default>
{
	static void run(Kernel& kernel)
	{
		using size_type	= typename Kernel::size_type;
		
		const size_type outer = kernel.outer();
		const size_type inner = kernel.inner();

		for (size_type i = 0; i < outer; ++i)
			for (size_type j = 0; j < inner; ++j)
				kernel.assignOuterInner(i, j);
	}
};

template<class Kernel>
struct AssignmentLoop<Kernel, LoopTraits::Packet>
{
	using size_type		= typename Kernel::size_type;
	using Traits		= PacketTraits<typename Kernel::value_type>;
	using PacketType	= typename Traits::type;

	enum
	{
		Stride = Traits::Stride,
	};

	static void run(Kernel& kernel)
	{
		const size_type outer		= kernel.outer();
		const size_type inner		= kernel.inner();
		const size_type maxInner	= inner - inner % Stride;

		// TODO: Detect non-aligned memory / do non-aligned memory up to aligned block

		// Do aligned ops
		for (size_type i = 0; i < outer; ++i)
			for (size_type j = 0; j < maxInner; j += Stride)
				kernel.assignPacketOuterInner<PacketType>(i, j);

		// Do leftovers
		for (size_type i = 0; i < outer; ++i)
			for (size_type j = maxInner; j < inner; ++j)
				kernel.assignOuterInner(i, j);
	}
};

template<class Kernel>
struct AssignmentLoop<Kernel, LoopTraits::Index>
{
	using size_type		= typename Kernel::size_type;
	using Traits		= PacketTraits<typename Kernel::value_type>;
	using PacketType	= typename Traits::type;

	enum
	{
		Stride = Traits::Stride,
	};

	static void run(Kernel& kernel)
	{
		// TODO: Detect non-aligned memory

		const size_type size			= kernel.size();
		const size_type maxVectorized	= size - size % Stride;

		for (size_type idx = 0; idx < maxVectorized; idx += Stride)
			kernel.assignPacket<PacketType>(idx);

		for(size_type idx = maxVectorized; idx < size; ++idx)
			kernel.assignIndex(idx);
	}
};

template<class... Args>
struct Assignment {};

template<class DestImpl, class ExprImpl, class Func>
struct AssignmentKernel
{
	using ThisType		= AssignmentKernel<DestImpl, ExprImpl, Func>;
	using size_type		= typename DestImpl::size_type;
	using value_type	= typename DestImpl::value_type;
	enum
	{
		MajorOrder	= DestImpl::MajorOrder,
		Packetable	= std::is_same_v<value_type, typename ExprImpl::value_type>
			&& DestImpl::Packetable && ExprImpl::Packetable,
		Indexable	= Packetable && DestImpl::Indexable && ExprImpl::Indexable
	};

	static constexpr LoopTraits LoopType = Indexable ? LoopTraits::Index
		: Packetable ? LoopTraits::Packet : LoopTraits::Default;

	AssignmentKernel(DestImpl& destImpl, ExprImpl& exprImpl, const Func& func) :
		destImpl{ destImpl },
		exprImpl{ exprImpl },
		func{ func }
	{}

	size_type size()	const noexcept { return destImpl.size(); }
	size_type outer()	const noexcept { return destImpl.outer(); }
	size_type inner()	const noexcept { return destImpl.inner(); }

	void run()
	{
		AssignmentLoop<ThisType, LoopType>::run(*this);
	}

	static constexpr size_type rowIndex(size_type outer, size_type inner)
	{
		return MajorOrder ? inner : outer;
	}

	static constexpr size_type colIndex(size_type outer, size_type inner)
	{
		return MajorOrder ? outer : inner;
	}

	void assignIndex(size_type idx)
	{
		func(destImpl.evaluateRef(idx), exprImpl.evaluate(idx));
	}

	void assignOuterInner(size_type outer, size_type inner)
	{
		auto row = rowIndex(outer, inner);
		auto col = colIndex(outer, inner);
		func(destImpl.evaluateRef(row, col), exprImpl.evaluate(row, col));
	}

	template<class Packet>
	void assignPacketOuterInner(size_type outer, size_type inner)
	{
		auto row = rowIndex(outer, inner);
		auto col = colIndex(outer, inner);
		func(&destImpl.evaluateRef(row, col), exprImpl.template packet<Packet>(row, col));
	}

	template<class Packet>
	void assignPacket(size_type index)
	{
		func(&destImpl.evaluateRef(index), exprImpl.template packet<Packet>(index));
	}

private:
	const Func&		func;
	DestImpl&		destImpl;
	ExprImpl&		exprImpl;
};

// TODO: Right now this serves no purpose but should in the future when
// we need to transpose things to get the proper result
// (DestEval can become TransposeEvaluator<Dest> instead if needed)
template<class Dest, class ExprEval>
struct ActualDest
{
	using DestEval		= Evaluator<Dest>;
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;

	enum
	{
		MajorOrder	= DestEval::MajorOrder,
		Packetable	= DestEval::Packetable,
		Indexable	= DestEval::Indexable
	};

	ActualDest(Dest& dest) :
		dest{ dest }
	{}

	size_type size() const noexcept { return dest.size(); }
	size_type rows() const noexcept { return dest.rows(); }
	size_type cols() const noexcept { return dest.cols(); }
	size_type outer() const noexcept { return dest.outer(); }
	size_type inner() const noexcept { return dest.inner(); }

	value_type& evaluateRef(size_type row, size_type col)
	{
		return dest.evaluateRef(row, col);
	}

	value_type& evaluateRef(size_type idx)
	{
		return dest.evaluateRef(idx);
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p) 
	{
		dest.template writePacket<Packet>(row, col, p);
	}

	template<class Packet>
	void writePacket(size_type idx, const Packet& p)
	{
		dest.template writePacket<Packet>(idx, p);
	}

	void set(Dest&& src)
	{
		dest.set(std::move(src));
	}

private:
	DestEval dest;
};

template<class Dest, class Lhs, class Rhs, class Type, class Func> 
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type, Func>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr, const Func& func)
	{
		using ExprEval = Evaluator<ExprType>;
		
		// Note:  No need to resize when we're just moving the temporary in below
		//dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	actDest{ dest };
		
		actDest.set(exprE.moveMatrix());
	}
};

template<class Dest, class Op, class Lhs, class Rhs, class Type, class Func>
struct Assignment<Dest, CwiseBinaryOp<Op, Lhs, Rhs>, Type, Func>
{
	using ExprType = CwiseBinaryOp<Op, Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr, const Func& func)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };
		AssignmentKernel			kernel{ destE, exprE, func };

		kernel.run();
	}
};

template<class Dest, class Expr, class Type, class Func> 
struct Assignment<Dest, TransposeOp<Expr>, Type, Func>
{
	using ExprType = TransposeOp<Expr>;

	inline static void run(Dest& dest, const ExprType& expr, const Func& func)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };
		AssignmentKernel			kernel{ destE, exprE, func };

		kernel.run();
	}
};

template<class Dest, class Type, bool MajorOrder, class Func>
struct Assignment<Dest, Matrix<Type, MajorOrder>, Type, Func>
{
	using MatrixType	= Matrix<Type, MajorOrder>;
	//using ExprType		= CwiseBinaryOp<typename Func::OpType, Dest, MatrixType>;

	// TODO: This needs to be specialized for *= as it won't work as is
	// 
	inline static void run(Dest& dest, const MatrixType& matrix, const Func& func)
	{
		using ExprEval = Evaluator<MatrixType>;

		//ExprType expr{ typename Func::OpType{}, dest, matrix };
		

		ExprEval					exprE{ matrix };
		ActualDest<Dest, ExprEval>	destE{ dest };
		AssignmentKernel			kernel{ destE, exprE, func };

		kernel.run();
	}
};

} // End impl::
