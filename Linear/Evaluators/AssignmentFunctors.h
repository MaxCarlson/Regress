#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Type>
struct Equals
{
	inline void operator()(Type& to, const Type& from)
	{
		to = from;
	}

	template<class Packet>
	inline void operator()(Packet& to, const Packet& from)
	{
		impl::pstore<Packet>(to, from);
	}
};

template<class Type>
struct PlusEquals
{
	inline void operator()(Type& to, const Type& from)
	{
		to += from;
	}
};

template<class Type>
struct MinusEquals
{
	inline void operator()(Type& to, const Type& from)
	{
		to -= from;
	}
};

template<class Type>
struct TimesEquals
{
	inline void operator()(Type& to, const Type& from)
	{
		to *= from;
	}
};

template<class Type>
struct DivideEquals
{
	inline void operator()(Type& to, const Type& from)
	{
		to /= from;
	}
};



}