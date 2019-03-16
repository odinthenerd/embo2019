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
template <typename T, typename U>
class pool;
template <typename P>
class queue;
namespace detail
{
using namespace boost::tmp;

struct handle_seam;

struct handle_impl
{
	object_base *_obj;
	template <typename T>
	T &as()
	{
		return *static_cast<T *>(static_cast<void *>(&static_cast<object<16> *>(_obj)->_buf[0]));
	}
	template <typename T>
	const T &as() const
	{
		return *static_cast<T *>(static_cast<void *>(&static_cast<object<16> *>(_obj)->_buf[0]));
	}
};
template <typename P>
struct destruct_visitor;

template <typename I, typename E>
struct visit_helper
{
	template <typename V>
	void operator()(V &&visitor, handle_impl &h) const
	{
		if (h._obj->_type == I::value)
		{
			visitor(h.as<E>());
		}
	}
};

template <typename P>
using make_visitor_helpers = call_<ui0_<unpack_<zip_with_index_<lift_<visit_helper>>>>, P>;
} // namespace detail
template <typename Pool, typename T = void, typename Helpers = detail::make_visitor_helpers<Pool>>
class handle;

template <typename... Ts, typename... Ps, typename... Vs>
class handle<pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>, void, boost::tmp::list_<Vs...>> : protected detail::handle_impl
{
	using pool_type = pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>;

  public:
	handle(kvasir::memory::detail::object_base *p) : detail::handle_impl{p}
	{
		if (p)
		{
			p->_next = nullptr; // no matter whether its coming out of a queue or a pool it should not have a _next any more
		}
	}
	handle() = default;
	handle(const handle &) = delete;
	handle &operator=(const handle &) = delete;
	handle(handle &&x) : detail::handle_impl{x._obj}
	{
		x._obj = nullptr;
	}
	handle &operator=(handle &&x)
	{
		if (this != &x)
		{ //protect against self assign
			if (_obj != nullptr)
			{													  //if we already contain an object
				visit(detail::destruct_visitor<pool_type>{_obj}); //destroy it first
			}
			_obj = x._obj;
			x._obj = nullptr;
		}
		return *this;
	}
	~handle()
	{
		if (_obj != nullptr)
		{
			visit(detail::destruct_visitor<pool_type>{_obj});
		}
	}

	constexpr bool has_value() const noexcept
	{
		return _obj != nullptr;
	}
	constexpr explicit operator bool() const noexcept
	{
		return has_value();
	}
	template <typename T>
	void visit(T &&v)
	{
		int m[] = {0, (Vs{}(v, *this), 0)...};
		(void)m;
	}
	friend queue<pool_type>;
	friend detail::handle_seam;
};

//specialization for the case where we know the type of the object of which we are a handle
template <typename... Ts, typename... Ps, typename T, typename V>
class handle<pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>, T, V> : public handle<pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>, void>
{
	using pool_type = pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>;
	using base = handle<pool<boost::tmp::list_<Ts...>, boost::tmp::list_<Ps...>>, void>;

  public:
	handle(kvasir::memory::detail::object_base *p) : base{p} {}
	handle() = default;
	handle(const handle &) = delete;
	handle &operator=(const handle &) = delete;
	handle(handle &&x) : base{std::move(x)} {}
	handle &operator=(handle &&x)
	{
		static_cast<base &>(*this) = std::move(static_cast<base &>(x));
		return *this;
	}

	using base::has_value;
	using base::operator bool;
	using base::visit;

	T &operator*()
	{
		return detail::handle_impl::as<T>();
	}
	const T &operator*() const
	{
		return detail::handle_impl::as<T>();
	}

	friend detail::handle_seam;
};
} // namespace memory
} // namespace kvasir
