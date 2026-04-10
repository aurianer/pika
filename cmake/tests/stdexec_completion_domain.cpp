//  Copyright (c) 2026 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <stdexec/execution.hpp>

// Check if stdexec supports get_completion_domain_t for domain-based
// algorithm customization (e.g. bulk).
int main()
{
    using domain_query_t = stdexec::get_completion_domain_t<stdexec::set_value_t>;
    (void) sizeof(domain_query_t);
}
