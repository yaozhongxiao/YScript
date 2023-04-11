#
# Copyright 2023 Develop Group Participants. All right reserver.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.0)
project(YScriptTester)

# set the options so as to check the property values
set(CMAKE_DEBUG_TARGET_PROPERTIES
#  INCLUDE_DIRECTORIES
#  CONTAINER_SIZE_REQUIRED
#  LIB_VERSION
#  COMPILE_DEFINITIONS
#  COMPILE_OPTIONS
#  AUTOUIC_OPTIONS
#  SOURCES
#  COMPILE_FEATURES
#  POSITION_INDEPENDENT_CODE
)

set(CI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CI_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CI_SOURCE_DIR}/../cmake)

find_package(TestUtil)
if (NOT TestUtil_FOUND)
    message(FATAL_ERROR "can not find module TestUtil!")
endif()

find_package(Benchmark)
if (NOT Benchmark_FOUND)
    message(FATAL_ERROR "can not find module Benchmark!")
endif()

# find_package(GTest)
# if (NOT GTest_FOUND)
#     message(FATAL_ERROR "can not find module googletest!")
# endif()

# include CMAKE_CURRENT_SOURCE_DIR & CMAKE_CURRENT_BINARY_DIR
# include_directories for each module
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_custom_target(target_all)
# add_dependencies(target_all gtest_main)
add_dependencies(target_all gmock_main)

set(CI_STD_SIMD_DIR ${CI_SOURCE_DIR}/..)

# include the common search directories
include_directories(${CI_STD_SIMD_DIR})

add_subdirectory(unittest)

add_subdirectory(benchmark)
