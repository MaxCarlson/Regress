#pragma once

template<class Derived>
class MatrixBase;

template<class, bool>
class Matrix;

template<class T>
struct Traits
{
	using size_type = int;
};