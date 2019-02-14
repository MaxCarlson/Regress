#pragma once
#include <vector>

template<class T> class Matrix;

enum Op
{
	ADD,
	SUB,
	MUL,
	DIV,
	POW
};

// TODO: system to evaluate matrix math expressions at operator=() time 
// (instead of returning many matrices that are throw away)