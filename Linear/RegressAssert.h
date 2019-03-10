#pragma once
#include <string>



struct regress_assert
{
	template<class Arg>
	regress_assert(bool condition, std::string msg)
	{
#ifndef NDEBUG
		if (!condition)
			throw std::runtime_error(msg);
#endif
	}
};
