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

template <class T, class Enable = void>
struct is_defined
{
	static constexpr bool value = false;
};

template <class T>
struct is_defined<T, std::enable_if_t<(sizeof(T))>>
{
	static constexpr bool value = true;
};

template<class Expr, 
	class = std::enable_if_t<is_defined<typename Expr::minor_const_iterator>::value>>
struct ItSelector
{
	using type = typename Expr::minor_const_iterator;
};

template<class Expr>
struct ItSelector
{
	using type = typename Expr::const_iterator;
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
	using It				= typename ItSelector<Expr>::type;
	

	using const_iterator	= impl::ExprIterator<ThisType&, MajorOrder>;
	using TmpMatrix			= std::shared_ptr<MatrixT<value_type, MajorOrder>>;
	using TmpIt				= typename MatrixT<value_type, MajorOrder>::iterator;

	ExprType	expr;
	It			it;
	TmpMatrix	tmpMat;
	TmpIt		tmpIt;
	bool		evaluated;

	TransposeOp(const Expr& expr) :
		expr{ expr },
		it{ expr.begin() }, // If expr is a matrix, this iterator is converted into a nonMajorOrder iterator
		tmpMat{ nullptr },
		tmpIt{},
		evaluated{ false }
	{}

	inline size_type lhsRows()		const noexcept { return expr.rows(); }
	inline size_type rhsRows()		const noexcept { return expr.rows(); }
	inline size_type rhsCols()		const noexcept { return expr.cols(); }
	inline size_type rows()			const noexcept { return expr.cols(); }
	inline size_type cols()			const noexcept { return expr.rows(); }
	inline size_type resultRows()	const noexcept { return expr.cols(); }
	inline size_type resultCols()	const noexcept { return expr.rows(); }

	inline void lhsInc(size_type i)  noexcept { if (IsMatrix) it += i; else tmpIt += i; }
	inline void lhsDec(size_type i)  noexcept { if (IsMatrix) it -= i; else tmpIt -= i; }
	inline void rhsInc(size_type i)  noexcept { if (IsMatrix) it += i; else tmpIt += i; }
	inline void rhsDec(size_type i)  noexcept { if (IsMatrix) it -= i; else tmpIt -= i; }

	Type evaluate() noexcept
	{
		if (IsMatrix)
			return *it;
		else
		{
			if (!evaluated)
				createTemporary();
			return *tmpIt;
		}
	}

	void createTemporary()
	{
		tmpMat	= TmpMatrix{ new MatrixT<value_type, MajorOrder> };
		tmpIt	= tmpMat->begin();

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