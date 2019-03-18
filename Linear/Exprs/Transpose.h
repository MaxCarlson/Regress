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

namespace impl
{
template<class, class = std::void_t<>>
struct has_minor_iterator_member
{
	using type = std::false_type;
};
// specialization recognizes types that do have a nested ::minor_const_iterator member:
template<class T>
struct has_minor_iterator_member<T, std::void_t<typename T::minor_const_iterator>>
{
	using type = std::true_type;
};

template<class Expr, class>
struct ItSelector
{
	using type = void;
};

template<class Expr>
struct ItSelector<Expr, std::true_type>
{
	using type = typename Expr::minor_const_iterator;
};

template<class Expr>
struct ItSelector<Expr, std::false_type>
{
	using type = typename Expr::const_iterator;
};
} // End impl::

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
	using It				= typename impl::ItSelector<Expr, 
		typename impl::has_minor_iterator_member<Expr>::type>::type;

	using const_iterator	= impl::ExprIterator<ThisType, MajorOrder>;
	using TmpMatrix			= std::shared_ptr<MatrixT<value_type, MajorOrder>>;
	using TmpIt				= typename MatrixT<value_type, MajorOrder>::iterator;

	ExprType	expr;
	It			it;
	TmpMatrix	tmpMat;
	TmpIt		tmpIt;
	bool		evaluated;

	TransposeOp(const Expr& expr) : 
		expr{ expr },
		it{ this->expr.begin() }, // If expr is a matrix, this iterator is converted into a nonMajorOrder iterator
		tmpMat{ nullptr },
		tmpIt{},
		evaluated{ false }
	{}


	inline size_type rows()			const noexcept { return expr.cols(); }
	inline size_type cols()			const noexcept { return expr.rows(); }
	inline size_type resultRows()	const noexcept { return expr.cols(); }
	inline size_type resultCols()	const noexcept { return expr.rows(); }

	inline void lhsInc(size_type i)  noexcept { if (IsMatrix) it += i; else tmpIt += i; }
	inline void lhsDec(size_type i)  noexcept { if (IsMatrix) it -= i; else tmpIt -= i; }
	inline void rhsInc(size_type i)  noexcept { if (IsMatrix) it += i; else tmpIt += i; }
	inline void rhsDec(size_type i)  noexcept { if (IsMatrix) it -= i; else tmpIt -= i; }

	Type evaluate() const noexcept
	{
		if constexpr (IsMatrix)
			return *it;
		else
			return *tmpIt;
	}

	/*
	void analyze()
	{
		expr.analyze();
		if (!IsMatrix && !evaluated)
			createTemporary();
	}
	*/

	void createTemporary()
	{
		tmpMat = TmpMatrix{ new MatrixT<value_type, MajorOrder>() };

		if constexpr (IsMatrix)
			tmpMat->resize(cols(), rows());
		else
			tmpMat->resize(resultCols(), resultRows());
		
		tmpIt = tmpMat->begin();

		for (auto tIt = tmpMat->m_begin(); 
			tIt != tmpMat->m_end(); ++tIt, ++it)
			*tIt = *it;

		evaluated = true;
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