//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/condition_variable.hpp>
#include <pika/execution.hpp>
#include <pika/init.hpp>
#include <pika/mutex.hpp>
#include <pika/testing.hpp>
#include <pika/thread.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

struct custom_type_non_default_constructible_non_copyable
{
    int x;
    custom_type_non_default_constructible_non_copyable() = delete;
    explicit custom_type_non_default_constructible_non_copyable(int x)
      : x(x){};
    custom_type_non_default_constructible_non_copyable(
        custom_type_non_default_constructible_non_copyable&&) = default;
    custom_type_non_default_constructible_non_copyable& operator=(
        custom_type_non_default_constructible_non_copyable&&) = default;
    custom_type_non_default_constructible_non_copyable(
        custom_type_non_default_constructible_non_copyable const&) = delete;
    custom_type_non_default_constructible_non_copyable& operator=(
        custom_type_non_default_constructible_non_copyable const&) = delete;
};

namespace ex = pika::execution::experimental;
namespace tt = pika::this_thread::experimental;

///////////////////////////////////////////////////////////////////////////////
void test_let_value()
{
    ex::thread_pool_scheduler sched{};

    // void predecessor
    {
        auto result =
            tt::sync_wait(ex::schedule(sched) | ex::let_value([]() { return ex::just(42); }));
        PIKA_TEST_EQ(result, 42);
    }

    {
        auto result = tt::sync_wait(
            ex::schedule(sched) | ex::let_value([=]() { return ex::transfer_just(sched, 42); }));
        PIKA_TEST_EQ(result, 42);
    }

    {
        auto result = tt::sync_wait(
            ex::just() | ex::let_value([=]() { return ex::transfer_just(sched, 42); }));
        PIKA_TEST_EQ(result, 42);
    }

    // int predecessor, value ignored
    {
        auto result = tt::sync_wait(
            ex::transfer_just(sched, 43) | ex::let_value([](int&) { return ex::just(42); }));
        PIKA_TEST_EQ(result, 42);
    }

    {
        auto result = tt::sync_wait(ex::transfer_just(sched, 43) |
            ex::let_value([=](int&) { return ex::transfer_just(sched, 42); }));
        PIKA_TEST_EQ(result, 42);
    }

    {
        auto result = tt::sync_wait(
            ex::just(43) | ex::let_value([=](int&) { return ex::transfer_just(sched, 42); }));
        PIKA_TEST_EQ(result, 42);
    }

    // int predecessor, value used
    {
        auto result = tt::sync_wait(ex::transfer_just(sched, 43) | ex::let_value([](int& x) {
            return ex::just(42) | ex::then([&](int y) { return x + y; });
        }));
        PIKA_TEST_EQ(result, 85);
    }

    {
        auto result = tt::sync_wait(ex::transfer_just(sched, 43) | ex::let_value([=](int& x) {
            return ex::transfer_just(sched, 42) | ex::then([&](int y) { return x + y; });
        }));
        PIKA_TEST_EQ(result, 85);
    }

    {
        auto result = tt::sync_wait(ex::just(43) | ex::let_value([=](int& x) {
            return ex::transfer_just(sched, 42) | ex::then([&](int y) { return x + y; });
        }));
        PIKA_TEST_EQ(result, 85);
    }

    // predecessor throws, let sender is ignored
    {
        bool exception_thrown = false;

        try
        {
            tt::sync_wait(ex::transfer_just(sched, 43) | ex::then([](int) -> int {
                throw std::runtime_error("error");
            }) | ex::let_value([](int&) {
                PIKA_TEST(false);
                return ex::just(0);
            }));
            PIKA_TEST(false);
        }
        catch (std::runtime_error const& e)
        {
            PIKA_TEST_EQ(std::string(e.what()), std::string("error"));
            exception_thrown = true;
        }

        PIKA_TEST(exception_thrown);
    }
}

void check_exception_ptr_message(std::exception_ptr ep, std::string const& message)
{
    try
    {
        std::rethrow_exception(ep);
    }
    catch (std::runtime_error const& e)
    {
        PIKA_TEST_EQ(std::string(e.what()), message);
        return;
    }

    PIKA_TEST(false);
}

void test_let_error()
{
    ex::thread_pool_scheduler sched{};

    {
        [[maybe_unused]]
        auto result = tt::sync_wait(ex::schedule(sched) | ex::then([]() -> int {
            throw std::runtime_error("error");
        }) | ex::let_error([=](std::exception_ptr& ep) {
            return ex::transfer_just(sched, 42);
            //return ex::just(42); //compiles
        }));
    }

    //{
    //    auto result = tt::sync_wait(ex::just() | ex::then([]() -> int {
    //        throw std::runtime_error("error");
    //    }) | ex::let_error([=](std::exception_ptr& ep) {
    //        check_exception_ptr_message(ep, "error");
    //        return ex::transfer_just(sched, 42);
    //    }));
    //    PIKA_TEST_EQ(result, 42);
    //}

    //// predecessor doesn't throw, let sender is ignored
    //{
    //    auto result =
    //        tt::sync_wait(ex::transfer_just(sched, 42) | ex::let_error([](std::exception_ptr) {
    //            PIKA_TEST(false);
    //            return ex::just(43);
    //        }));
    //    PIKA_TEST_EQ(result, 42);
    //}

    //{
    //    auto result =
    //        tt::sync_wait(ex::transfer_just(sched, 42) | ex::let_error([=](std::exception_ptr) {
    //            PIKA_TEST(false);
    //            return ex::transfer_just(sched, 43);
    //        }));
    //    PIKA_TEST_EQ(result, 42);
    //}

    //{
    //    auto result = tt::sync_wait(ex::just(42) | ex::let_error([=](std::exception_ptr) {
    //        PIKA_TEST(false);
    //        return ex::transfer_just(sched, 43);
    //    }));
    //    PIKA_TEST_EQ(result, 42);
    //}
}

void test_bulk()
{
    std::vector<int> const ns = {0, 1, 10, 43};

    for (int n : ns)
    {
        std::vector<int> v(n, 0);
        pika::thread::id parent_id = pika::this_thread::get_id();

        tt::sync_wait(ex::schedule(ex::thread_pool_scheduler{}) | ex::bulk(n, [&](int i) {
            ++v[i];
            PIKA_TEST_NEQ(parent_id, pika::this_thread::get_id());
        }));

        for (int i = 0; i < n; ++i)
        {
            PIKA_TEST_EQ(v[i], 1);
        }
    }

    //for (auto n : ns)
    //{
    //    std::vector<int> v(n, -1);
    //    pika::thread::id parent_id = pika::this_thread::get_id();

    //    auto v_out = tt::sync_wait(ex::transfer_just(ex::thread_pool_scheduler{}, std::move(v)) |
    //        ex::bulk(n, [&parent_id](int i, std::vector<int>& v) {
    //            v[i] = i;
    //            PIKA_TEST_NEQ(parent_id, pika::this_thread::get_id());
    //        }));

    //    for (int i = 0; i < n; ++i)
    //    {
    //        PIKA_TEST_EQ(v_out[i], i);
    //    }
    //}

    // l-value reference sender
    for (int n : ns)
    {
        std::vector<int> v(n, 0);
        pika::thread::id parent_id = pika::this_thread::get_id();

        auto s = ex::schedule(ex::thread_pool_scheduler{}) | ex::bulk(n, [&](int i) {
            ++v[i];
            PIKA_TEST_NEQ(parent_id, pika::this_thread::get_id());
        });
        tt::sync_wait(s);

        for (int i = 0; i < n; ++i)
        {
            PIKA_TEST_EQ(v[i], 1);
        }
    }

    // The specification only allows integral shapes
#if !defined(PIKA_HAVE_STDEXEC)
    {
        std::unordered_set<std::string> string_map;
        std::vector<std::string> v = {"hello", "brave", "new", "world"};
        std::vector<std::string> v_ref = v;

        pika::mutex mtx;

        tt::sync_wait(ex::schedule(ex::thread_pool_scheduler{}) |
            ex::bulk(std::move(v), [&](std::string const& s) {
                std::lock_guard lk(mtx);
                string_map.insert(s);
            }));

        for (auto const& s : v_ref)
        {
            PIKA_TEST(string_map.find(s) != string_map.end());
        }
    }
#endif

    for (auto n : ns)
    {
        int const i_fail = 3;

        std::vector<int> v(n, -1);
        bool const expect_exception = n > i_fail;

        try
        {
            tt::sync_wait(ex::transfer_just(ex::thread_pool_scheduler{}) | ex::bulk(n, [&v](int i) {
                if (i == i_fail)
                {
                    throw std::runtime_error("error");
                }
                v[i] = i;
            }));

            if (expect_exception)
            {
                PIKA_TEST(false);
            }
        }
        catch (std::runtime_error const& e)
        {
            if (!expect_exception)
            {
                PIKA_TEST(false);
            }

            PIKA_TEST_EQ(std::string(e.what()), std::string("error"));
        }

        if (expect_exception)
        {
            PIKA_TEST_EQ(v[i_fail], -1);
        }
        else
        {
            for (int i = 0; i < n; ++i)
            {
                PIKA_TEST_EQ(v[i], i);
            }
        }
    }
}

void test_completion_scheduler()
{
    {
        auto sender = ex::schedule(ex::thread_pool_scheduler{});
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }

    {
        auto sender = ex::then(ex::schedule(ex::thread_pool_scheduler{}), []() {});
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }

    {
        auto sender = ex::transfer_just(ex::thread_pool_scheduler{}, 42);
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }

    {
        auto sender = ex::bulk(ex::schedule(ex::thread_pool_scheduler{}), 10, [](int) {});
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }

    {
        auto sender = ex::then(
            ex::bulk(ex::transfer_just(ex::thread_pool_scheduler{}, 42), 10, [](int, int) {}),
            [](int) {});
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }

    {
        auto sender =
            ex::bulk(ex::then(ex::transfer_just(ex::thread_pool_scheduler{}, 42), [](int) {}), 10,
                [](int, int) {});
        auto completion_scheduler =
            ex::get_completion_scheduler<ex::set_value_t>(ex::get_env(sender));
        static_assert(
            std::is_same_v<std::decay_t<decltype(completion_scheduler)>, ex::thread_pool_scheduler>,
            "the completion scheduler should be a thread_pool_scheduler");
    }
}

void test_drop_value()
{
    ex::thread_pool_scheduler sched{};

    {
        tt::sync_wait(ex::drop_value(ex::schedule(sched)));
        static_assert(std::is_void_v<decltype(tt::sync_wait(ex::drop_value(ex::schedule(sched))))>);
    }

    {
        tt::sync_wait(ex::drop_value(ex::transfer_just(sched, 3)));
        static_assert(
            std::is_void_v<decltype(tt::sync_wait(ex::drop_value(ex::transfer_just(sched, 3))))>);
    }

    {
        tt::sync_wait(ex::drop_value(ex::transfer_just(sched, std::string("hello"))));
        static_assert(std::is_void_v<decltype(tt::sync_wait(
                ex::drop_value(ex::transfer_just(sched, std::string("hello")))))>);
    }

    {
        tt::sync_wait(ex::drop_value(
            ex::transfer_just(sched, custom_type_non_default_constructible_non_copyable{0})));
        static_assert(std::is_void_v<decltype(tt::sync_wait(ex::drop_value(ex::transfer_just(
                sched, custom_type_non_default_constructible_non_copyable{0}))))>);
    }

    {
        auto s = ex::drop_value(ex::then(ex::just(), [] { throw std::runtime_error("error"); }));

        bool exception_thrown = false;

        try
        {
            tt::sync_wait(std::move(s));
            PIKA_TEST(false);
        }
        catch (std::runtime_error const& e)
        {
            PIKA_TEST_EQ(std::string(e.what()), std::string("error"));
            exception_thrown = true;
        }

        PIKA_TEST(exception_thrown);
    }
}

void test_split_tuple()
{
    ex::thread_pool_scheduler sched{};

    {
        auto [s] = ex::split_tuple(ex::transfer_just(sched, std::tuple(42)));
        PIKA_TEST_EQ(tt::sync_wait(std::move(s)), 42);
    }

    {
        auto [s1, s2, s3] =
            ex::split_tuple(ex::transfer_just(sched, std::tuple(42, std::string{"hello"}, 3.14)));
        PIKA_TEST_EQ(tt::sync_wait(std::move(s1)), 42);
        PIKA_TEST_EQ(tt::sync_wait(std::move(s2)), std::string{"hello"});
        PIKA_TEST_EQ(tt::sync_wait(std::move(s3)), 3.14);
    }

    {
        auto [s1, s2, s3] =
            ex::split_tuple(ex::transfer_just(sched, std::tuple(42, std::string{"hello"}, 3.14)));
        auto s1_transfer = std::move(s1) | ex::transfer(sched);
        auto s2_transfer = std::move(s2) | ex::transfer(sched);
        auto s3_transfer = std::move(s3) | ex::transfer(sched);
        PIKA_TEST_EQ(tt::sync_wait(std::move(s1_transfer)), 42);
        PIKA_TEST_EQ(tt::sync_wait(std::move(s2_transfer)), std::string{"hello"});
        PIKA_TEST_EQ(tt::sync_wait(std::move(s3_transfer)), 3.14);
    }

    {
        auto [s1, s2, s3] = ex::split_tuple(ex::then(ex::schedule(sched),
            []() -> std::tuple<int, std::string, double> { throw std::runtime_error("error"); }));

        auto check_exception = [](auto&& s) {
            bool exception_thrown = false;

            try
            {
                tt::sync_wait(std::move(s));
                PIKA_TEST(false);
            }
            catch (std::runtime_error const& e)
            {
                PIKA_TEST_EQ(std::string(e.what()), std::string("error"));
                exception_thrown = true;
            }

            PIKA_TEST(exception_thrown);
        };

        check_exception(std::move(s1));
        check_exception(std::move(s2));
        check_exception(std::move(s3));
    }
}

void test_scheduler_queries()
{
    PIKA_TEST(ex::get_forward_progress_guarantee(ex::thread_pool_scheduler{}) ==
        ex::forward_progress_guarantee::weakly_parallel);
}

///////////////////////////////////////////////////////////////////////////////
int pika_main()
{
    //test_let_value();
    test_let_error();
    //test_drop_value();
    //test_split_tuple();
    //test_completion_scheduler();
    //test_scheduler_queries();

    return pika::finalize();
}

int main(int argc, char* argv[])
{
    PIKA_TEST_EQ_MSG(pika::init(pika_main, argc, argv), 0, "pika main exited with non-zero status");

    return 0;
}
