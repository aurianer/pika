//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/execution.hpp>
#include <pika/execution_base/stdexec_forward.hpp>
#include <pika/execution_base/any_sender.hpp>
#include <pika/execution_base/sender.hpp>
#include <pika/functional/bind_front.hpp>
#include <pika/functional/invoke_fused.hpp>
#include <pika/modules/errors.hpp>
#include <pika/testing.hpp>

#include <atomic>
#include <cstddef>
#include <exception>
#include <string>
#include <tuple>
#include <utility>

namespace ex = pika::execution::experimental;

class SweepWorker {
public:
  SweepWorker() {}
  SweepWorker(SweepWorker&&) noexcept {};
};

int main()
{
    ex::unique_any_sender<SweepWorker> send_ = ex::just(std::move(SweepWorker()));
    return 0;
}
