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
namespace tt = pika::this_thread::experimental;

template <class T>
class SweepWorker {
public:
  SweepWorker(std::ptrdiff_t size, std::ptrdiff_t band_size)
      : size_(size), band_size_(band_size) {}

  SweepWorker(const SweepWorker&) = delete;
  SweepWorker(SweepWorker&&) = default;

  SweepWorker& operator=(const SweepWorker&) = delete;
  SweepWorker& operator=(SweepWorker&&) = default;

protected:
  std::ptrdiff_t size_;
  std::ptrdiff_t band_size_;
  std::ptrdiff_t sweep_ = 0;
  std::ptrdiff_t step_ = 0;
};

int main()
{
    ex::unique_any_sender<SweepWorker<float>> send_ = ex::just(std::move(SweepWorker<float>(0, std::ptrdiff_t(1))));

    return 0;
}
