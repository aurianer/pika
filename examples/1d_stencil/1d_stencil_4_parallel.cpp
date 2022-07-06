//  Copyright (c) 2014 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the fourth in a series of examples demonstrating the development of
// a fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds on example three. It futurizes the code from that
// example. Compared to example two this code runs much more efficiently. It
// allows for changing the amount of work executed in one pika thread which
// enables tuning the performance for the optimal grain size of the
// computation. This example is still fully local but demonstrates nice
// scalability on SMP machines.

#include <pika/algorithm.hpp>
#include <pika/chrono.hpp>
#include <pika/future.hpp>
#include <pika/init.hpp>
#include <pika/modules/iterator_support.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "print_time_results.hpp"

///////////////////////////////////////////////////////////////////////////////
bool header = true;    // print csv heading
double k = 0.5;        // heat transfer coefficient
double dt = 1.;        // time step
double dx = 1.;        // grid spacing

inline std::size_t idx(std::size_t i, std::size_t size)
{
    return (std::int64_t(i) < 0) ? (i + size) % size : i % size;
}

///////////////////////////////////////////////////////////////////////////////
// Our partition data type
struct partition_data
{
    explicit partition_data(std::size_t size)
      : data_(new double[size])
      , size_(size)
    {
    }

    partition_data(std::size_t size, double initial_value)
      : data_(new double[size])
      , size_(size)
    {
        double base_value = initial_value * static_cast<double>(size);
        for (std::size_t i = 0; i != size; ++i)
            data_[static_cast<std::ptrdiff_t>(i)] = base_value + double(i);
    }

    double& operator[](std::size_t idx)
    {
        return data_[static_cast<std::ptrdiff_t>(idx)];
    }
    double operator[](std::size_t idx) const
    {
        return data_[static_cast<std::ptrdiff_t>(idx)];
    }

    std::size_t size() const
    {
        return size_;
    }

private:
    std::shared_ptr<double[]> data_;
    std::size_t size_;
};

std::ostream& operator<<(std::ostream& os, partition_data const& c)
{
    os << "{";
    for (std::size_t i = 0; i != c.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << c[i];
    }
    os << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
struct stepper
{
    // Our data for one time step
    using partition = pika::shared_future<partition_data>;
    using space = std::vector<partition>;

    // Our operator
    static double heat(double left, double middle, double right)
    {
        return middle + (k * dt / (dx * dx)) * (left - 2 * middle + right);
    }

    // The partitioned operator, it invokes the heat operator above on all
    // elements of a partition.
    static partition_data heat_part(partition_data const& left,
        partition_data const& middle, partition_data const& right)
    {
        std::size_t size = middle.size();
        partition_data next(size);

        using iterator = pika::util::counting_iterator<std::size_t>;

        next[0] = heat(left[size - 1], middle[0], middle[1]);

        pika::for_each(pika::execution::par, iterator(1), iterator(size - 1),
            [&next, &middle](std::size_t i) {
                next[i] = heat(middle[i - 1], middle[i], middle[i + 1]);
            });

        next[size - 1] = heat(middle[size - 2], middle[size - 1], right[0]);

        return next;
    }

    // do all the work on 'np' partitions, 'nx' data points each, for 'nt'
    // time steps
    pika::future<space> do_work(std::size_t np, std::size_t nx, std::size_t nt)
    {
        // U[t][i] is the state of position i at time t.
        std::vector<space> U(2);
        for (space& s : U)
            s.resize(np);

        // Initial conditions: f(0, i) = i
        for (std::size_t i = 0; i != np; ++i)
            U[0][i] = pika::make_ready_future(partition_data(nx, double(i)));

        auto Op = pika::unwrapping(&stepper::heat_part);

        // Actual time step loop
        for (std::size_t t = 0; t != nt; ++t)
        {
            space const& current = U[t % 2];
            space& next = U[(t + 1) % 2];

            using iterator = pika::util::counting_iterator<std::size_t>;

            pika::for_each(pika::execution::par, iterator(0), iterator(np),
                [&next, &current, np, &Op](std::size_t i) {
                    next[i] = pika::dataflow(pika::launch::async, Op,
                        current[idx(i - 1, np)], current[i],
                        current[idx(i + 1, np)]);
                });
        }

        // Return the solution at time-step 'nt'.
        return pika::when_all(U[nt % 2]);
    }
};

///////////////////////////////////////////////////////////////////////////////
int pika_main(pika::program_options::variables_map& vm)
{
    std::uint64_t np = vm["np"].as<std::uint64_t>();    // Number of partitions.
    std::uint64_t nx =
        vm["nx"].as<std::uint64_t>();    // Number of grid points.
    std::uint64_t nt = vm["nt"].as<std::uint64_t>();    // Number of steps.

    if (vm.count("no-header"))
        header = false;

    // Create the stepper object
    stepper step;

    using namespace std::chrono;
    // Measure execution time.
    auto t = high_resolution_clock::now();

    // Execute nt time steps on nx grid points and print the final solution.
    pika::future<stepper::space> result = step.do_work(np, nx, nt);

    stepper::space solution = result.get();
    pika::wait_all(solution);

    double elapsed = duration<double>(high_resolution_clock::now() - t).count();

    // Print the final solution
    if (vm.count("result"))
    {
        for (std::size_t i = 0; i != np; ++i)
            std::cout << "U[" << i << "] = " << solution[i].get() << std::endl;
    }

    std::uint64_t const os_thread_count = pika::get_os_thread_count();
    print_time_results(os_thread_count, elapsed, nx, np, nt, header);

    return pika::finalize();
}

int main(int argc, char* argv[])
{
    using namespace pika::program_options;

    options_description desc_commandline;
    // clang-format off
    desc_commandline.add_options()
        ("results,r", "print generated results (default: false)")
        ("nx", value<std::uint64_t>()->default_value(10),
         "Local x dimension (of each partition)")
        ("nt", value<std::uint64_t>()->default_value(45),
         "Number of time steps")
        ("np", value<std::uint64_t>()->default_value(10),
         "Number of partitions")
        ("k", value<double>(&k)->default_value(0.5),
         "Heat transfer coefficient (default: 0.5)")
        ("dt", value<double>(&dt)->default_value(1.0),
         "Timestep unit (default: 1.0[s])")
        ("dx", value<double>(&dx)->default_value(1.0),
         "Local x dimension")
        ( "no-header", "do not print out the csv header row")
    ;
    // clang-format on

    // Initialize and run pika
    pika::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return pika::init(pika_main, argc, argv, init_args);
}
