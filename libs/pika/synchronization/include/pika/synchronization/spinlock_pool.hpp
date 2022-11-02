//  Copyright (c) 2012-2021 Hartmut Kaiser
//  Copyright (c) 2014 Thomas Heller
//
//  adapted from:
//  boost/detail/spinlock_pool.hpp
//
//  Copyright (c) 2008 Peter Dimov
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>
#include <pika/concurrency/cache_line_data.hpp>
#include <pika/hashing/fibhash.hpp>
#include <pika/synchronization/spinlock.hpp>

#include <cstddef>

///////////////////////////////////////////////////////////////////////////////
namespace pika::detail {

#if PIKA_HAVE_ITTNOTIFY != 0
    template <typename Tag, std::size_t N>
    struct itt_spinlock_init
    {
        itt_spinlock_init() noexcept;
        ~itt_spinlock_init();
    };
#endif

    template <typename Tag, std::size_t N = PIKA_HAVE_SPINLOCK_POOL_NUM>
    class spinlock_pool
    {
    private:
        static pika::concurrency::detail::cache_aligned_data<pika::spinlock>
            pool_[N];
#if PIKA_HAVE_ITTNOTIFY != 0
        static detail::itt_spinlock_init<Tag, N> init_;
#endif
    public:
        static pika::spinlock& spinlock_for(void const* pv) noexcept
        {
            std::size_t i = util::fibhash<N>(reinterpret_cast<std::size_t>(pv));
            return pool_[i].data_;
        }

        class scoped_lock
        {
        private:
            pika::spinlock& sp_;

        public:
            PIKA_NON_COPYABLE(scoped_lock);

        public:
            explicit scoped_lock(void const* pv)
              : sp_(spinlock_for(pv))
            {
                lock();
            }

            ~scoped_lock()
            {
                unlock();
            }

            void lock()
            {
                PIKA_ITT_SYNC_PREPARE(&sp_);
                sp_.lock();
                PIKA_ITT_SYNC_ACQUIRED(&sp_);
            }

            void unlock()
            {
                PIKA_ITT_SYNC_RELEASING(&sp_);
                sp_.unlock();
                PIKA_ITT_SYNC_RELEASED(&sp_);
            }
        };
    };

    template <typename Tag, std::size_t N>
    pika::concurrency::detail::cache_aligned_data<pika::spinlock>
        spinlock_pool<Tag, N>::pool_[N];

#if PIKA_HAVE_ITTNOTIFY != 0

    template <typename Tag, std::size_t N>
    itt_spinlock_init<Tag, N>::itt_spinlock_init() noexcept
    {
        for (int i = 0; i < N; ++i)
        {
            PIKA_ITT_SYNC_CREATE(
                (&pika::detail::spinlock_pool<Tag, N>::pool_[i].data_),
                "pika::lcos::spinlock", 0);
            PIKA_ITT_SYNC_RENAME(
                (&pika::detail::spinlock_pool<Tag, N>::pool_[i].data_),
                "pika::lcos::spinlock");
        }
    }

    template <typename Tag, std::size_t N>
    itt_spinlock_init<Tag, N>::~itt_spinlock_init()
    {
        for (int i = 0; i < N; ++i)
        {
            PIKA_ITT_SYNC_DESTROY((&spinlock_pool<Tag, N>::pool_[i].data_));
        }
    }

    template <typename Tag, std::size_t N>
    itt_spinlock_init<Tag, N> spinlock_pool<Tag, N>::init_{};
#endif
}    // namespace pika::detail
