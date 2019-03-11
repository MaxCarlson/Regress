#pragma once

template<class Derived>
class MatrixBase;

template<class, bool>
class Matrix;

template<class, bool>
class MatrixT;

template<class T>
struct Traits
{};


template<class Derived>
struct BaseExpr
{
	using size_type = int;

	template<class Matrix>
	void assign(Matrix& to)
	{
		auto rit = static_cast<const Derived&>(*this);
		to.resize(rit.resultRows(), rit.resultCols());
		
		for (auto lit = to.begin();
			lit != to.end(); ++lit, ++rit)
		{
			*lit = *rit;
		}
	}
};