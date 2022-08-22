//  Copyright (c) 2020 ETH Zurich
//  Copyright (c) 2015 Hartmut Kaiser
//  Copyright (c) 2013 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/chrono.hpp>
#include <pika/errors/try_catch_exception_ptr.hpp>
#include <pika/execution.hpp>
#include <pika/functional.hpp>
#include <pika/future.hpp>
#include <pika/init.hpp>
#include <pika/memory/intrusive_ptr.hpp>
#include <pika/program_options.hpp>
#include <pika/testing.hpp>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

std::atomic<bool> done{false};

///////////////////////////////////////////////////////////////////////////////
struct external_future_executor
{
    // This is not actually called by dataflow, but it is used for the return
    // type calculation of it. dataflow_finalize has to set the same type to
    // the future state.
    template <typename F, typename... Ts>
    decltype(auto) async_execute_helper(std::true_type, F&& f, Ts&&... ts)
    {
        // The completion of f is signalled out-of-band.
        pika::invoke(std::forward<F>(f), std::forward<Ts>(ts)...);
        return pika::async(
            []() { pika::util::yield_while([]() { return !done; }); });
    }

    template <typename F, typename... Ts>
    decltype(auto) async_execute_helper(std::false_type, F&& f, Ts&&... ts)
    {
        // The completion of f is signalled out-of-band.
        auto&& r = pika::invoke(std::forward<F>(f), std::forward<Ts>(ts)...);
        return pika::async([r = std::move(r)]() {
            pika::util::yield_while([]() { return !done; });
            return r;
        });
    }

    template <typename F, typename... Ts>
    decltype(auto) async_execute(F&& f, Ts&&... ts)
    {
        using is_void = typename std::is_void<
            typename pika::util::detail::invoke_result<F, Ts...>::type>;
        return async_execute_helper(
            is_void{}, std::forward<F>(f), std::forward<Ts>(ts)...);
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize_helper(
        std::true_type, Frame frame, F&& f, Futures&& futures)
    {
        pika::detail::try_catch_exception_ptr(
            [&]() {
                pika::util::detail::invoke_fused(
                    std::forward<F>(f), std::forward<Futures>(futures));

                // Signal completion from another thread/task.
                pika::intrusive_ptr<
                    typename std::remove_pointer<std::decay_t<Frame>>::type>
                    frame_p(frame);
                pika::apply([frame_p = std::move(frame_p)]() {
                    pika::util::yield_while([]() { return !done; });
                    frame_p->set_data(pika::util::detail::unused_type{});
                });
            },
            [&](std::exception_ptr ep) {
                frame->set_exception(std::move(ep));
            });
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize_helper(
        std::false_type, Frame frame, F&& f, Futures&& futures)
    {
        pika::detail::try_catch_exception_ptr(
            [&]() {
                auto&& r = pika::util::detail::invoke_fused(
                    std::forward<F>(f), std::forward<Futures>(futures));

                // Signal completion from another thread/task.
                pika::intrusive_ptr<
                    typename std::remove_pointer<std::decay_t<Frame>>::type>
                    frame_p(frame);
                pika::apply([frame_p = std::move(frame_p), r = std::move(r)]() {
                    pika::util::yield_while([]() { return !done; });
                    frame_p->set_data(std::move(r));
                });
            },
            [&](std::exception_ptr ep) {
                frame->set_exception(std::move(ep));
            });
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize(Frame&& frame, F&& f, Futures&& futures)
    {
        using is_void =
            typename std::remove_pointer<std::decay_t<Frame>>::type::is_void;
        dataflow_finalize_helper(is_void{}, std::forward<Frame>(frame),
            std::forward<F>(f), std::forward<Futures>(futures));
    }
};

struct additional_argument
{
};

struct external_future_additional_argument_executor
{
    // This is not actually called by dataflow, but it is used for the return
    // type calculation of it. dataflow_finalize has to set the same type to
    // the future state.
    template <typename F, typename... Ts>
    decltype(auto) async_execute_helper(std::true_type, F&& f, Ts&&... ts)
    {
        // The completion of f is signalled out-of-band.
        pika::invoke(
            std::forward<F>(f), additional_argument{}, std::forward<Ts>(ts)...);
        return pika::async(
            []() { pika::util::yield_while([]() { return !done; }); });
    }

    template <typename F, typename... Ts>
    decltype(auto) async_execute_helper(std::false_type, F&& f, Ts&&... ts)
    {
        // The completion of f is signalled out-of-band.
        auto&& r = pika::invoke(
            std::forward<F>(f), additional_argument{}, std::forward<Ts>(ts)...);
        return pika::async([r = std::move(r)]() {
            pika::util::yield_while([]() { return !done; });
            return r;
        });
    }

    template <typename F, typename... Ts>
    decltype(auto) async_execute(F&& f, Ts&&... ts)
    {
        using is_void =
            typename std::is_void<typename pika::util::detail::invoke_result<F,
                additional_argument, Ts...>::type>;
        return async_execute_helper(
            is_void{}, std::forward<F>(f), std::forward<Ts>(ts)...);
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize_helper(
        std::true_type, Frame frame, F&& f, Futures&& futures)
    {
        pika::detail::try_catch_exception_ptr(
            [&]() {
                additional_argument a{};
                pika::util::detail::invoke_fused(std::forward<F>(f),
                    std::tuple_cat(
                        std::tie(a), std::forward<Futures>(futures)));

                // Signal completion from another thread/task.
                pika::intrusive_ptr<
                    typename std::remove_pointer<std::decay_t<Frame>>::type>
                    frame_p(frame);
                pika::apply([frame_p = std::move(frame_p)]() {
                    pika::util::yield_while([]() { return !done; });
                    frame_p->set_data(pika::util::detail::unused_type{});
                });
            },
            [&](std::exception_ptr ep) {
                frame->set_exception(std::move(ep));
            });
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize_helper(
        std::false_type, Frame frame, F&& f, Futures&& futures)
    {
        pika::detail::try_catch_exception_ptr(
            [&]() {
                additional_argument a{};
                auto&& r = pika::util::detail::invoke_fused(std::forward<F>(f),
                    std::tuple_cat(
                        std::tie(a), std::forward<Futures>(futures)));

                // Signal completion from another thread/task.
                pika::intrusive_ptr<
                    typename std::remove_pointer<std::decay_t<Frame>>::type>
                    frame_p(frame);
                pika::apply([frame_p = std::move(frame_p), r = std::move(r)]() {
                    pika::util::yield_while([]() { return !done; });
                    frame_p->set_data(std::move(r));
                });
            },
            [&](std::exception_ptr ep) {
                frame->set_exception(std::move(ep));
            });
    }

    template <typename Frame, typename F, typename Futures>
    void dataflow_finalize(Frame&& frame, F&& f, Futures&& futures)
    {
        using is_void =
            typename std::remove_pointer<std::decay_t<Frame>>::type::is_void;
        dataflow_finalize_helper(is_void{}, std::forward<Frame>(frame),
            std::forward<F>(f), std::forward<Futures>(futures));
    }
};

namespace pika { namespace parallel { namespace execution {
    template <>
    struct is_two_way_executor<external_future_executor> : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<external_future_additional_argument_executor>
      : std::true_type
    {
    };
}}}    // namespace pika::parallel::execution

int pika_main()
{
    // We time the spawn and the wait. The wait should take significantly
    // longer than the spawn, and the wait should be long.
    {
        external_future_executor exec;
        pika::chrono::detail::high_resolution_timer t;
        pika::future<void> f = pika::dataflow(exec, []() {
            // This represents an asynchronous operation which has an
            // out-of-band mechanism for signaling completion.
            pika::apply([]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                done = true;
            });
        });
        double spawn_time = t.elapsed();
        t.restart();
        f.get();
        double wait_time = t.elapsed();
        PIKA_TEST_LT(spawn_time, wait_time);
        PIKA_TEST_LT(0.3, wait_time);
    }

    {
        done = false;

        external_future_executor exec;
        pika::chrono::detail::high_resolution_timer t;
        pika::future<int> f = pika::dataflow(exec, []() {
            // This represents an asynchronous operation which has an
            // out-of-band mechanism for signaling completion.
            pika::apply([]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                done = true;
            });

            return 42;
        });
        double spawn_time = t.elapsed();
        t.restart();
        int r = f.get();
        PIKA_TEST_EQ(r, 42);
        double wait_time = t.elapsed();
        PIKA_TEST_LT(spawn_time, wait_time);
        PIKA_TEST_LT(0.3, wait_time);
    }

    {
        done = false;

        external_future_additional_argument_executor exec;
        pika::chrono::detail::high_resolution_timer t;
        pika::future<void> f = pika::dataflow(exec, [](additional_argument) {
            // This represents an asynchronous operation which has an
            // out-of-band mechanism for signaling completion.
            pika::apply([]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                done = true;
            });
        });
        double spawn_time = t.elapsed();
        t.restart();
        f.get();
        double wait_time = t.elapsed();
        PIKA_TEST_LT(spawn_time, wait_time);
        PIKA_TEST_LT(0.3, wait_time);
    }

    {
        done = false;

        external_future_additional_argument_executor exec;
        pika::chrono::detail::high_resolution_timer t;
        pika::future<int> f = pika::dataflow(exec, [](additional_argument) {
            // This represents an asynchronous operation which has an
            // out-of-band mechanism for signaling completion.
            pika::apply([]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                done = true;
            });

            return 42;
        });
        double spawn_time = t.elapsed();
        t.restart();
        int r = f.get();
        PIKA_TEST_EQ(r, 42);
        double wait_time = t.elapsed();
        PIKA_TEST_LT(spawn_time, wait_time);
        PIKA_TEST_LT(0.3, wait_time);
    }

    return pika::finalize();
}

int main(int argc, char* argv[])
{
    PIKA_TEST_EQ_MSG(pika::init(pika_main, argc, argv), 0,
        "pika main exited with non-zero status");

    return pika::util::report_errors();
}
