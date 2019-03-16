#pragma once
//  Copyright 2018 Odin Holmes.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include "object.hpp"
#include <boost/tmp.hpp>
#include <type_traits>
namespace kvasir
{
namespace memory
{

//specialize this traits for int and provide a lock() and unlock() member
//function for custom behavior
template <typename T = int>
struct critical_section_traits;

//critical_section() takes a functior, executes it between a lock() and unlock()
//and returns the result.
template <typename T>
auto critical_section(T &&l) -> decltype(decltype(l()){})
{ //will sfinae if return of l() is not constructible
	//decltype(l(),0) always resolves to int, however technically this makes the instantiation
	//of critical_section_traits a dependant type delaying the point of instantiation
	//in order to pull in the traits specialization
	critical_section_traits<decltype(l(), 0)>::lock();
	auto r = l();
	critical_section_traits<decltype(l(), 0)>::unlock();
	return r;
}

template <typename T>
auto critical_section(T &&l) -> typename std::enable_if<std::is_same<void, decltype(l())>::value>::type //will sfinae if l() does not return void
{
	critical_section_traits<decltype(l(), 0)>::lock();
	l();
	critical_section_traits<decltype(l(), 0)>::unlock();
}
} // namespace memory
} // namespace kvasir
