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
		
		const size_type outer = kernel.outerSize();
		const size_type inner = kernel.innerSize();

		for (size_type i = 0; i < outer; ++i)
			for (size_type j = 0; j < inner; ++j)
				kernel.assignOuterInner(i, j);
	}
};

template<class Kernel>
struct AssignmentLoop<Kernel, LoopTraits::Packet>
{
	using size_type = typename Kernel::size_type;
	using Traits = PacketTraits<typename Kernel::value_type>;
	using PacketType = typename Traits::type;

	enum
	{
		Stride = Traits::Stride,
	};

	static void run(Kernel& kernel)
	{
		const size_type outer		= kernel.outerSize();
		const size_type inner		= kernel.innerSize();
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

		size_type size = kernel.size();
		//for(size_type idx = 0; idx < size; ++idx)
		//	kernel.
	}
};

template<class... Args>
struct Assignment {};

template<class DestImpl, class ExprImpl>
struct AssignmentKernel
{
	using ThisType		= AssignmentKernel<DestImpl, ExprImpl>;
	using size_type		= typename DestImpl::size_type;
	using value_type	= typename DestImpl::value_type;
	enum
	{
		MajorOrder	= DestImpl::MajorOrder,
		Packetable	= std::is_same_v<value_type, typename ExprImpl::value_type>
			&& DestImpl::Packetable && ExprImpl::Packetable,
		Indexable	= Packetable && DestImpl::Indexable && ExprImpl::Indexable
	};

	static constexpr LoopTraits LoopType = LoopTraits::Packet;// Indexable ? LoopTraits::Index
		//: Packetable ? LoopTraits::Packet : LoopTraits::Default;

	AssignmentKernel(DestImpl& destImpl, ExprImpl& exprImpl) :
		destImpl{ destImpl },
		exprImpl{ exprImpl }
	{}

	size_type size()		const { return destImpl.size(); }
	size_type outerSize()	const { return MajorOrder ? destImpl.cols() : destImpl.rows(); }
	size_type innerSize()	const { return MajorOrder ? destImpl.rows() : destImpl.cols(); }

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

	template<class Packet>
	void assignIndex(size_type index)
	{
		destImpl.template writeIndex<Packet>(index, exprImpl.template index<Packet>(index));
	}

private:
	DestImpl& destImpl;
	ExprImpl& exprImpl;
};

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
	//size_type outer() const noexcept { return MajorOrder ? cols() : rows(); }
	//size_type inner() const noexcept { return MajorOrder ? rows() : cols(); }

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

private:
	DestEval dest;
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

		kernel.run();
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

		kernel.run();
	}
};


} // End impl::
