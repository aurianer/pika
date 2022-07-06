//  Copyright (c) 2015 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/init.hpp>
#include <pika/modules/iterator_support.hpp>
#include <pika/modules/timing.hpp>
#include <pika/testing.hpp>
#include <pika/tuple.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
int test_count = 100;
int partition_size = 10000;

///////////////////////////////////////////////////////////////////////////////
namespace pika { namespace experimental { namespace detail {
    template <typename Iterator>
    PIKA_FORCEINLINE Iterator previous(Iterator it, std::false_type)
    {
        return --it;
    }

    template <typename Iterator>
    PIKA_FORCEINLINE Iterator previous(Iterator const& it, std::true_type)
    {
        return it - 1;
    }

    template <typename Iterator>
    PIKA_FORCEINLINE Iterator previous(Iterator const& it)
    {
        return previous(
            it, pika::traits::is_random_access_iterator<Iterator>());
    }

    template <typename Iterator>
    PIKA_FORCEINLINE Iterator next(Iterator it, std::false_type)
    {
        return ++it;
    }

    template <typename Iterator>
    PIKA_FORCEINLINE Iterator next(Iterator const& it, std::true_type)
    {
        return it + 1;
    }

    template <typename Iterator>
    PIKA_FORCEINLINE Iterator next(Iterator const& it)
    {
        return next(it, pika::traits::is_random_access_iterator<Iterator>());
    }
}}}    // namespace pika::experimental::detail

///////////////////////////////////////////////////////////////////////////////
// Version of stencil3_iterator which handles boundary elements internally
namespace pika { namespace experimental {
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename IterBegin = Iterator,
        typename IterValueBegin = Iterator, typename IterEnd = IterBegin,
        typename IterValueEnd = IterValueBegin>
    class stencil3_iterator_full;

    namespace detail {
        ///////////////////////////////////////////////////////////////////////
        template <typename IteratorBase, typename IteratorValue>
        struct previous_transformer
        {
            previous_transformer() {}

            // at position 'begin' it will dereference 'value', otherwise 'it-1'
            previous_transformer(
                IteratorBase const& begin, IteratorValue const& value)
              : begin_(begin)
              , value_(value)
            {
            }

            template <typename Iterator>
            typename std::iterator_traits<Iterator>::reference operator()(
                Iterator const& it) const
            {
                if (it == begin_)
                    return *value_;
                return *detail::previous(it);
            }

        private:
            IteratorBase begin_;
            IteratorValue value_;
        };

        template <typename IteratorBase, typename IteratorValue>
        inline previous_transformer<IteratorBase, IteratorValue>
        make_previous_transformer(
            IteratorBase const& base, IteratorValue const& value)
        {
            return previous_transformer<IteratorBase, IteratorValue>(
                base, value);
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename IteratorBase, typename IteratorValue>
        struct next_transformer
        {
            next_transformer() {}

            // at position 'end' it will dereference 'value', otherwise 'it+1'
            next_transformer(
                IteratorBase const& end, IteratorValue const& value)
              : end_(end)
              , value_(value)
            {
            }

            template <typename Iterator>
            typename std::iterator_traits<Iterator>::reference operator()(
                Iterator const& it) const
            {
                if (it == end_)
                    return *value_;
                return *detail::next(it);
            }

        private:
            IteratorBase end_;
            IteratorValue value_;
        };

        template <typename IteratorBase, typename IteratorValue>
        inline next_transformer<IteratorBase, IteratorValue>
        make_next_transformer(
            IteratorBase const& base, IteratorValue const& value)
        {
            return next_transformer<IteratorBase, IteratorValue>(base, value);
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename Iterator, typename IterBegin,
            typename IterValueBegin, typename IterEnd, typename IterValueEnd>
        struct stencil3_iterator_base
        {
            using left_transformer =
                previous_transformer<IterBegin, IterValueBegin>;
            using right_transformer = next_transformer<IterEnd, IterValueEnd>;

            using left_iterator =
                util::transform_iterator<Iterator, left_transformer>;
            using right_iterator =
                util::transform_iterator<Iterator, right_transformer>;

            using type = util::detail::zip_iterator_base<
                pika::tuple<left_iterator, Iterator, right_iterator>,
                stencil3_iterator_full<Iterator, IterBegin, IterValueBegin,
                    IterEnd, IterValueEnd>>;

            static type create(Iterator const& it, IterBegin const& begin,
                IterValueBegin const& begin_val, IterEnd const& end,
                IterValueEnd const& end_val)
            {
                auto prev = make_previous_transformer(begin, begin_val);
                auto next = make_next_transformer(end, end_val);

                return type(
                    pika::make_tuple(util::make_transform_iterator(it, prev),
                        it, util::make_transform_iterator(it, next)));
            }

            static type create(Iterator const& it)
            {
                return type(pika::make_tuple(
                    util::make_transform_iterator(it, left_transformer()), it,
                    util::make_transform_iterator(it, right_transformer())));
            }
        };
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename IterBegin, typename IterValueBegin,
        typename IterEnd, typename IterValueEnd>
    class stencil3_iterator_full
      : public detail::stencil3_iterator_base<Iterator, IterBegin,
            IterValueBegin, IterEnd, IterValueEnd>::type
    {
    private:
        using base_maker = detail::stencil3_iterator_base<Iterator, IterBegin,
            IterValueBegin, IterEnd, IterValueEnd>;
        using base_type = typename base_maker::type;

    public:
        stencil3_iterator_full() {}

        stencil3_iterator_full(Iterator const& it, IterBegin const& begin,
            IterValueBegin const& begin_val, IterEnd const& end,
            IterValueEnd const& end_val)
          : base_type(base_maker::create(it, begin, begin_val, end, end_val))
        {
        }

        explicit stencil3_iterator_full(Iterator const& it)
          : base_type(base_maker::create(it))
        {
        }

    private:
        friend class pika::util::iterator_core_access;

        bool equal(stencil3_iterator_full const& other) const
        {
            return pika::get<1>(this->get_iterator_tuple()) ==
                pika::get<1>(other.get_iterator_tuple());
        }
    };

    template <typename Iterator, typename IterBegin, typename IterValueBegin,
        typename IterEnd, typename IterValueEnd>
    inline stencil3_iterator_full<Iterator, IterBegin, IterValueBegin, IterEnd,
        IterValueEnd>
    make_stencil3_full_iterator(Iterator const& it, IterBegin const& begin,
        IterValueBegin const& begin_val, IterEnd const& end,
        IterValueEnd const& end_val)
    {
        using result_type = stencil3_iterator_full<Iterator, IterBegin,
            IterValueBegin, IterEnd, IterValueEnd>;
        return result_type(it, begin, begin_val, end, end_val);
    }

    template <typename StencilIterator, typename Iterator>
    inline StencilIterator make_stencil3_full_iterator(Iterator const& it)
    {
        return StencilIterator(it);
    }

    template <typename Iterator, typename IterValue>
    inline std::pair<stencil3_iterator_full<Iterator, Iterator, IterValue,
                         Iterator, IterValue>,
        stencil3_iterator_full<Iterator, Iterator, IterValue, Iterator,
            IterValue>>
    make_stencil3_full_range(Iterator const& begin, Iterator const& end,
        IterValue const& begin_val, IterValue const& end_val)
    {
        auto b = make_stencil3_full_iterator(
            begin, begin, begin_val, detail::previous(end), end_val);
        return std::make_pair(b, make_stencil3_full_iterator<decltype(b)>(end));
    }
}}    // namespace pika::experimental

std::chrono::duration<double> bench_stencil3_iterator_full()
{
    std::vector<int> values(partition_size);
    std::iota(std::begin(values), std::end(values), 0);

    auto start = std::chrono::high_resolution_clock::now();

    auto r = pika::experimental::make_stencil3_full_range(
        values.begin(), values.end(), &values.back(), &values.front());

    using reference = std::iterator_traits<decltype(r.first)>::reference;

    int result = 0;

    std::for_each(r.first, r.second, [&result](reference val) {
        using pika::get;
        result += get<0>(val) + get<1>(val) + get<2>(val);
    });

    return std::chrono::high_resolution_clock::now() - start;
}

///////////////////////////////////////////////////////////////////////////////
// compare with unchecked stencil3_iterator (version 1)
namespace pika { namespace experimental {
    template <typename Iterator>
    class stencil3_iterator_v1
      : public util::detail::zip_iterator_base<
            pika::tuple<Iterator, Iterator, Iterator>,
            stencil3_iterator_v1<Iterator>>
    {
    private:
        using base_type = util::detail::zip_iterator_base<
            pika::tuple<Iterator, Iterator, Iterator>,
            stencil3_iterator_v1<Iterator>>;

    public:
        stencil3_iterator_v1() {}

        explicit stencil3_iterator_v1(Iterator const& it)
          : base_type(
                pika::make_tuple(detail::previous(it), it, detail::next(it)))
        {
        }

    private:
        friend class pika::util::iterator_core_access;

        bool equal(stencil3_iterator_v1 const& other) const
        {
            return pika::get<1>(this->get_iterator_tuple()) ==
                pika::get<1>(other.get_iterator_tuple());
        }
    };

    template <typename Iterator>
    inline stencil3_iterator_v1<Iterator> make_stencil3_iterator_v1(
        Iterator const& it)
    {
        return stencil3_iterator_v1<Iterator>(it);
    }

    template <typename Iterator>
    inline std::pair<stencil3_iterator_v1<Iterator>,
        stencil3_iterator_v1<Iterator>>
    make_stencil3_range_v1(Iterator const& begin, Iterator const& end)
    {
        return std::make_pair(
            make_stencil3_iterator_v1(begin), make_stencil3_iterator_v1(end));
    }
}}    // namespace pika::experimental

std::chrono::duration<double> bench_stencil3_iterator_v1()
{
    std::vector<int> values(partition_size);
    std::iota(std::begin(values), std::end(values), 0);

    auto start = std::chrono::high_resolution_clock::now();

    auto r = pika::experimental::make_stencil3_range_v1(
        values.begin() + 1, values.end() - 1);

    using reference = std::iterator_traits<decltype(r.first)>::reference;

    // handle boundary elements explicitly
    int result = values.back() + values.front() + values[1];

    std::for_each(r.first, r.second, [&result](reference val) {
        using pika::get;
        result += get<0>(val) + get<1>(val) + get<2>(val);
    });

    result += values[partition_size - 2] + values.back() + values.front();
    PIKA_UNUSED(result);

    return std::chrono::high_resolution_clock::now() - start;
}

///////////////////////////////////////////////////////////////////////////////
// compare with unchecked stencil3_iterator (version 2)
namespace pika { namespace experimental {
    namespace detail {
        struct stencil_transformer_v2
        {
            template <typename Iterator>
            struct result
            {
                using element_type =
                    typename std::iterator_traits<Iterator>::reference;
                using type =
                    pika::tuple<element_type, element_type, element_type>;
            };

            // it will dereference tuple(it-1, it, it+1)
            template <typename Iterator>
            typename result<Iterator>::type operator()(Iterator const& it) const
            {
                using type = typename result<Iterator>::type;
                return type(*detail::previous(it), *it, *detail::next(it));
            }
        };
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator,
        typename Transformer = detail::stencil_transformer_v2>
    class stencil3_iterator_v2
      : public pika::util::transform_iterator<Iterator, Transformer>
    {
    private:
        using base_type = pika::util::transform_iterator<Iterator, Transformer>;

    public:
        stencil3_iterator_v2() {}

        explicit stencil3_iterator_v2(Iterator const& it)
          : base_type(it, Transformer())
        {
        }

        stencil3_iterator_v2(Iterator const& it, Transformer const& t)
          : base_type(it, t)
        {
        }
    };

    template <typename Iterator, typename Transformer>
    inline stencil3_iterator_v2<Iterator, Transformer>
    make_stencil3_iterator_v2(Iterator const& it, Transformer const& t)
    {
        return stencil3_iterator_v2<Iterator, Transformer>(it, t);
    }

    template <typename Iterator, typename Transformer>
    inline std::pair<stencil3_iterator_v2<Iterator, Transformer>,
        stencil3_iterator_v2<Iterator, Transformer>>
    make_stencil3_range_v2(
        Iterator const& begin, Iterator const& end, Transformer const& t)
    {
        return std::make_pair(make_stencil3_iterator_v2(begin, t),
            make_stencil3_iterator_v2(end, t));
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    inline stencil3_iterator_v2<Iterator> make_stencil3_iterator_v2(
        Iterator const& it)
    {
        return stencil3_iterator_v2<Iterator>(it);
    }

    template <typename Iterator>
    inline std::pair<stencil3_iterator_v2<Iterator>,
        stencil3_iterator_v2<Iterator>>
    make_stencil3_range_v2(Iterator const& begin, Iterator const& end)
    {
        return std::make_pair(
            make_stencil3_iterator_v2(begin), make_stencil3_iterator_v2(end));
    }
}}    // namespace pika::experimental

std::chrono::duration<double> bench_stencil3_iterator_v2()
{
    std::vector<int> values(partition_size);
    std::iota(std::begin(values), std::end(values), 0);

    auto start = std::chrono::high_resolution_clock::now();

    auto r = pika::experimental::make_stencil3_range_v2(
        values.begin() + 1, values.end() - 1);

    using reference = std::iterator_traits<decltype(r.first)>::reference;

    // handle boundary elements explicitly
    int result = values.back() + values.front() + values[1];

    std::for_each(r.first, r.second, [&result](reference val) {
        using pika::get;
        result += get<0>(val) + get<1>(val) + get<2>(val);
    });

    result += values[partition_size - 2] + values.back() + values.front();
    PIKA_UNUSED(result);

    return std::chrono::high_resolution_clock::now() - start;
}

///////////////////////////////////////////////////////////////////////////////
std::chrono::duration<double> bench_stencil3_iterator_explicit()
{
    std::vector<int> values(partition_size);
    std::iota(std::begin(values), std::end(values), 0);

    auto start = std::chrono::high_resolution_clock::now();

    // handle all elements explicitly
    int result = values.back() + values.front() + values[1];

    auto range = pika::detail::irange(1, partition_size - 1);

    std::for_each(
        std::begin(range), std::end(range), [&result, &values](std::size_t i) {
            result += values[i - 1] + values[i] + values[i + 1];
        });

    result += values[partition_size - 2] + values.back() + values.front();
    PIKA_UNUSED(result);

    return std::chrono::high_resolution_clock::now() - start;
}

///////////////////////////////////////////////////////////////////////////////
int pika_main(pika::program_options::variables_map& vm)
{
    //     bool csvoutput = vm.count("csv_output") != 0;

    test_count = vm["test_count"].as<int>();
    partition_size = vm["partition_size"].as<int>();

    // verify that input is within domain of program
    if (test_count <= 0)
    {
        std::cout << "test_count cannot be zero or negative..." << std::endl;
    }
    else if (partition_size < 3)
    {
        std::cout << "partition_size cannot be smaller than 3..." << std::endl;
    }
    else
    {
        // first run full stencil3 tests
        {
            std::chrono::duration<double> t(0);
            for (int i = 0; i != test_count; ++i)
                t += bench_stencil3_iterator_full();
            std::cout << "full: " << t.count() / test_count << std::endl;
            pika::util::print_cdash_timing(
                "Stencil3Full", t.count() / test_count);
        }

        // now run explicit (no-check) stencil3 tests
        {
            std::chrono::duration<double> t(0);
            for (int i = 0; i != test_count; ++i)
                t += bench_stencil3_iterator_v1();
            std::cout << "nocheck(v1): " << t.count() / test_count << std::endl;
            pika::util::print_cdash_timing(
                "Stencil3NocheckV1", t.count() / test_count);
        }

        {
            std::chrono::duration<double> t(0);
            for (int i = 0; i != test_count; ++i)
                t += bench_stencil3_iterator_v2();
            std::cout << "nocheck(v2): " << t.count() / test_count << std::endl;
            pika::util::print_cdash_timing(
                "Stencil3NocheckV2", t.count() / test_count);
        }

        // now run explicit tests
        {
            std::chrono::duration<double> t(0);
            for (int i = 0; i != test_count; ++i)
                t += bench_stencil3_iterator_explicit();
            std::cout << "explicit: " << t.count() / test_count << std::endl;
            pika::util::print_cdash_timing(
                "Stencil3Explicit", t.count() / test_count);
        }
    }

    return pika::finalize();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // initialize program
    pika::program_options::options_description cmdline(
        "usage: " PIKA_APPLICATION_STRING " [options]");

    // clang-format off
    cmdline.add_options()
        ("test_count",
            pika::program_options::value<int>()->default_value(100),
            "number of tests to be averaged (default: 100)")
        ("partition_size",
            pika::program_options::value<int>()->default_value(10000),
            "number of elements to iterate over (default: 10000)")
        ;
    // clang-format on

    pika::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return pika::init(pika_main, argc, argv, init_args);
}
