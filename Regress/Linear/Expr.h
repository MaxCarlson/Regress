#pragma once
#include <vector>

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)


template<class E>
class BaseExpr
{
public:
	double operator[](size_t i)		const { return static_cast<E const&>(*this)[i]; }

	double operator()(int r, int c) const { return static_cast<E const&>(*this)(r, c); }
	size_t size()					const { return static_cast<E const&>(*this).size(); }

	int row() const { return static_cast<E const&>(*this).row(); }
	int col() const { return static_cast<E const&>(*this).col(); }

	using value_type = typename E::value_type;

	value_type evalExpr(int row, int col, int idx) const { return static_cast<E const&>(*this).evaluate(row, col, idx); }

};

template<class Type>
struct MatrixT : public BaseExpr<MatrixT<Type>>
{
	int nrows;
	int ncolumns;
	std::vector<Type> vals;

	using value_type	= Type;
	using MyType		= MatrixT<Type>;

	MatrixT(const std::initializer_list<std::initializer_list<Type>>& m) :
		nrows{ static_cast<int>(m.size()) },
		ncolumns{ static_cast<int>(m.begin()->size()) },
		vals(nrows * ncolumns)
	{
		int i = 0;
		for (auto it = std::begin(m); it != std::end(m); ++it)
			for (auto jt = std::begin(*it); jt != std::end(*it); ++jt)
				vals[i++] = *jt;
	}

	MatrixT(const BaseExpr<MyType>& expr) :
		nrows{},
		ncolumns{},
		vals{expr.size()}
	{
		int idx = 0;
		for(int i = 0; i < nrows; ++i)
			for (int j = 0; j < ncolumns; ++j, ++idx)
			{
				vals[idx] = expr.evalExpr(i, j, idx);
			}
	}
};

template<class A, class B>
class AddExpr : public BaseExpr<AddExpr<A, B>>
{
	const A& v1;
	const B& v2;
public:

	using value_type = typename A::value_type;
	
	AddExpr(const A& v1, const B& v2) :
		v1{v1},
		v2{v2}
	{}

	value_type evalExpr(int row, int col, int idx) const
	{
		return v1.vals[idx] + v2.vals[idx];
	}
	
};

template <typename E1, typename E2>
AddExpr<E1, E2> operator+(E1 const& u, E2 const& v) {
	return AddExpr<E1, E2>(u, v);
}


template <typename E>

class VecExpression
{

public:

	double operator[](size_t i) const { return static_cast<E const&>(*this)[i]; }
	size_t size()               const { return static_cast<E const&>(*this).size(); }

};

class Vec 
	: public VecExpression<Vec>
{
	std::vector<double> elems;

public:
	double operator[](size_t i) const { return elems[i]; }
	double &operator[](size_t i) { return elems[i]; }
	size_t size() const { return elems.size(); }

	Vec(size_t n) : elems(n) {}

	// construct vector using initializer list 
	Vec(std::initializer_list<double>init) {
		for (auto i : init)
			elems.push_back(i);
	}


	// A Vec can be constructed from any VecExpression, forcing its evaluation.
	template <typename E>
	Vec(VecExpression<E> const& vec) 
		: elems(vec.size()) 
	{
		for (size_t i = 0; i != vec.size(); ++i) {
			elems[i] = vec[i];
		}
	}
};

template <typename E1, typename E2>
class VecSum : public VecExpression<VecSum<E1, E2> >
{
	E1 const& _u;
	E2 const& _v;
public:

	VecSum(E1 const& u, E2 const& v) : _u(u), _v(v) {
	}

	double operator[](size_t i) const { return _u[i] + _v[i]; }
	size_t size()               const { return _v.size(); }
};

template <typename E1, typename E2>
VecSum<E1, E2> operator+(E1 const& u, E2 const& v) {
	return VecSum<E1, E2>(u, v);
}