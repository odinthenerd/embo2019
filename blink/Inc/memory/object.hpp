#pragma once
//  Copyright 2018 Odin Holmes.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <cstdint>

namespace kvasir
{
namespace memory
{
template <unsigned N>
class object;
namespace detail
{
struct handle_referencable_seam;
struct object_seam;
struct object_base
{
    object_base *_next;
    uint16_t _type;
    friend detail::handle_referencable_seam;
};
} // namespace detail

template <unsigned N>
struct object : detail::object_base
{
    friend detail::object_seam;
    char _buf[N];
};
} // namespace memory
} // namespace kvasir
