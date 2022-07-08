#!/bin/bash

export SPACK_ROOT=/apps/daint/SSL/pika/spack CMAKE_GENERATOR=Ninja build_type=Debug && source ~/projects/pika/.jenkins/cscs/env-common.sh && unset SPACK_USER_CACHE_PATH && source ~/projects/pika/.jenkins/cscs/env-gcc-cuda.sh
