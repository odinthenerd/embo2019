#pragma once
//  Copyright 2018 Odin Holmes.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include "critical_section.hpp"
#include "handle.hpp"

namespace kvasir
{
namespace memory
{
namespace detail
{
struct queue_seam;
}
template <typename P>
class queue
{
	detail::object_base *_head;
	detail::object_base *_tail;

  public:
	queue() : _head{nullptr}, _tail{nullptr} {}
	handle<P, void> pop()
	{
		return handle<P, void>{critical_section([&]() {
					auto temp = _head;
					if(temp) {	//if we are not at the end
						_head = temp->_next;
						if(_head == nullptr){  //if we poped the last one
							_tail = nullptr;
						}
					}
					return temp; })};
	}
	template <typename T>
	void push(handle<P, T> &&h)
	{
		auto obj = h._obj;
		h._obj = nullptr; //move from
		critical_section([&]() {
			if (_tail == nullptr)
			{ //if tail is nullptr then queue is empty
				_head = obj;
			}
			else
			{
				_tail->_next = obj;
			}
			_tail = obj;
		});
	}
	friend detail::queue_seam;
};
} // namespace memory
} // namespace kvasir
