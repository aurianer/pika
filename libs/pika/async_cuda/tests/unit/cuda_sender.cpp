//  Copyright (c) 2018 John Biddiscombe
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/algorithm.hpp>
#include <pika/cuda.hpp>
#include <pika/execution.hpp>
#include <pika/functional.hpp>
#include <pika/init.hpp>
#include <pika/testing.hpp>
#include <pika/threading_base/thread_num_tss.hpp>

#include <whip.hpp>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace cu = pika::cuda::experimental;
namespace ex = pika::execution::experimental;
namespace tt = pika::this_thread::experimental;

// -------------------------------------------------------------------------
int pika_main(pika::program_options::variables_map& vm)
{
    // install cuda future polling handler
    pika::cuda::experimental::enable_user_polling poll("default");

    std::ostringstream s;
    s << pika::get_worker_thread_num();
    s << "\t in between poll and cuda_pool instances\n";
    std::cerr << s.str();

    pika::cuda::experimental::cuda_pool cuda_pool(0);

    s << pika::get_worker_thread_num();
    s << "\t after cuda pool\n";
    std::cerr << s.str();

    return pika::finalize();
}

// -------------------------------------------------------------------------
int main(int argc, char** argv)
{
    printf("[pika Cuda future] - Starting...\n");

    pika::init_params init_args;

    return pika::init(pika_main, argc, argv, init_args);
}
