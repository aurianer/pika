#!/usr/bin/env python3
"""
Copyright (c) 2019-2020 ETH Zurich
Copyright (c) 2018      Thomas Heller

SPDX-License-Identifier: BSL-1.0
Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

create_module_skeleton.py - A tool to generate a module skeleton to be used
as a component of pika
"""

import sys, os

if len(sys.argv) != 3:
    print("Usage: %s <lib_name> <module_name>" % sys.argv[0])
    print(
        "Generates the skeleton for module_name in the <lib_name> directory under the current working directory"
    )
    sys.exit(1)

lib_name = sys.argv[1]
lib_name_upper = lib_name.upper()
module_name = sys.argv[2]
header_str = "=" * len(module_name)


cmake_root_header = f"""# Copyright (c) 2021-2022 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
"""

cmake_header = f"""# Copyright (c) 2021-2022 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
"""

readme_template = f"""
..
    Copyright (c) 2021-2022 ETH Zurich

    SPDX-License-Identifier: BSL-1.0
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

{header_str}
{module_name}
{header_str}

This module is part of pika.
"""

index_rst = f"""..
    Copyright (c) 2021-2022 ETH Zurich

    SPDX-License-Identifier: BSL-1.0
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

.. _modules_{module_name}:

{header_str}
{module_name}
{header_str}

TODO: High-level description of the module.

See the :ref:`API reference <modules_{module_name}_api>` of this module for more
details.

"""

root_cmakelists_template = (
    cmake_root_header
    + f"""
list(APPEND CMAKE_MODULE_PATH "${{CMAKE_CURRENT_SOURCE_DIR}}/cmake")

set({module_name}_headers)

# cmake-format: off
set({module_name}_compat_headers)
# cmake-format: on

set({module_name}_sources)

include(pika_add_module)
pika_add_module(
  {lib_name} {module_name}
  GLOBAL_HEADER_GEN ON
  SOURCES ${{{module_name}_sources}}
  HEADERS ${{{module_name}_headers}}
  DEPENDENCIES
  CMAKE_SUBDIRS examples tests
)
"""
)

examples_cmakelists_template = (
    cmake_header
    + f"""
if(PIKA_WITH_EXAMPLES)
  pika_add_pseudo_target(examples.modules.{module_name})
  pika_add_pseudo_dependencies(examples.modules examples.modules.{module_name})
  if(PIKA_WITH_TESTS AND PIKA_WITH_TESTS_EXAMPLES)
    pika_add_pseudo_target(tests.examples.modules.{module_name})
    pika_add_pseudo_dependencies(
      tests.examples.modules tests.examples.modules.{module_name}
    )
  endif()
endif()
"""
)

tests_cmakelists_template = (
    cmake_header
    + f"""
include(pika_message)

if(PIKA_WITH_TESTS)
  if(PIKA_WITH_TESTS_UNIT)
    pika_add_pseudo_target(tests.unit.modules.{module_name})
    pika_add_pseudo_dependencies(
      tests.unit.modules tests.unit.modules.{module_name}
    )
    add_subdirectory(unit)
  endif()

  if(PIKA_WITH_TESTS_REGRESSIONS)
    pika_add_pseudo_target(tests.regressions.modules.{module_name})
    pika_add_pseudo_dependencies(
      tests.regressions.modules tests.regressions.modules.{module_name}
    )
    add_subdirectory(regressions)
  endif()

  if(PIKA_WITH_TESTS_BENCHMARKS)
    pika_add_pseudo_target(tests.performance.modules.{module_name})
    pika_add_pseudo_dependencies(
      tests.performance.modules tests.performance.modules.{module_name}
    )
    add_subdirectory(performance)
  endif()

  if(PIKA_WITH_TESTS_HEADERS)
    pika_add_header_tests(
      modules.{module_name}
      HEADERS ${{{module_name}_headers}}
      HEADER_ROOT ${{PROJECT_SOURCE_DIR}}/include
      DEPENDENCIES pika_{module_name}
    )
  endif()
endif()
"""
)

if module_name != "--recreate-index":

    def mkdir(path):
        if not os.path.exists(path):
            os.makedirs(path)

    mkdir(os.path.realpath(os.path.join(lib_name, module_name)))

    ################################################################################
    # Generate basic directory structure
    for subdir in ["docs", "examples", "include", "src", "tests"]:
        path = os.path.realpath(os.path.join(lib_name, module_name, subdir))
        mkdir(path)
    # Generate include directory structure
    # Normalize path...
    include_path = "".join(module_name)
    path = os.path.realpath(os.path.join(lib_name, module_name, "include", "pika", include_path))
    mkdir(path)
    path = os.path.realpath(os.path.join(lib_name, module_name, "tests", "unit"))
    mkdir(path)
    path = os.path.realpath(os.path.join(lib_name, module_name, "tests", "regressions"))
    mkdir(path)
    path = os.path.realpath(os.path.join(lib_name, module_name, "tests", "performance"))
    mkdir(path)
    ################################################################################

    ################################################################################
    # Generate README skeleton
    f = open(os.path.realpath(os.path.join(lib_name, module_name, "README.rst")), "w")
    f.write(readme_template)
    ################################################################################

    ################################################################################
    # Generate CMakeLists.txt skeletons

    # Generate top level CMakeLists.txt
    f = open(os.path.realpath(os.path.join(lib_name, module_name, "CMakeLists.txt")), "w")
    f.write(root_cmakelists_template)

    # Generate docs/index.rst
    f = open(os.path.realpath(os.path.join(lib_name, module_name, "docs", "index.rst")), "w")
    f.write(index_rst)

    # Generate examples/CMakeLists.txt
    f = open(os.path.realpath(os.path.join(lib_name, module_name, "examples", "CMakeLists.txt")), "w")
    f.write(examples_cmakelists_template)

    # Generate tests/CMakeLists.txt
    f = open(os.path.realpath(os.path.join(lib_name, module_name, "tests", "CMakeLists.txt")), "w")
    f.write(tests_cmakelists_template)

    # Generate tests/unit/CMakeLists.txt
    f = open(
        os.path.realpath(os.path.join(lib_name, module_name, "tests", "unit", "CMakeLists.txt")), "w"
    )
    f.write(cmake_header)

    # Generate tests/regressions/CMakeLists.txt
    f = open(
        os.path.realpath(os.path.join(lib_name, module_name, "tests", "regressions",
            "CMakeLists.txt")),
        "w",
    )
    f.write(cmake_header)

    # Generate tests/performance/CMakeLists.txt
    f = open(
        os.path.realpath(os.path.join(lib_name, module_name, "tests", "performance",
            "CMakeLists.txt")),
        "w",
    )
    f.write(cmake_header)
    ################################################################################

################################################################################

# Scan directory to get all modules...
cwd = os.getcwd()
modules = sorted(
    [
        module
        for module in os.listdir(os.path.realpath(os.path.join(cwd, lib_name)))
        if os.path.isdir(os.path.join(cwd, lib_name, module))
    ]
)


# Adapting top level CMakeLists.txt
modules_cmakelists = (
    cmake_header
    + f"""
# Do not edit this file! It has been generated by the
# libs/create_module_skeleton.py script.
"""
)

modules_cmakelists += f"""
include(pika_message)

# cmake-format: off
set(_pika_{lib_name}_modules
"""
for module in modules:
    if not module.startswith("_"):
        modules_cmakelists += f"    {module}\n"
modules_cmakelists += ")\n# cmake-format: on\n"

modules_cmakelists += f"""
pika_info("")
pika_info("  Configuring libpika{"_" + lib_name if lib_name != "full" else ""} modules:")

foreach(module ${{_pika_{lib_name}_modules}})
  add_subdirectory(${{module}})
endforeach()
"""

f = open(os.path.realpath(os.path.join(cwd, lib_name, "CMakeLists.txt")), "w")
f.write(modules_cmakelists)

header_name_str = (
    "Main |pika| modules" if lib_name == "full" else lib_name.capitalize() + " modules"
)
header_underline_str = "=" * len(header_name_str)

modules_rst = f"""..
    Copyright (c) 2021-2022 ETH Zurich

    SPDX-License-Identifier: BSL-1.0
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

.. _{lib_name}_modules:

{header_underline_str}
{header_name_str}
{header_underline_str}

.. toctree::
   :maxdepth: 2

"""
for module in modules:
    modules_rst += f"   /libs/{lib_name}/{module}/docs/index.rst\n"

f = open(os.path.realpath(os.path.join(cwd, lib_name, "modules.rst")), "w")
f.write(modules_rst)

################################################################################
