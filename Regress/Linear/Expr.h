#pragma once
#include <vector>

template<class T> class Matrix;

enum class Op
{
	ADD,
	SUB,
	MUL,
	DIV,
	POW,
	DOT,
	TRANSPOSE
};

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)

class BaseExpr
{
public:
	BaseExpr* next;
	BaseExpr* prev;


	BaseExpr* operator+(BaseExpr* other)
	{

	}
	
};

template<class A, class B>
class AddExpr : public BaseExpr
{
	A* v1;
	B* v2;
public:
	AddExpr(A* v1, B* v2) :
		v1{v1},
		v2{v2}
	{

	}
};