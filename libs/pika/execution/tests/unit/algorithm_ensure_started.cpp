//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/modules/execution.hpp>
#include <pika/testing.hpp>

#include <pika/algorithm_test_utils.hpp>

#include <atomic>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace ex = pika::execution::experimental;

// This overload is only used to check dispatching. It is not a useful
// implementation.
template <typename Allocator = pika::detail::internal_allocator<>>
auto tag_invoke(ex::ensure_started_t, custom_sender_tag_invoke s, Allocator const& = Allocator{})
{
    s.tag_invoke_overload_called = true;
    return void_sender{};
}

int main()
{
    // Success path
    {
        std::atomic<bool> set_value_called{false};
        std::atomic<bool> started{false};
        auto s1 = ex::then(void_sender{}, [&]() { started = true; });
        auto s2 = ex::ensure_started(std::move(s1));
        PIKA_TEST(started);
        auto f = [] {};
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s2), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    {
        std::atomic<bool> set_value_called{false};
        std::atomic<bool> started{false};
        auto s1 = ex::then(ex::just(0), [&](int x) {
            started = true;
            return x;
        });
        auto s2 = ex::ensure_started(std::move(s1));
        PIKA_TEST(started);
        auto f = [](int x) { PIKA_TEST_EQ(x, 0); };
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s2), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    {
        std::atomic<bool> set_value_called{false};
        std::atomic<bool> started{false};
        auto s1 = ex::then(ex::just(custom_type_non_default_constructible{42}),
            [&](custom_type_non_default_constructible x) {
                started = true;
                return x;
            });
        auto s2 = ex::ensure_started(std::move(s1));
        PIKA_TEST(started);
        auto f = [](auto x) { PIKA_TEST_EQ(x.x, 42); };
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s2), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    {
        std::atomic<bool> set_value_called{false};
        std::atomic<bool> started{false};
        auto s1 = ex::then(ex::just(custom_type_non_default_constructible_non_copyable{42}),
            [&](custom_type_non_default_constructible_non_copyable&& x) {
                started = true;
                return std::move(x);
            });
        auto s2 = ex::ensure_started(std::move(s1));
        PIKA_TEST(started);
        auto f = [](auto&& x) { PIKA_TEST_EQ(x.x, 42); };
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s2), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    {
        std::atomic<bool> set_value_called{false};
        int x = 42;
        auto s1 = const_reference_sender<int>{x};
        auto s2 = ex::ensure_started(std::move(s1));
        auto f = [](auto&& x) { PIKA_TEST_EQ(x, 42); };
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s2), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    // operator| overload
    {
        std::atomic<bool> set_value_called{false};
        auto s = void_sender{} | ex::ensure_started();
        auto f = [] {};
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        PIKA_TEST(set_value_called);
    }

    // tag_invoke overload
    {
        std::atomic<bool> receiver_set_value_called{false};
        std::atomic<bool> tag_invoke_overload_called{false};
        auto s = custom_sender_tag_invoke{tag_invoke_overload_called} | ex::ensure_started();
        auto f = [] {};
        auto r = callback_receiver<decltype(f)>{f, receiver_set_value_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        PIKA_TEST(receiver_set_value_called);
        PIKA_TEST(tag_invoke_overload_called);
    }

    // Failure path
    {
        std::atomic<bool> set_error_called{false};
        auto s = error_sender{} | ex::ensure_started();
        auto r = error_callback_receiver<decltype(check_exception_ptr)>{
            check_exception_ptr, set_error_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        PIKA_TEST(set_error_called);
    }

    {
        std::atomic<bool> set_error_called{false};
        auto s = const_reference_error_sender{} | ex::ensure_started();
        auto r = error_callback_receiver<decltype(check_exception_ptr)>{
            check_exception_ptr, set_error_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        PIKA_TEST(set_error_called);
    }

#if !defined(PIKA_HAVE_STDEXEC)
    {
        std::atomic<bool> set_error_called{false};
        auto s =
            error_sender{} | ex::ensure_started() | ex::ensure_started() | ex::ensure_started();
        auto r = error_callback_receiver<decltype(check_exception_ptr)>{
            check_exception_ptr, set_error_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        PIKA_TEST(set_error_called);
    }
#endif

    // It's allowed to discard the sender from ensure_started
    {
        ex::just() | ex::ensure_started();
    }

    {
        test_adl_isolation(ex::ensure_started(my_namespace::my_sender{}));
    }

    return 0;
}
