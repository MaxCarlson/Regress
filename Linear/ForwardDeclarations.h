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


};