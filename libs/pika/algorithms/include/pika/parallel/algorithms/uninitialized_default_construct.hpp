//  Copyright (c) 2014-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/uninitialized_default_construct.hpp

#pragma once

#if defined(DOXYGEN)
namespace pika {

    /// Constructs objects of type typename iterator_traits<ForwardIt>
    /// ::value_type in the uninitialized storage designated by the range
    /// by default-initialization. If an exception is thrown during the
    /// initialization, the function has no effects.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first assignments.
    ///
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    ///
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct
    /// algorithm invoked without an execution policy object will execute in
    /// sequential order in the calling thread.
    ///
    /// \returns  The \a uninitialized_default_construct algorithm
    ///           returns nothing
    ///
    template <typename FwdIter>
    void uninitialized_default_construct(FwdIter first, FwdIter last);

    /// Constructs objects of type typename iterator_traits<ForwardIt>
    /// ::value_type in the uninitialized storage designated by the range
    /// by default-initialization. If an exception is thrown during the
    /// initialization, the function has no effects.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first assignments.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct
    /// algorithm invoked with an execution policy object of type \a
    /// sequenced_policy execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct
    /// algorithm invoked with an execution policy object of type \a
    /// parallel_policy or \a parallel_task_policy are permitted to execute
    /// in an unordered fashion in unspecified threads, and indeterminately
    /// sequenced within each thread.
    ///
    /// \returns  The \a uninitialized_default_construct algorithm returns a
    ///           \a pika::future<void>, if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and returns nothing
    ///           otherwise.
    ///
    template <typename ExPolicy, typename FwdIter>
    typename parallel::util::detail::algorithm_result<ExPolicy>::type
    uninitialized_default_construct(
        ExPolicy&& policy, FwdIter first, FwdIter last);

    /// Constructs objects of type typename iterator_traits<ForwardIt>
    /// ::value_type in the uninitialized storage designated by the range
    /// [first, first + count) by default-initialization. If an exception
    /// is thrown during the initialization, the function has no effects.
    ///
    /// \note   Complexity: Performs exactly \a count assignments, if
    ///         count > 0, no assignments otherwise.
    ///
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    /// \tparam Size        The type of the argument specifying the number of
    ///                     elements to apply \a f to.
    ///
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements starting at
    ///                     \a first the algorithm will be applied to.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct_n
    /// algorithm invoked without an execution policy object execute in
    /// sequential order in the calling thread.
    ///
    /// \returns  The \a uninitialized_default_construct_n algorithm returns a
    ///           returns \a FwdIter.
    ///           The \a uninitialized_default_construct_n algorithm returns
    ///           the iterator to the element in the source range, one past
    ///           the last element constructed.
    ///
    template <typename FwdIter, typename Size>
    FwdIter uninitialized_default_construct_n(FwdIter first, Size count);

    /// Constructs objects of type typename iterator_traits<ForwardIt>
    /// ::value_type in the uninitialized storage designated by the range
    /// [first, first + count) by default-initialization. If an exception
    /// is thrown during the initialization, the function has no effects.
    ///
    /// \note   Complexity: Performs exactly \a count assignments, if
    ///         count > 0, no assignments otherwise.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    /// \tparam Size        The type of the argument specifying the number of
    ///                     elements to apply \a f to.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements starting at
    ///                     \a first the algorithm will be applied to.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct_n
    /// algorithm invoked with an execution policy object of type
    /// \a sequenced_policy execute in sequential order in the
    /// calling thread.
    ///
    /// The assignments in the parallel \a uninitialized_default_construct_n
    /// algorithm invoked with an execution policy object of type
    /// \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an
    /// unordered fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a uninitialized_default_construct_n algorithm returns a
    ///           \a pika::future<FwdIter> if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a FwdIter otherwise.
    ///           The \a uninitialized_default_construct_n algorithm returns
    ///           the iterator to the element in the source range, one past
    ///           the last element constructed.
    ///
    template <typename ExPolicy, typename FwdIter, typename Size>
    typename parallel::util::detail::algorithm_result<ExPolicy, FwdIter>::type
    uninitialized_default_construct_n(
        ExPolicy&& policy, FwdIter first, Size count);
}    // namespace pika

#else    // DOXYGEN

#include <pika/config.hpp>
#include <pika/iterator_support/traits/is_iterator.hpp>
#include <pika/type_support/void_guard.hpp>

#include <pika/execution/algorithms/detail/is_negative.hpp>
#include <pika/executors/execution_policy.hpp>
#include <pika/parallel/algorithms/detail/dispatch.hpp>
#include <pika/parallel/algorithms/detail/distance.hpp>
#include <pika/parallel/util/detail/algorithm_result.hpp>
#include <pika/parallel/util/detail/sender_util.hpp>
#include <pika/parallel/util/loop.hpp>
#include <pika/parallel/util/partitioner_with_cleanup.hpp>
#include <pika/parallel/util/zip_iterator.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace pika { namespace parallel { inline namespace v1 {
    ///////////////////////////////////////////////////////////////////////////
    // uninitialized_default_construct
    namespace detail {
        /// \cond NOINTERNAL

        // provide our own implementation of std::uninitialized_default_construct as some
        // versions of MSVC horribly fail at compiling it for some types T
        template <typename Iter, typename Sent>
        Iter std_uninitialized_default_construct(Iter first, Sent last)
        {
            using value_type = typename std::iterator_traits<Iter>::value_type;

            Iter s_first = first;
            try
            {
                for (/* */; first != last; ++first)
                {
                    ::new (std::addressof(*first)) value_type;
                }
                return first;
            }
            catch (...)
            {
                for (/* */; s_first != first; ++s_first)
                {
                    (*s_first).~value_type();
                }
                throw;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename InIter>
        InIter sequential_uninitialized_default_construct_n(InIter first,
            std::size_t count,
            util::cancellation_token<util::detail::no_data>& tok)
        {
            using value_type =
                typename std::iterator_traits<InIter>::value_type;

            return util::loop_with_cleanup_n_with_token(
                first, count, tok,
                [](InIter it) -> void {
                    ::new (std::addressof(*it)) value_type;
                },
                [](InIter it) -> void { (*it).~value_type(); });
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename ExPolicy, typename FwdIter>
        typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
        parallel_sequential_uninitialized_default_construct_n(
            ExPolicy&& policy, FwdIter first, std::size_t count)
        {
            if (count == 0)
            {
                return util::detail::algorithm_result<ExPolicy, FwdIter>::get(
                    PIKA_MOVE(first));
            }

            typedef std::pair<FwdIter, FwdIter> partition_result_type;
            typedef
                typename std::iterator_traits<FwdIter>::value_type value_type;

            util::cancellation_token<util::detail::no_data> tok;
            return util::partitioner_with_cleanup<ExPolicy, FwdIter,
                partition_result_type>::
                call(
                    PIKA_FORWARD(ExPolicy, policy), first, count,
                    [tok](FwdIter it, std::size_t part_size) mutable
                    -> partition_result_type {
                        return std::make_pair(it,
                            sequential_uninitialized_default_construct_n(
                                it, part_size, tok));
                    },
                    // finalize, called once if no error occurred
                    [first, count](
                        std::vector<pika::future<partition_result_type>>&&
                            data) mutable -> FwdIter {
                        // make sure iterators embedded in function object that is
                        // attached to futures are invalidated
                        data.clear();

                        std::advance(first, count);
                        return first;
                    },
                    // cleanup function, called for each partition which
                    // didn't fail, but only if at least one failed
                    [](partition_result_type&& r) -> void {
                        while (r.first != r.second)
                        {
                            (*r.first).~value_type();
                            ++r.first;
                        }
                    });
        }
        ///////////////////////////////////////////////////////////////////////
        template <typename FwdIter>
        struct uninitialized_default_construct
          : public detail::algorithm<uninitialized_default_construct<FwdIter>,
                FwdIter>
        {
            uninitialized_default_construct()
              : uninitialized_default_construct::algorithm(
                    "uninitialized_default_construct")
            {
            }

            template <typename ExPolicy, typename Sent>
            static FwdIter sequential(ExPolicy, FwdIter first, Sent last)
            {
                return std_uninitialized_default_construct(first, last);
            }

            template <typename ExPolicy, typename Sent>
            static
                typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
                parallel(ExPolicy&& policy, FwdIter first, Sent last)
            {
                return parallel_sequential_uninitialized_default_construct_n(
                    PIKA_FORWARD(ExPolicy, policy), first,
                    detail::distance(first, last));
            }
        };
        /// \endcond
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    // uninitialized_default_construct_n
    namespace detail {
        /// \cond NOINTERNAL

        // provide our own implementation of std::uninitialized_default_construct as some
        // versions of MSVC horribly fail at compiling it for some types T
        template <typename InIter>
        InIter std_uninitialized_default_construct_n(
            InIter first, std::size_t count)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            InIter s_first = first;
            try
            {
                for (/* */; count != 0; (void) ++first, --count)
                {
                    ::new (std::addressof(*first)) value_type;
                }
                return first;
            }
            catch (...)
            {
                for (/* */; s_first != first; ++s_first)
                {
                    (*s_first).~value_type();
                }
                throw;
            }
        }

        template <typename FwdIter>
        struct uninitialized_default_construct_n
          : public detail::algorithm<uninitialized_default_construct_n<FwdIter>,
                FwdIter>
        {
            uninitialized_default_construct_n()
              : uninitialized_default_construct_n::algorithm(
                    "uninitialized_default_construct_n")
            {
            }

            template <typename ExPolicy>
            static FwdIter sequential(
                ExPolicy, FwdIter first, std::size_t count)
            {
                return std_uninitialized_default_construct_n(first, count);
            }

            template <typename ExPolicy>
            static
                typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
                parallel(ExPolicy&& policy, FwdIter first, std::size_t count)
            {
                return parallel_sequential_uninitialized_default_construct_n(
                    PIKA_FORWARD(ExPolicy, policy), first, count);
            }
        };
        /// \endcond
    }    // namespace detail
}}}      // namespace pika::parallel::v1

namespace pika {
    ///////////////////////////////////////////////////////////////////////////
    // DPO for pika::uninitialized_default_construct
    inline constexpr struct uninitialized_default_construct_t final
      : pika::detail::tag_parallel_algorithm<uninitialized_default_construct_t>
    {
        // clang-format off
        template <typename FwdIter,
            PIKA_CONCEPT_REQUIRES_(
                pika::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend void tag_fallback_invoke(pika::uninitialized_default_construct_t,
            FwdIter first, FwdIter last)
        {
            static_assert(pika::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            pika::parallel::v1::detail::uninitialized_default_construct<
                FwdIter>()
                .call(pika::execution::seq, first, last);
        }

        // clang-format off
        template <typename ExPolicy, typename FwdIter,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend typename parallel::util::detail::algorithm_result<ExPolicy>::type
        tag_fallback_invoke(pika::uninitialized_default_construct_t,
            ExPolicy&& policy, FwdIter first, FwdIter last)
        {
            static_assert(pika::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            using result_type =
                typename pika::parallel::util::detail::algorithm_result<
                    ExPolicy>::type;

            return pika::util::void_guard<result_type>(),
                   pika::parallel::v1::detail::uninitialized_default_construct<
                       FwdIter>()
                       .call(PIKA_FORWARD(ExPolicy, policy), first, last);
        }

    } uninitialized_default_construct{};

    ///////////////////////////////////////////////////////////////////////////
    // DPO for pika::uninitialized_default_construct_n
    inline constexpr struct uninitialized_default_construct_n_t final
      : pika::detail::tag_parallel_algorithm<
            uninitialized_default_construct_n_t>
    {
        // clang-format off
        template <typename FwdIter, typename Size,
            PIKA_CONCEPT_REQUIRES_(
                pika::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend FwdIter tag_fallback_invoke(
            pika::uninitialized_default_construct_n_t, FwdIter first,
            Size count)
        {
            static_assert(pika::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            // if count is representing a negative value, we do nothing
            if (pika::parallel::v1::detail::is_negative(count))
            {
                return first;
            }

            return pika::parallel::v1::detail::
                uninitialized_default_construct_n<FwdIter>()
                    .call(pika::execution::seq, first, std::size_t(count));
        }

        // clang-format off
        template <typename ExPolicy, typename FwdIter, typename Size,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend typename parallel::util::detail::algorithm_result<ExPolicy,
            FwdIter>::type
        tag_fallback_invoke(pika::uninitialized_default_construct_n_t,
            ExPolicy&& policy, FwdIter first, Size count)
        {
            static_assert(pika::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            // if count is representing a negative value, we do nothing
            if (pika::parallel::v1::detail::is_negative(count))
            {
                return parallel::util::detail::algorithm_result<ExPolicy,
                    FwdIter>::get(PIKA_MOVE(first));
            }

            return pika::parallel::v1::detail::
                uninitialized_default_construct_n<FwdIter>()
                    .call(PIKA_FORWARD(ExPolicy, policy), first,
                        std::size_t(count));
        }

    } uninitialized_default_construct_n{};
}    // namespace pika

#endif    // DOXYGEN
