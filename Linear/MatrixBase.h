#pragma once
#include "ForwardDeclarations.h"

template<class Derived>
class MatrixBase
{
public:
	using ThisType		= MatrixBase<Derived>;

	using value_type = typename Traits<Derived>::value_type;
};