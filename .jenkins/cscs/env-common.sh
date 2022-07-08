# Copyright (c) 2021 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

module load daint-gpu spack-config

export SPACK_USER_CONFIG_PATH="${SPACK_ROOT}/../spack-user-config"
export SPACK_USER_CACHE_PATH="/scratch/snx3000/simbergm/spack-user-cache-jenkins"
source "${SPACK_ROOT}/share/spack/setup-env.sh"

spack load ccache@4.5.1 %gcc@10.3.0

export CMAKE_CXX_COMPILER_LAUNCHER=ccache
export CMAKE_GENERATOR=Ninja
if [[ $USER == "jenkssl" ]]; then
    export CCACHE_DIR=/scratch/snx3000/simbergm/ccache-jenkins-pika
fi
export CCACHE_MAXSIZE=100G
export CCACHE_MAXFILES=50000
export CCACHE_COMPILERCHECK="%compiler% -v"

configure_extra_options+=" -DCMAKE_BUILD_TYPE=${build_type}"
configure_extra_options+=" -DPIKA_WITH_COMPILER_WARNINGS=ON"
configure_extra_options+=" -DPIKA_WITH_COMPILER_WARNINGS_AS_ERRORS=ON"
configure_extra_options+=" -DPIKA_WITH_CHECK_MODULE_DEPENDENCIES=ON"
configure_extra_options+=" -DPIKA_WITH_EXAMPLES=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_UNIT=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_REGRESSIONS=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_BENCHMARKS=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_EXTERNAL_BUILD=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_EXAMPLES=ON"
configure_extra_options+=" -DPIKA_WITH_TESTS_HEADERS=ON"
configure_extra_options+=" -DPIKA_WITH_COMPILER_WARNINGS=ON"
configure_extra_options+=" -DPIKA_WITH_COMPILER_WARNINGS_AS_ERRORS=ON"
