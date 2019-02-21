#pragma once
#include <vector>

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)

class ExprAnalyzer
{
public:
	int nrows;
	int ncols;

	// TODO: Could hold OP order in here if needed?

//public:
	ExprAnalyzer() :
		nrows{},
		ncols{}
	{
	}

};


struct ExprEvaluated
{
	bool done = false;
	int nrows;
	int ncols;
};

template<class E>
class BaseExpr
{
public:
	ExprEvaluated eval;

	//double operator[](int i)		const { return static_cast<E const&>(*this)[i]; }

	//double operator()(int r, int c) const { return static_cast<E const&>(*this)(r, c); }
	size_t size()					const { return static_cast<E const&>(*this).size(); }

	int rows() const { return static_cast<E const&>(*this).rows(); }
	int cols() const { return static_cast<E const&>(*this).cols(); }

	//using value_type = typename E::value_type;

	void analyzeLeft(ExprAnalyzer& ea) const { return static_cast<E const&>(*this).analyzeLeft(ea);}
	void analyzeRight(ExprAnalyzer& ea) const { return static_cast<E const&>(*this).analyzeRight(ea); }

	double evalExpr(int row, int col, int idx) const { return static_cast<E const&>(*this).evalExpr(row, col, idx); }

	//int totalRows() const { return static_cast<E const&>(*this).totalRows(); }
	//int totalCols() const { return static_cast<E const&>(*this).totalCols(); }

};

template<class E>
struct MatrixTBase : public BaseExpr<E>
{ };

// TODO: Check for constants!
template<class Type>
struct IsLeaf
{
	static constexpr bool value = std::is_base_of_v<MatrixTBase<Type>, Type>;
};

template<class Type>
class MatrixT : public MatrixTBase<MatrixT<Type>>
{
	int nrows;
	int ncols;
	std::vector<Type> vals;

public:
	using value_type	= Type;
	using MyType		= MatrixT<Type>;

	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
		nrows{ static_cast<int>(m.size()) },
		ncols{ static_cast<int>(m.begin()->size()) },
		vals(nrows * ncols)
	{
		int i = 0;
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
				vals[i++] = *jt;
	}

	template<class E>
	MatrixT(const BaseExpr<E>& expr) :
		nrows{},
		ncols{},
		vals{}
		//nrows{expr.totalRows()},
		//ncols{expr.totalCols()},
		//vals(nrows * ncols)
	{
		ExprAnalyzer ea;
		expr.analyzeLeft(ea);

		int idx = 0;
		for(int i = 0; i < nrows; ++i)
			for (int j = 0; j < ncols; ++j, ++idx)
			{
				vals[idx] = expr.evalExpr(i, j, idx);
			}
	}



	int size() const { return vals.size(); }
	int rows() const { return nrows; }
	int cols() const { return ncols; }
	const value_type& operator[](int idx) const { return vals[idx]; }


	void analyzeLeft(ExprAnalyzer& ea) const {}
	void analyzeRight(ExprAnalyzer& ea) const {}
	const value_type& evalExpr(int row, int col, int idx) const { return vals[idx]; }
};

template<class A, class B>
class AddExpr : public BaseExpr<AddExpr<A, B>>
{
	const A& v1;
	const B& v2;

public:

	using value_type = typename MatrixT<A>::value_type;
	static constexpr bool left_leaf  = IsLeaf<A>::value;
	static constexpr bool right_leaf = IsLeaf<B>::value;

	AddExpr(const A& v1, const B& v2) :
		v1{v1},
		v2{v2}
	{}

	void analyzeLeft(ExprAnalyzer& ea) const 
	{
		if constexpr (!left_leaf)
			v1.analyzeLeft(ea);
		else
		{
			ea.nrows = v1.rows();
			ea.ncols = v1.cols();
		}
	}

	void analyzeRight(ExprAnalyzer& ea) const {}

	double evalExpr(int row, int col, int idx) const
	{
		return v1.evalExpr(row, col, idx) + v2.evalExpr(row, col, idx);
	}
	
};

template<class A, class B>
AddExpr<MatrixT<A>, MatrixT<B>> operator+(const MatrixT<A>& a, const MatrixT<B>& b) {
	return AddExpr<MatrixT<A>, MatrixT<B>>(a, b);
}

template<class A, class C>
AddExpr<BaseExpr<A>, MatrixT<C>> operator+(const BaseExpr<A>& a, const MatrixT<C>& b) {
	return AddExpr<BaseExpr<A>, MatrixT<C>>(a, b);
}

template<class A, class C>
AddExpr<MatrixT<C>, BaseExpr<A>> operator+(const MatrixT<C>& a, const BaseExpr<A>& b) {
	return AddExpr<MatrixT<C>, BaseExpr<A>>(a, b);
}

template<class A, class B>
class MulExpr : public BaseExpr<MulExpr<A, B>>
{
	const A& v1;
	const B& v2;

public:

	static constexpr bool left_leaf  = IsLeaf<A>::value;
	static constexpr bool right_leaf = IsLeaf<B>::value;

	MulExpr(const A& v1, const B& v2) :
		v1{ v1 },
		v2{ v2 }
	{}

	void analyzeLeft(ExprAnalyzer& ea) const 
	{
		if constexpr (!left_leaf)
			v1.analyzeLeft(ea);
		else
		{
			this->eval.nrows = v1.rows();
			this->eval.ncols = v2.cols();
			this->eval.done = true;
			return;
		}

		
	}

	void analyzeRight(ExprAnalyzer& ea) const 
	{
		if constexpr (!right_leaf)
			v1.analyzeRight(ea);
		else
		{
			ea.nrows = v1.rows();
			ea.ncols = v2.cols();
			return;
		}

		ea.ncols = v2.cols();
	}

	double evalExpr(int row, int col, int idx) const
	{
		int sum = 0;
		int ncols = 0;//cols(); // TODO: Must be replaced with number of columns
		int fidx = row * ncols;

		for(int i = 0; i < ncols; ++i)
			sum += v1.evalExpr(row, col, fidx + i) * v2.evalExpr(row, col, i * ncols + col);
		return sum;
	}

};


template<class A, class B>
MulExpr<BaseExpr<A>, BaseExpr<B>> operator*(const BaseExpr<A>& a, const BaseExpr<B>& b) {
	return MulExpr<BaseExpr<A>, BaseExpr<B>>(a, b);
}
