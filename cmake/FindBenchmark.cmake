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

set(Benchmark_FOUND false)

find_package(Threads REQUIRED)

include(ExternalProject)

# 1. set google/benchmark source code branch
option(BENCHMARK_BRANCH "select branch for benchmark" master)
# option only be initialized to ON or OFF, otherwise could be set by cmake -DBENCHMARK_BRANCH=X
if(NOT BENCHMARK_BRANCH)
    set(BENCHMARK_BRANCH master)
endif()

# 2. set source code repository
set(BENCHMARK_DEFAULT_REPOSITORY git@github.com:google/benchmark.git)
option(BENCHMARK_REPOSITORY "set repository for source code" ${BENCHMARK_DEFAULT_REPOSITORY})
if(NOT BENCHMARK_REPOSITORY)
    set(BENCHMARK_REPOSITORY ${BENCHMARK_DEFAULT_REPOSITORY})
endif()
message(STATUS "try to download code from ${BENCHMARK_REPOSITORY}")

# 3. set target build type
option(BENCHMARK_BUILD_TYPE "set the build mode" Debug)
if(NOT BENCHMARK_BUILD_TYPE)
    set(BENCHMARK_BUILD_TYPE Release)
endif()
if(BENCHMARK_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DBENCHMARK_DEBUG)
endif()
message(STATUS "try to build ${BENCHMARK_BUILD_TYPE} version ${BENCHMARK_REPOSITORY}:${BENCHMARK_BRANCH}")

option(BUILD_LOCAL "set the source code to build" ON)
set(BENCHMARK_UPDATE_DISCONNECTED 0)
if(BUILD_LOCAL)
  set(BENCHMARK_UPDATE_DISCONNECTED 1)
endif()
message(STATUS "try to build BUILD_LOCAL ${BENCHMARK_UPDATE_DISCONNECTED}")

set(BENCHMARK_BUILD_PREFIX ${CMAKE_BINARY_DIR}/third_party/)
ExternalProject_Add(google_benchmark
    PREFIX ${BENCHMARK_BUILD_PREFIX}
    BUILD_IN_SOURCE 0
    GIT_REPOSITORY ${BENCHMARK_REPOSITORY}
    GIT_TAG ${BENCHMARK_BRANCH}
    UPDATE_DISCONNECTED ${BENCHMARK_UPDATE_DISCONNECTED}
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
               "-DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON"
               "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
               "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
    INSTALL_COMMAND ""
)

# FIXME(): IMPORTED target can not set interface_directories with non-existing path
# we use INTERFACE target to workaroud
# target_include_directories(T INTERFACE DIR) DIR must existed 
# target_include_directories(gtest INTERFACE ${IMPORTED_INCLUDE_DIR}>)

# 4. export the libs and includes
ExternalProject_Get_Property(google_benchmark SOURCE_DIR)
ExternalProject_Get_Property(google_benchmark BINARY_DIR)
set(GOOGLETEST_PREFIX "${BINARY_DIR}/third_party/googletest")

# 5. set the imported property for gtest which included by benchmark indirectly!
set(GTEST_SRC "${GOOGLETEST_PREFIX}/src/googletest")
set(GTEST_INCLUDE_DIR "${GTEST_SRC}/include")
set(GTEST_LIB_DIR "${BINARY_DIR}/lib")
set(GTEST_LIB "${GTEST_LIB_DIR}/libgtest.a")
set(GTEST_MAIN "${GTEST_LIB_DIR}/libgtest_main.a")

add_library(gtest_ STATIC IMPORTED GLOBAL)
set_property(TARGET gtest_ PROPERTY IMPORTED_LOCATION ${GTEST_LIB})
add_dependencies(gtest_ google_benchmark)
add_library(gtest INTERFACE)
target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gtest INTERFACE gtest_ ${CMAKE_THREAD_LIBS_INIT})

add_library(gtest_main_ STATIC IMPORTED GLOBAL)
set_property(TARGET gtest_main_ PROPERTY IMPORTED_LOCATION ${GTEST_MAIN})
target_link_libraries(gtest_main_ INTERFACE gtest)
add_library(gtest_main INTERFACE)
target_include_directories(gtest_main INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gtest_main INTERFACE gtest_main_)

# 6. set the imported property for gmock which included by benchmark indirectly!
set(GMOCK_SRC "${GOOGLETEST_PREFIX}/src/googlemock")
set(GMOCK_INCLUDE_DIR "${GMOCK_SRC}/include")
set(GMOCK_LIB_DIR "${BINARY_DIR}/lib")
set(GMOCK_LIB "${GMOCK_LIB_DIR}/libgmock.a")
set(GMOCK_MAIN "${GMOCK_LIB_DIR}/libgmock_main.a")

add_library(gmock_ STATIC IMPORTED GLOBAL)
set_property(TARGET gmock_ PROPERTY IMPORTED_LOCATION ${GMOCK_LIB})
target_link_libraries(gmock_ INTERFACE gtest)
add_library(gmock INTERFACE)
target_include_directories(gmock INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gmock INTERFACE gmock_)

add_library(gmock_main_ STATIC IMPORTED GLOBAL)
set_property(TARGET gmock_main_ PROPERTY IMPORTED_LOCATION ${GMOCK_MAIN})
target_link_libraries(gmock_main_ INTERFACE gmock)
add_library(gmock_main INTERFACE)
target_include_directories(gmock_main INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gmock_main INTERFACE gmock_main_)

# 7. set the imported property for benchmark!
set(BENCHMARK_INCLUDE_DIR "${SOURCE_DIR}/include/")
set(BENCHMARK_LIB_DIR "${BINARY_DIR}/src")
set(BENCHMARK_LIB "${BENCHMARK_LIB_DIR}/libbenchmark.a")
set(BENCHMARK_MAIN "${BENCHMARK_LIB_DIR}/libbenchmark_main.a")

add_library(benchmark_ STATIC IMPORTED GLOBAL)
set_property(TARGET benchmark_ PROPERTY IMPORTED_LOCATION ${BENCHMARK_LIB})
add_dependencies(benchmark_ google_benchmark)
add_library(benchmark INTERFACE)
target_include_directories(benchmark INTERFACE ${BENCHMARK_INCLUDE_DIR})
target_link_libraries(benchmark INTERFACE benchmark_ gmock ${CMAKE_THREAD_LIBS_INIT})

add_library(benchmark_main_ STATIC IMPORTED GLOBAL)
set_property(TARGET benchmark_main_ PROPERTY IMPORTED_LOCATION ${BENCHMARK_MAIN})
target_link_libraries(benchmark_main_ INTERFACE benchmark)
add_library(benchmark_main INTERFACE)
target_include_directories(benchmark_main INTERFACE ${BENCHMARK_INCLUDE_DIR})
target_link_libraries(benchmark_main INTERFACE benchmark_main_)

set(Benchmark_FOUND true)
