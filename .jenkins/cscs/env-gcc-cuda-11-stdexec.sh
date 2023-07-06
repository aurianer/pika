# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cxx_std="20"
cuda_std="17"
gcc_version="9.3.0"
boost_version="1.75.0"
hwloc_version="2.0.3"
cuda_version="11.2.0"
fmt_version="9.1.0"
spack_compiler="gcc@${gcc_version}"
spack_arch="cray-cnl7-haswell"
stdexec_version="e81dd21338da3b497d0b6c206502b1d3dda394b1"

spack_spec="pika@main arch=${spack_arch} %${spack_compiler} +stdexec +cuda malloc=system cxxstd=${cxx_std} ^boost@${boost_version} ^hwloc@${hwloc_version} ^cuda@${cuda_version} ^fmt@${fmt_version} ^stdexec@${stdexec_version}"

configure_extra_options+=" -DPIKA_WITH_CXX_STANDARD=${cxx_std}"
configure_extra_options+=" -DPIKA_WITH_CUDA_STANDARD=${cuda_std}"
configure_extra_options+=" -DPIKA_WITH_MALLOC=system"
configure_extra_options+=" -DPIKA_WITH_STDEXEC=ON"
configure_extra_options+=" -DPIKA_WITH_CUDA=ON"
configure_extra_options+=" -DPIKA_WITH_EXAMPLES_OPENMP=ON"
