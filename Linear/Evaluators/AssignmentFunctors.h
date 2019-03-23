#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Type>
struct Equals
{
	inline void operator()(Type& to, const Type& from) const noexcept
	{
		to = from;
	}

	template<class Packet>
	inline void operator()(Type* to, const Packet& from) const noexcept
	{
		impl::pstore<Packet>(to, from);
	}
};

template<class Type>
struct PlusEquals
{
	inline void operator()(Type& to, const Type& from) const noexcept
	{
		to += from;
	}

	template<class Packet>
	inline void operator()(Type* to, const Packet& from) const noexcept
	{
		impl::pstore<Packet>(to, impl::padd<Packet>(impl::pload<Packet>(to), from));
	}
};

template<class Type>
struct MinusEquals
{
	inline void operator()(Type& to, const Type& from) const noexcept
	{
		to -= from;
	}

	template<class Packet>
	inline void operator()(Type* to, const Packet& from) const noexcept
	{
		impl::pstore<Packet>(to, impl::psub<Packet>(impl::pload<Packet>(to), from));
	}
};

template<class Type>
struct TimesEquals
{
	inline void operator()(Type& to, const Type& from) const noexcept
	{
		to *= from;
	}
};

template<class Type>
struct DivideEquals
{
	inline void operator()(Type& to, const Type& from) const noexcept
	{
		to /= from;
	}
};



}