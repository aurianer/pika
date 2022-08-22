//  Copyright (c) 2007-2020 Hartmut Kaiser
//  Copyright (c) 2019 Austin McCartney
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>

#include <type_traits>
#include <utility>

namespace pika::traits {
    namespace detail {

        ///////////////////////////////////////////////////////////////////////
        template <typename T, typename U, typename Enable = void>
        struct equality_result
        {
        };

        template <typename T, typename U>
        struct equality_result<T, U,
            std::void_t<decltype(std::declval<const T&>() ==
                std::declval<const U&>())>>
        {
            using type =
                decltype(std::declval<const T&>() == std::declval<const U&>());
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename T, typename U, typename Enable = void>
        struct inequality_result
        {
        };

        template <typename T, typename U>
        struct inequality_result<T, U,
            std::void_t<decltype(std::declval<const T&>() !=
                std::declval<const U&>())>>
        {
            using type =
                decltype(std::declval<const T&>() != std::declval<const U&>());
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename T, typename U, typename Enable = void>
        struct is_weakly_equality_comparable_with : std::false_type
        {
        };

        template <typename T, typename U>
        struct is_weakly_equality_comparable_with<T, U,
            std::void_t<typename detail::equality_result<T, U>::type,
                typename detail::equality_result<U, T>::type,
                typename detail::inequality_result<T, U>::type,
                typename detail::inequality_result<U, T>::type>>
          : std::true_type
        {
        };

    }    // namespace detail

    template <typename T, typename U>
    struct is_weakly_equality_comparable_with
      : detail::is_weakly_equality_comparable_with<std::decay_t<T>,
            std::decay_t<U>>
    {
    };

    // for now is_equality_comparable is equivalent to its weak version
    template <typename T, typename U>
    struct is_equality_comparable_with
      : detail::is_weakly_equality_comparable_with<std::decay_t<T>,
            std::decay_t<U>>
    {
    };

    template <typename T>
    struct is_equality_comparable
      : detail::is_weakly_equality_comparable_with<std::decay_t<T>,
            std::decay_t<T>>
    {
    };
}    // namespace pika::traits
