//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/config.hpp>
#include <pika/async_cuda/cuda_device_scope.hpp>
#include <pika/async_cuda/cuda_stream.hpp>
#include <pika/coroutines/thread_enums.hpp>
#include <pika/threading_base/thread_num_tss.hpp>

#include <whip.hpp>

#include <ostream>
#include <sstream>

namespace pika::cuda::experimental {
    cuda_stream::priorities cuda_stream::get_available_priorities()
    {
        cuda_stream::priorities p;

        whip::device_get_stream_priority_range(&p.least, &p.greatest);

        return p;
    }

    whip::stream_t cuda_stream::create_stream(int device,
        pika::execution::thread_priority priority, unsigned int flags)
    {
        std::ostringstream s;
        s << pika::get_worker_thread_num();
        s << "\t create_stream begin\n";
        std::cerr << s.str();
        cuda_device_scope d{device};
        auto p = get_available_priorities();

        whip::stream_t stream;
        if (priority <= pika::execution::thread_priority::normal)
        {
            whip::stream_create_with_priority(&stream, flags, p.least);
        }
        else
        {
            whip::stream_create_with_priority(&stream, flags, p.greatest);
        }
        s << pika::get_worker_thread_num();
        s << "\t create_stream end\n";
        std::cerr << s.str();

        return stream;
    }

    cuda_stream::cuda_stream(int device,
        pika::execution::thread_priority priority, unsigned int flags)
      : device(device)
      , priority(priority)
      , flags(flags)
      , stream(create_stream(device, priority, flags))
    {
    }

    cuda_stream::cuda_stream(cuda_stream&& other) noexcept
      : device(other.device)
      , priority(other.priority)
      , flags(other.flags)
      , stream(other.stream)
    {
        other.device = 0;
        other.priority = pika::execution::thread_priority::default_;
        other.flags = 0;
        other.stream = 0;
    }

    cuda_stream& cuda_stream::operator=(cuda_stream&& other) noexcept
    {
        device = other.device;
        priority = other.priority;
        flags = other.flags;
        stream = other.stream;

        other.device = 0;
        other.priority = pika::execution::thread_priority::default_;
        other.flags = 0;
        other.stream = 0;

        return *this;
    }

    cuda_stream::cuda_stream(cuda_stream const& other)
      : device(other.device)
      , priority(other.priority)
      , flags(other.flags)
      , stream(other.stream != 0 ? create_stream(device, priority, flags) : 0)
    {
    }

    cuda_stream& cuda_stream::operator=(cuda_stream const& other)
    {
        device = other.device;
        priority = other.priority;
        flags = other.flags;
        stream = other.stream != 0 ? create_stream(device, priority, flags) : 0;

        return *this;
    }

    cuda_stream::~cuda_stream()
    {
        if (stream != 0)
        {
            whip::stream_destroy(stream);
        }
    }

    int cuda_stream::get_device() const noexcept
    {
        return device;
    }

    pika::execution::thread_priority cuda_stream::get_priority() const noexcept
    {
        return priority;
    }

    unsigned int cuda_stream::get_flags() const noexcept
    {
        return flags;
    }

    whip::stream_t cuda_stream::get() const noexcept
    {
        return stream;
    }

    std::ostream& operator<<(std::ostream& os, cuda_stream const& stream)
    {
        os << "cuda_stream(" << stream.get() << ")";
        return os;
    }
}    // namespace pika::cuda::experimental
