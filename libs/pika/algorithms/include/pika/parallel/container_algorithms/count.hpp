//  Copyright (c) 2018 Christopher Ogle
//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/container_algorithms/count.hpp

#pragma once

#if defined(DOXYGEN)
namespace pika { namespace ranges {
    // clang-format off

    /// Returns the number of elements in the range [first, last) satisfying
    /// a specific criteria. This version counts the elements that are equal to
    /// the given \a value.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first comparisons.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the comparisons.
    /// \tparam Rng         The type of the source range used (deduced).
    ///                     The iterators extracted from this range type must
    ///                     meet the requirements of an input iterator.
    /// \tparam T           The type of the value to search for (deduced).
    /// \tparam Proj        The type of an optional projection function. This
    ///                     defaults to \a util::projection_identity
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param rng          Refers to the sequence of elements the algorithm
    ///                     will be applied to.
    /// \param value        The value to search for.
    /// \param proj         Specifies the function (or function object) which
    ///                     will be invoked for each of the elements as a
    ///                     projection operation before the actual predicate
    ///                     \a is invoked.
    ///
    /// The comparisons in the parallel \a count algorithm invoked with
    /// an execution policy object of type \a sequenced_policy
    /// execute in sequential order in the calling thread.
    ///
    /// \note The comparisons in the parallel \a count algorithm invoked with
    ///       an execution policy object of type \a parallel_policy or
    ///       \a parallel_task_policy are permitted to execute in an unordered
    ///       fashion in unspecified threads, and indeterminately sequenced
    ///       within each thread.
    ///
    /// \returns  The \a count algorithm returns a
    ///           \a pika::future<difference_type> if the execution policy is of
    ///           type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a difference_type otherwise (where \a difference_type
    ///           is defined by \a std::iterator_traits<FwdIter>::difference_type.
    ///           The \a count algorithm returns the number of elements
    ///           satisfying the given criteria.
    ///
    template <typename ExPolicy, typename Rng, typename T,
        typename Proj = util::projection_identity>
    typename util::detail::algorithm_result<ExPolicy,
        typename std::iterator_traits<
            typename pika::traits::range_traits<Rng>::iterator_type
        >::difference_type
    >::type
    count(ExPolicy&& policy, Rng&& rng, T const& value, Proj&& proj = Proj());

    /// Returns the number of elements in the range [first, last) satisfying
    /// a specific criteria. This version counts elements for which predicate
    /// \a f returns true.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first applications of
    ///         the predicate.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the comparisons.
    /// \tparam Rng         The type of the source range used (deduced).
    ///                     The iterators extracted from this range type must
    ///                     meet the requirements of an input iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a count_if requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    /// \tparam Proj        The type of an optional projection function. This
    ///                     defaults to \a util::projection_identity
    //
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param rng          Refers to the sequence of elements the algorithm
    ///                     will be applied to.
    /// \param f            Specifies the function (or function object) which
    ///                     will be invoked for each of the elements in the
    ///                     sequence specified by [first, last).This is an
    ///                     unary predicate which returns \a true for the
    ///                     required elements. The signature of this predicate
    ///                     should be equivalent to:
    ///                     \code
    ///                     bool pred(const Type &a);
    ///                     \endcode \n
    ///                     The signature does not need to have const&, but
    ///                     the function must not modify the objects passed to
    ///                     it. The type \a Type must be such that an object of
    ///                     type \a FwdIter can be dereferenced and then
    ///                     implicitly converted to Type.
    /// \param proj         Specifies the function (or function object) which
    ///                     will be invoked for each of the elements as a
    ///                     projection operation before the actual predicate
    ///                     \a is invoked.
    ///
    /// \note The assignments in the parallel \a count_if algorithm invoked with
    ///       an execution policy object of type \a sequenced_policy
    ///       execute in sequential order in the calling thread.
    /// \note The assignments in the parallel \a count_if algorithm invoked with
    ///       an execution policy object of type \a parallel_policy or
    ///       \a parallel_task_policy are permitted to execute in an unordered
    ///       fashion in unspecified threads, and indeterminately sequenced
    ///       within each thread.
    ///
    /// \returns  The \a count_if algorithm returns
    ///           \a pika::future<difference_type> if the execution policy is of
    ///           type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a difference_type otherwise (where \a difference_type
    ///           is defined by \a std::iterator_traits<FwdIter>::difference_type.
    ///           The \a count algorithm returns the number of elements
    ///           satisfying the given criteria.
    ///
    template <typename ExPolicy, typename Rng, typename F,
        typename Proj = util::projection_identity>
    typename util::detail::algorithm_result<ExPolicy,
        typename std::iterator_traits<
            typename pika::traits::range_traits<Rng>::iterator_type
        >::difference_type
    >::type
    count_if(ExPolicy&& policy, Rng&& rng, F&& f, Proj&& proj = Proj());

    // clang-format on
}}    // namespace pika::ranges

#else    // DOXYGEN

#include <pika/config.hpp>
#include <pika/concepts/concepts.hpp>
#include <pika/iterator_support/range.hpp>
#include <pika/iterator_support/traits/is_range.hpp>

#include <pika/algorithms/traits/projected_range.hpp>
#include <pika/parallel/algorithms/count.hpp>
#include <pika/parallel/util/projection_identity.hpp>

#include <type_traits>
#include <utility>

namespace pika::ranges {
    ///////////////////////////////////////////////////////////////////////////
    // CPO for pika::ranges::count
    inline constexpr struct count_t final
      : pika::detail::tag_parallel_algorithm<count_t>
    {
    private:
        // clang-format off
        template <typename ExPolicy, typename Rng,
            typename Proj = pika::parallel::util::projection_identity,
            typename T = typename pika::parallel::detail::projected<
                pika::traits::range_iterator_t<Rng>, Proj>::value_type,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::parallel::detail::is_projected_range<Proj, Rng>::value &&
                pika::traits::is_range<Rng>::value
            )>
        // clang-format on
        friend typename pika::parallel::util::detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<typename pika::traits::range_traits<
                Rng>::iterator_type>::difference_type>::type
        tag_fallback_invoke(count_t, ExPolicy&& policy, Rng&& rng,
            T const& value, Proj&& proj = Proj())
        {
            using iterator_type =
                typename pika::traits::range_traits<Rng>::iterator_type;

            static_assert(
                (pika::traits::is_forward_iterator<iterator_type>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<iterator_type>::difference_type;

            return pika::parallel::detail::count<difference_type>().call(
                PIKA_FORWARD(ExPolicy, policy), pika::util::begin(rng),
                pika::util::end(rng), value, PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename ExPolicy, typename Iter, typename Sent,
            typename Proj = pika::parallel::util::projection_identity,
            typename T = typename pika::parallel::detail::projected<Iter,
                Proj>::value_type,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::traits::is_sentinel_for<Sent, Iter>::value
            )>
        // clang-format on
        friend typename pika::parallel::util::detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<Iter>::difference_type>::type
        tag_fallback_invoke(count_t, ExPolicy&& policy, Iter first, Sent last,
            T const& value, Proj&& proj = Proj())
        {
            static_assert((pika::traits::is_forward_iterator<Iter>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<Iter>::difference_type;

            return pika::parallel::detail::count<difference_type>().call(
                PIKA_FORWARD(ExPolicy, policy), first, last, value,
                PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename Rng,
            typename Proj = pika::parallel::util::projection_identity,
            typename T = typename pika::parallel::detail::projected<
                pika::traits::range_iterator_t<Rng>, Proj>::value_type,
            PIKA_CONCEPT_REQUIRES_(
                pika::parallel::detail::is_projected_range<Proj, Rng>::value &&
                pika::traits::is_range<Rng>::value
            )>
        // clang-format on
        friend
            typename std::iterator_traits<typename pika::traits::range_traits<
                Rng>::iterator_type>::difference_type
            tag_fallback_invoke(
                count_t, Rng&& rng, T const& value, Proj&& proj = Proj())
        {
            using iterator_type =
                typename pika::traits::range_traits<Rng>::iterator_type;

            static_assert(
                (pika::traits::is_input_iterator<iterator_type>::value),
                "Required at least input iterator.");

            using difference_type =
                typename std::iterator_traits<iterator_type>::difference_type;

            return pika::parallel::detail::count<difference_type>().call(
                pika::execution::seq, pika::util::begin(rng),
                pika::util::end(rng), value, PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename Iter, typename Sent,
            typename Proj = pika::parallel::util::projection_identity,
            typename T = typename pika::parallel::detail::projected<Iter,
                Proj>::value_type,
            PIKA_CONCEPT_REQUIRES_(
                pika::traits::is_sentinel_for<Sent, Iter>::value
            )>
        // clang-format on
        friend typename std::iterator_traits<Iter>::difference_type
        tag_fallback_invoke(count_t, Iter first, Sent last, T const& value,
            Proj&& proj = Proj())
        {
            static_assert((pika::traits::is_input_iterator<Iter>::value),
                "Required at least input iterator.");

            using difference_type =
                typename std::iterator_traits<Iter>::difference_type;

            return pika::parallel::detail::count<difference_type>().call(
                pika::execution::seq, first, last, value,
                PIKA_FORWARD(Proj, proj));
        }
    } count{};

    ///////////////////////////////////////////////////////////////////////////
    // CPO for pika::ranges::count_if
    inline constexpr struct count_if_t final
      : pika::detail::tag_parallel_algorithm<count_if_t>
    {
    private:
        // clang-format off
        template <typename ExPolicy, typename Rng, typename F,
            typename Proj = pika::parallel::util::projection_identity,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::traits::is_range<Rng>::value &&
                pika::parallel::detail::is_projected_range<Proj, Rng>::value &&
                pika::parallel::detail::is_indirect_callable<ExPolicy, F,
                    pika::parallel::detail::projected_range<Proj, Rng>
                >::value
            )>
        // clang-format on
        friend typename pika::parallel::util::detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<typename pika::traits::range_traits<
                Rng>::iterator_type>::difference_type>::type
        tag_fallback_invoke(count_if_t, ExPolicy&& policy, Rng&& rng, F&& f,
            Proj&& proj = Proj())
        {
            using iterator_type =
                typename pika::traits::range_traits<Rng>::iterator_type;

            static_assert(
                (pika::traits::is_forward_iterator<iterator_type>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<iterator_type>::difference_type;

            return pika::parallel::detail::count_if<difference_type>().call(
                PIKA_FORWARD(ExPolicy, policy), pika::util::begin(rng),
                pika::util::end(rng), PIKA_FORWARD(F, f),
                PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename ExPolicy, typename Iter, typename Sent, typename F,
            typename Proj = pika::parallel::util::projection_identity,
            PIKA_CONCEPT_REQUIRES_(
                pika::is_execution_policy<ExPolicy>::value &&
                pika::traits::is_sentinel_for<Sent, Iter>::value &&
                pika::parallel::detail::is_projected<Proj, Iter>::value &&
                pika::parallel::detail::is_indirect_callable<ExPolicy, F,
                    pika::parallel::detail::projected<Proj, Iter>
                >::value
            )>
        // clang-format on
        friend typename pika::parallel::util::detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<Iter>::difference_type>::type
        tag_fallback_invoke(count_if_t, ExPolicy&& policy, Iter first,
            Sent last, F&& f, Proj&& proj = Proj())
        {
            static_assert((pika::traits::is_forward_iterator<Iter>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<Iter>::difference_type;

            return pika::parallel::detail::count_if<difference_type>().call(
                PIKA_FORWARD(ExPolicy, policy), first, last, PIKA_FORWARD(F, f),
                PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename Rng, typename F,
            typename Proj = pika::parallel::util::projection_identity,
            PIKA_CONCEPT_REQUIRES_(
                pika::traits::is_range<Rng>::value &&
                pika::parallel::detail::is_projected_range<Proj, Rng>::value &&
                pika::parallel::detail::is_indirect_callable<
                    pika::execution::sequenced_policy, F,
                    pika::parallel::detail::projected_range<Proj, Rng>
                >::value
            )>
        // clang-format on
        friend
            typename std::iterator_traits<typename pika::traits::range_traits<
                Rng>::iterator_type>::difference_type
            tag_fallback_invoke(
                count_if_t, Rng&& rng, F&& f, Proj&& proj = Proj())
        {
            using iterator_type =
                typename pika::traits::range_traits<Rng>::iterator_type;

            static_assert(
                (pika::traits::is_forward_iterator<iterator_type>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<iterator_type>::difference_type;

            return pika::parallel::detail::count_if<difference_type>().call(
                pika::execution::seq, pika::util::begin(rng),
                pika::util::end(rng), PIKA_FORWARD(F, f),
                PIKA_FORWARD(Proj, proj));
        }

        // clang-format off
        template <typename Iter, typename Sent, typename F,
            typename Proj = pika::parallel::util::projection_identity,
            PIKA_CONCEPT_REQUIRES_(
                pika::traits::is_sentinel_for<Sent, Iter>::value &&
                pika::parallel::detail::is_projected<Proj, Iter>::value &&
                pika::parallel::detail::is_indirect_callable<
                    pika::execution::sequenced_policy, F,
                    pika::parallel::detail::projected<Proj, Iter>
                >::value
            )>
        // clang-format on
        friend typename std::iterator_traits<Iter>::difference_type
        tag_fallback_invoke(
            count_if_t, Iter first, Sent last, F&& f, Proj&& proj = Proj())
        {
            static_assert((pika::traits::is_forward_iterator<Iter>::value),
                "Required at least forward iterator.");

            using difference_type =
                typename std::iterator_traits<Iter>::difference_type;

            return pika::parallel::detail::count_if<difference_type>().call(
                pika::execution::seq, first, last, PIKA_FORWARD(F, f),
                PIKA_FORWARD(Proj, proj));
        }
    } count_if{};

}    // namespace pika::ranges
#endif    // DOXYGEN
