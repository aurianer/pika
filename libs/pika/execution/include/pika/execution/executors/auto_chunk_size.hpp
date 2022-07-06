//  Copyright (c) 2007-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/executors/auto_chunk_size.hpp

#pragma once

#include <pika/config.hpp>
#include <pika/execution_base/traits/is_executor_parameters.hpp>
#include <pika/modules/timing.hpp>

#include <pika/execution/executors/execution_parameters.hpp>
#include <pika/execution_base/execution.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace pika { namespace execution {
    ///////////////////////////////////////////////////////////////////////////
    /// Loop iterations are divided into pieces and then assigned to threads.
    /// The number of loop iterations combined is determined based on
    /// measurements of how long the execution of 1% of the overall number of
    /// iterations takes.
    /// This executor parameters type makes sure that as many loop iterations
    /// are combined as necessary to run for the amount of time specified.
    ///
    struct auto_chunk_size
    {
    public:
        /// Construct an \a auto_chunk_size executor parameters object
        ///
        /// \note Default constructed \a auto_chunk_size executor parameter
        ///       types will use 80 microseconds as the minimal time for which
        ///       any of the scheduled chunks should run.
        ///
        constexpr auto_chunk_size(std::uint64_t num_iters_for_timing = 0)
          : min_time_(200000)
          , num_iters_for_timing_(num_iters_for_timing)
        {
        }

        /// Construct an \a auto_chunk_size executor parameters object
        ///
        /// \param rel_time     [in] The time duration to use as the minimum
        ///                     to decide how many loop iterations should be
        ///                     combined.
        ///
        explicit auto_chunk_size(pika::chrono::steady_duration const& rel_time,
            std::uint64_t num_iters_for_timing = 0)
          : min_time_(rel_time.value().count())
          , num_iters_for_timing_(num_iters_for_timing)
        {
        }

        /// \cond NOINTERNAL
        // Estimate a chunk size based on number of cores used.
        template <typename Executor, typename F>
        std::size_t get_chunk_size(
            Executor&& exec, F&& f, std::size_t cores, std::size_t count)
        {
            // by default use 1% of the iterations
            if (num_iters_for_timing_ == 0)
            {
                num_iters_for_timing_ = count / 100;
            }

            // perform measurements only if necessary
            if (num_iters_for_timing_ > 0)
            {
                using namespace std::chrono;
                auto t = high_resolution_clock::now();

                // use executor to launch given function for measurements
                std::size_t test_chunk_size =
                    pika::parallel::execution::sync_execute(
                        PIKA_FORWARD(Executor, exec), f, num_iters_for_timing_);

                if (test_chunk_size != 0)
                {
                    auto dur =
                        (high_resolution_clock::now() - t) / test_chunk_size;
                    if (dur.count() != 0 && min_time_ >= dur)
                    {
                        // return chunk size which will create the required
                        // amount of work
                        return (std::min)(
                            count, (std::size_t)(min_time_ / dur));
                    }
                }
            }

            return (count + cores - 1) / cores;
        }
        /// \endcond

    private:
        /// \cond NOINTERNAL
        // target time for on thread (nanoseconds)
        std::chrono::duration<unsigned long, std::nano> min_time_;

        // number of iteration to use for timing
        std::uint64_t num_iters_for_timing_;
        /// \endcond
    };
}}    // namespace pika::execution

namespace pika { namespace parallel { namespace execution {
    /// \cond NOINTERNAL
    template <>
    struct is_executor_parameters<pika::execution::auto_chunk_size>
      : std::true_type
    {
    };
    /// \endcond
}}}    // namespace pika::parallel::execution
