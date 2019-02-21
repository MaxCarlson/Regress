#pragma once
#include <vector>

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)

class ExprAnalyzer
{
	int nrows;
	int ncols;

	// TODO: Could hold OP order in here if needed?

public:
	ExprAnalyzer(int nrows, int ncols) :
		nrows{ nrows },
		ncols{ ncols }
	{
	}

};

template<class E>
class BaseExpr
{
public:
	//double operator[](int i)		const { return static_cast<E const&>(*this)[i]; }

	//double operator()(int r, int c) const { return static_cast<E const&>(*this)(r, c); }
	size_t size()					const { return static_cast<E const&>(*this).size(); }

	int rows() const { return static_cast<E const&>(*this).rows(); }
	int cols() const { return static_cast<E const&>(*this).cols(); }

	//using value_type = typename E::value_type;

	void analyzeExpr(ExprAnalyzer& ea) const { return static_cast<E const&>(*this).analyzeExpr(ea);}

	double evalExpr(int row, int col, int idx) const { return static_cast<E const&>(*this).evalExpr(row, col, idx); }

	//int totalRows() const { return static_cast<E const&>(*this).totalRows(); }
	//int totalCols() const { return static_cast<E const&>(*this).totalCols(); }

};

template<class Type>
class MatrixT : public BaseExpr<MatrixT<Type>>
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
		expr.analyzeExpr(ea);

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


	void analyzeExpr(ExprAnalyzer& ea) const {}
	const value_type& evalExpr(int row, int col, int idx) const { return vals[idx]; }
};

template<class A, class B>
class AddExpr : public BaseExpr<AddExpr<A, B>>
{
	const A& v1;
	const B& v2;
public:

	using value_type = typename MatrixT<A>::value_type;
	
	AddExpr(const A& v1, const B& v2) :
		v1{v1},
		v2{v2}
	{}

	size_t size() const { return v1.size(); }
	int rows() const { return v1.rows(); }
	int cols() const { return v1.cols(); }

	void analyzeExpr(ExprAnalyzer& ea) const
	{
		v1.analyzeExpr(ea);
		v2.analyzeExpr(ea);
	}

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

	using value_type = typename MatrixT<A>::value_type;

	MulExpr(const A& v1, const B& v2) :
		v1{ v1 },
		v2{ v2 }
	{}

	size_t size() const { return v1.size(); }
	int rows() const { return v2.rows(); }
	int cols() const { return v1.cols(); }

	void analyzeExpr(ExprAnalyzer& ea) const
	{
		v1.analyzeExpr(ea);
		v2.analyzeExpr(ea);
	}

	double evalExpr(int row, int col, int idx) const
	{
		int sum = 0;
		int ncols = cols();
		int fidx = row * ncols;

		for(int i = 0; i < cols(); ++i)
			sum += v1.evalExpr(row, col, fidx + i) * v2.evalExpr(row, col, i * ncols + col);
		return sum;
	}

};

template<class A, class B>
MulExpr<MatrixT<A>, MatrixT<B>> operator*(const MatrixT<A>& a, const MatrixT<B>& b) {
	return MulExpr<MatrixT<A>, MatrixT<B>>(a, b);
}

template<class A, class B>
MulExpr<BaseExpr<A>, MatrixT<B>> operator*(const BaseExpr<A>& a, const MatrixT<B>& b) {
	return MulExpr<BaseExpr<A>, MatrixT<B>>(a, b);
}