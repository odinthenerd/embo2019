#pragma once
//  Copyright 2018 Odin Holmes.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <cstdint>
#include <boost/tmp.hpp>
#include <utility>
#include <new>
#include "handle.hpp"
#include "object.hpp"
#include "critical_section.hpp"

namespace kvasir
{
namespace memory
{
template <typename T, typename U>
class pool;
namespace detail
{
using namespace boost::tmp;

struct pool_seam;

template <typename T, typename U>
using size_less = bool_<(sizeof(T) < sizeof(U))>;

template <typename T, typename U>
using fits_in_pool = bool_<(sizeof(T) < U::obj_size)>;

template <typename ObjSize, typename NumElements>
struct pool_base
{
	static object<ObjSize::value> _data[NumElements::value];
	static object_base *_head;

	static void init()
	{
		_head = &_data[0];
		for (unsigned i = 0; i < NumElements::value - 1; i++)
		{
			_data[i]._next = &_data[i + 1];
		}
	}

	static object<ObjSize::value> *pop()
	{
		return critical_section([&]() {
			auto temp = _head;
			_head = temp->_next;
			return static_cast<object<ObjSize::value> *>(temp);
		});
	}

	static void push(object_base *p)
	{
		return critical_section([&]() {
			p->_next = _head;
			_head = p;
		});
	}

	static constexpr unsigned obj_size = ObjSize::value;
};

template <typename ObjSize, typename NumElements>
object<ObjSize::value> pool_base<ObjSize, NumElements>::_data[NumElements::value];
template <typename ObjSize, typename NumElements>
object_base *pool_base<ObjSize, NumElements>::_head;

/*template<typename T, typename...Ty, typename...Pools, typename...Ts,
					typename pool_base_type = call_<tee_<
					    find_if_<push_front_<T,lift_<fits_in_pool>>, lift_<index_>>,
					    identity_,
					    call_f_<>>, Pools...>>
			auto find_pool(pool<list_<Ty...>, list_<Pools...>> p)->pool_base_type & {
				return static_cast<pool_base_type &>(p);
			}*/

template <typename P, typename T>
using find_pool = call_<ui1_<unpack_<tee_<
							find_if_<push_front_<T, lift_<fits_in_pool>>, lift_<index_>>,
							identity_,
							call_f_<>>>>,
						P>;

template <typename L, typename S>
struct chunk_by_size;

template <typename O, typename I, typename S>
struct chunk_by_size<list_<O, I>, S>
{
	using type = list_<call_<join_<>, O, list_<call_<unpack_<filter_<push_back_<S, lift_<fits_in_pool>>, push_front_<S, lift_<pool_base>>>>, I>>>, call_<unpack_<remove_if_<push_back_<S, lift_<fits_in_pool>>>>, I>>;
};

template <typename Types, typename... Sizes>
using make_pool = pool<Types, list_<Sizes...>> /*call_<
					sort_<transform_<ui0_<>, less_<>>,
							push_front_<list_<list_<>, Types>, fold_left_<lift_<chunk_by_size, result_<>>, push_front_<Types, lift_<pool>>>>>,
					Sizes...>*/
	;

template <unsigned obj_size, unsigned num_objects>
using pool_buffer = detail::pool_base<uint_<obj_size>, uint_<num_objects>>;

template <typename P>
struct destruct_visitor
{
	object_base *_obj;
	template <typename T>
	void operator()(T &i)
	{
		i.~T();
		find_pool<P, T>::push(_obj);
	}
};
} // namespace detail
template <typename... T, typename... Pools>
class pool<boost::tmp::list_<T...>, boost::tmp::list_<Pools...>> : public Pools...
{
  public:
	using type = pool;
	//init must be called once before any other functions are touched
	//it is suggested to
	friend void init(pool p)
	{
		int a[] = {(Pools::init(), 0)...};
		(void)a;
	}

	friend detail::pool_seam;
};

template <typename T, typename P, typename... Ts>
handle<P, T> make_object(const P p, Ts &&... args)
{
	using namespace boost::tmp;
	auto obj = detail::find_pool<P, T>::pop();
	char *buf = &obj->_buf[0];
	::new (buf) T{std::forward<Ts>(args)...};
	obj->_type = call_<ui0_<unpack_<find_if_<is_<T>>>>, P>::value;
	return handle<P, T>{obj};
}

template <typename Types, typename... Sizes>
using make_pool = detail::make_pool<Types, Sizes...>;

template <unsigned obj_size, unsigned num_objects>
using pool_buffer = detail::pool_base<boost::tmp::uint_<obj_size>, boost::tmp::uint_<num_objects>>;
} // namespace memory
} // namespace kvasir
