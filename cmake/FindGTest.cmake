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

set(GTest_FOUND false)

find_package(Threads REQUIRED)

include(ExternalProject)

# 1. set google/GTest source code branch
option(GTEST_BRANCH "select branch for GTest" master)
# option only be initialized to ON or OFF, otherwise could be set by cmake -DGTEST_BRANCH=X
if(NOT GTEST_BRANCH)
    set(GTEST_BRANCH master)
endif()

# 2. set source code repository
set(GTEST_DEFAULT_REPOSITORY git@github.com:google/googletest.git)
option(GTEST_REPOSITORY "set repository for source code" ${GTEST_DEFAULT_REPOSITORY})
if(NOT GTEST_REPOSITORY)
    set(GTEST_REPOSITORY ${GTEST_DEFAULT_REPOSITORY})
endif()
message(STATUS "try to download code from ${GTEST_REPOSITORY}")

message(STATUS "try to build ${CMAKE_BUILD_TYPE} version ${GTEST_REPOSITORY}:${GTEST_BRANCH}")

option(BUILD_LOCAL "set the source code to build" ON)
set(GTEST_UPDATE_DISCONNECTED 0)
if(BUILD_LOCAL)
  set(GTEST_UPDATE_DISCONNECTED 1)
endif()
message(STATUS "try to build BUILD_LOCAL ${GTEST_UPDATE_DISCONNECTED}")

set(GTEST_BUILD_PREFIX ${CMAKE_BINARY_DIR}/third_party/)
ExternalProject_Add(googletest
    PREFIX ${GTEST_BUILD_PREFIX}
    BUILD_IN_SOURCE 0
    GIT_REPOSITORY ${GTEST_REPOSITORY}
    GIT_TAG ${GTEST_BRANCH}
    UPDATE_DISCONNECTED ${GTEST_UPDATE_DISCONNECTED}
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
               "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
               "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
    INSTALL_COMMAND ""
)


# export the libs and includes for googletest
ExternalProject_Get_Property(googletest SOURCE_DIR)
ExternalProject_Get_Property(googletest BINARY_DIR)

set(GOOGLETEST_PREFIX "${SOURCE_DIR}")
# 4. set the imported property for gtest which included by GTest indirectly!
set(GTEST_SRC "${GOOGLETEST_PREFIX}/googletest")
set(GTEST_INCLUDE_DIR "${GTEST_SRC}/include")
set(GTEST_LIB_DIR "${BINARY_DIR}/lib")
set(GTEST_LIB "${GTEST_LIB_DIR}/libgtest.a")
set(GTEST_MAIN "${GTEST_LIB_DIR}/libgtest_main.a")

# FIXME(): IMPORTED target can not set interface_directories with non-existing path
# we use INTERFACE target to workaroud
# target_include_directories(T INTERFACE DIR) DIR must existed 
# target_include_directories(gtest INTERFACE $<BUILD_INTERFACE:${GTEST_INCLUDE_DIR}>)
add_library(gtest_ STATIC IMPORTED GLOBAL)
set_property(TARGET gtest_ PROPERTY IMPORTED_LOCATION ${GTEST_LIB})
add_dependencies(gtest_ googletest)
add_library(gtest INTERFACE)
target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gtest INTERFACE gtest_ ${CMAKE_THREAD_LIBS_INIT})

add_library(gtest_main_ STATIC IMPORTED GLOBAL)
set_property(TARGET gtest_main_ PROPERTY IMPORTED_LOCATION ${GTEST_MAIN})
target_link_libraries(gtest_main_ INTERFACE gtest)
add_library(gtest_main INTERFACE)
target_include_directories(gtest_main INTERFACE ${GTEST_INCLUDE_DIR})
target_link_libraries(gtest_main INTERFACE gtest_main_)

# 5. set the imported property for gmock which included by GTest indirectly!
set(GMOCK_SRC "${GOOGLETEST_PREFIX}/googlemock")
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

set(GTest_FOUND true)
