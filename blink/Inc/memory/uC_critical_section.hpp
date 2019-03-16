#pragma once
//  Copyright 2018 Odin Holmes.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include "object.hpp"
#include <boost/tmp.hpp>
namespace kvasir
{
namespace memory
{
template <>
struct critical_section_traits<int>
{
	static void lock()
	{
		//TODO disable interrupts here
	}

	static void unlock()
	{
		//TODO enable interrupts here
	}
};
} // namespace memory
} // namespace kvasir
