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

#include <whip.hpp>

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
    pika::cuda::experimental::cuda_pool cuda_pool(0);

    return pika::finalize();
}

// -------------------------------------------------------------------------
int main(int argc, char** argv)
{
    printf("[pika Cuda future] - Starting...\n");

    pika::init_params init_args;

    return pika::init(pika_main, argc, argv, init_args);
}
