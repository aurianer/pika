//  Copyright (c) 2020 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>
#include <pika/type_support/pack.hpp>

#include <cstddef>    // for size_t
#include <type_traits>
#include <utility>

namespace pika { namespace util {

    namespace detail {
#if defined(PIKA_HAVE_CXX20_NO_UNIQUE_ADDRESS_ATTRIBUTE)
        template <std::size_t I, typename T>
        struct member_leaf
        {
            PIKA_NO_UNIQUE_ADDRESS T member;

            member_leaf() = default;

            template <typename U>
            explicit constexpr member_leaf(std::piecewise_construct_t, U&& v)
              : member(PIKA_FORWARD(U, v))
            {
            }
        };

        template <std::size_t I, typename T>
        T member_type(member_leaf<I, T> const& /*leaf*/) noexcept;

        template <std::size_t I, typename T>
        static constexpr T& member_get(member_leaf<I, T>& leaf) noexcept
        {
            return leaf.member;
        }
        template <std::size_t I, typename T>
        static constexpr T const& member_get(
            member_leaf<I, T> const& leaf) noexcept
        {
            return leaf.member;
        }
#else
        template <std::size_t I, typename T,
            bool Empty = std::is_empty<T>::value && !std::is_final<T>::value>
        struct member_leaf
        {
            T member;

            member_leaf() = default;

            template <typename U>
            explicit constexpr member_leaf(std::piecewise_construct_t, U&& v)
              : member(PIKA_FORWARD(U, v))
            {
            }
        };

        template <std::size_t I, typename T>
        struct member_leaf<I, T, /*Empty*/ true> : T
        {
            member_leaf() = default;

            template <typename U>
            explicit constexpr member_leaf(std::piecewise_construct_t, U&& v)
              : T(PIKA_FORWARD(U, v))
            {
            }
        };

        template <std::size_t I, typename T>
        T member_type(member_leaf<I, T> const& /*leaf*/) noexcept;

        template <std::size_t I, typename T>
        static constexpr T& member_get(member_leaf<I, T, false>& leaf) noexcept
        {
            return leaf.member;
        }
        template <std::size_t I, typename T>
        static constexpr T& member_get(member_leaf<I, T, true>& leaf) noexcept
        {
            return leaf;
        }
        template <std::size_t I, typename T>
        static constexpr T const& member_get(
            member_leaf<I, T, false> const& leaf) noexcept
        {
            return leaf.member;
        }
        template <std::size_t I, typename T>
        static constexpr T const& member_get(
            member_leaf<I, T, true> const& leaf) noexcept
        {
            return leaf;
        }
#endif
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////
    template <typename Is, typename... Ts>
    struct PIKA_EMPTY_BASES member_pack;

    template <std::size_t... Is, typename... Ts>
    struct PIKA_EMPTY_BASES member_pack<util::index_pack<Is...>, Ts...>
      : detail::member_leaf<Is, Ts>...
    {
        member_pack() = default;

        template <typename... Us>
        explicit constexpr member_pack(std::piecewise_construct_t, Us&&... us)
          : detail::member_leaf<Is, Ts>(
                std::piecewise_construct, PIKA_FORWARD(Us, us))...
        {
        }

        template <std::size_t I>
        constexpr decltype(auto) get() & noexcept
        {
            return detail::member_get<I>(*this);
        }
        template <std::size_t I>
        constexpr decltype(auto) get() const& noexcept
        {
            return detail::member_get<I>(*this);
        }
        template <std::size_t I>
        constexpr decltype(auto) get() && noexcept
        {
            using T = decltype(detail::member_type<I>(*this));
            return static_cast<T&&>(detail::member_get<I>(*this));
        }
        template <std::size_t I>
        constexpr decltype(auto) get() const&& noexcept
        {
            using T = decltype(detail::member_type<I>(*this));
            return static_cast<T&&>(detail::member_get<I>(*this));
        }
    };

    template <typename... Ts>
    using member_pack_for =
        member_pack<typename util::make_index_pack<sizeof...(Ts)>::type, Ts...>;

}}    // namespace pika::util

///////////////////////////////////////////////////////////////////////////////
namespace pika { namespace serialization {
    ///////////////////////////////////////////////////////////////////////////
    template <typename Archive, std::size_t... Is, typename... Ts>
    PIKA_FORCEINLINE void serialize(Archive& ar,
        ::pika::util::member_pack<util::index_pack<Is...>, Ts...>& mp,
        unsigned int const /*version*/ = 0)
    {
        int sequencer[] = {((ar & mp.template get<Is>()), 0)...};
        (void) sequencer;
    }
}}    // namespace pika::serialization
