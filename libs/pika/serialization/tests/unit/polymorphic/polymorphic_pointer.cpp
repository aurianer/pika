//  Copyright (c) 2014 Thomas Heller
//  Copyright (c) 2014 Anton Bikineev
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/serialization/base_object.hpp>
#include <pika/serialization/detail/raw_ptr.hpp>
#include <pika/serialization/input_archive.hpp>
#include <pika/serialization/output_archive.hpp>
#include <pika/serialization/serialize.hpp>

#include <pika/testing.hpp>

#include <vector>

struct A
{
    A()
      : a(8)
    {
    }
    virtual ~A() {}

    int a;

    template <typename Archive>
    void serialize(Archive& ar, unsigned)
    {
        ar& a;
    }
    PIKA_SERIALIZATION_POLYMORPHIC(A);
};

struct B
{
    B()
      : b(6)
    {
    }
    explicit B(int i)
      : b(i)
    {
    }

    virtual ~B() {}

    virtual void f() = 0;

    int b;

    template <typename Archive>
    void serialize(Archive& ar, unsigned)
    {
        ar& b;
    }
    PIKA_SERIALIZATION_POLYMORPHIC_ABSTRACT(B);
};

struct D : B
{
    D()
      : d(89)
    {
    }
    explicit D(int i)
      : B(i)
      , d(89)
    {
    }
    void f() {}

    int d;

    template <typename Archive>
    void serialize(Archive& ar, unsigned)
    {
        b = 4711;
        ar& pika::serialization::base_object<B>(*this);
        ar& d;
    }
    PIKA_SERIALIZATION_POLYMORPHIC(D);
};

int main()
{
    std::vector<char> buffer;
    pika::serialization::output_archive oarchive(buffer);
    oarchive << A();

    B* const b1 = new D;
    oarchive << pika::serialization::detail::raw_ptr(b1);
    oarchive << pika::serialization::detail::raw_ptr(b1);

    pika::serialization::input_archive iarchive(buffer);
    A a;
    iarchive >> a;
    B *b2 = nullptr, *b3 = nullptr;
    iarchive >> pika::serialization::detail::raw_ptr(b2);
    iarchive >> pika::serialization::detail::raw_ptr(b3);

    PIKA_TEST_EQ(a.a, 8);
    PIKA_TEST_NEQ(b2, b1);
    PIKA_TEST_NEQ(b2, b3);    //untracked
    PIKA_TEST_EQ(b2->b, b1->b);

    delete b2;
    delete b3;

    PIKA_TEST_EQ(b1->b, 4711);

    delete b1;

    return pika::util::report_errors();
}
