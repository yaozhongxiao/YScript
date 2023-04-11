#
# Copyright 2023 Develop Group Participants. All right reserver.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.0)

project(YSCRIPT)

# set the options so as to check the property values
set(CMAKE_DEBUG_TARGET_PROPERTIES
    # INCLUDE_DIRECTORIES
    # CONTAINER_SIZE_REQUIRED
    # LIB_VERSION
    # COMPILE_DEFINITIONS
    # COMPILE_OPTIONS
    # AUTOUIC_OPTIONS
    # SOURCES
    # COMPILE_FEATURES
    # POSITION_INDEPENDENT_CODE
)

set(YSCRIPT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YSCRIPT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YSCRIPT_SOURCE_DIR}/cmake)

find_package(BuildConfig)

if(NOT BuildConfig_FOUND)
    message(FATAL_ERROR "can not find module BuildConfig!")
endif()

find_package(BuildFunction)

if(NOT BuildFunction_FOUND)
    message(FATAL_ERROR "can not find module BuildFunction!")
endif()

# find_package(Log4cplus)
# if(NOT Log4cplus_FOUND)
# message(FATAL_ERROR "can not find log4cplus!")
# endif()

# include CMAKE_CURRENT_SOURCE_DIR & CMAKE_CURRENT_BINARY_DIR
# include_directories for each module
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set(X_LIBIRARIES_DIR ${YSCRIPT_SOURCE_DIR}/libraries)
# set(X_BASE_DIR ${X_LIBIRARIES_DIR}/base)
# set(X_ENGINE_DIR ${YSCRIPT_SOURCE_DIR}/src/engine)

# include the common search directories
# include_directories(${YSCRIPT_BINARY_DIR}/gen)
include_directories(${YSCRIPT_SOURCE_DIR}/src)

# include_directories(${X_BASE_DIR}/include)
# add custom target working as the top node of module tree
add_custom_target(target_all)

# build the libbase
# add_subdirectory(libraries)

# build the engine libengine
add_subdirectory(src)

# build cli
add_subdirectory(tools/cli)

# build tools
# add_subdirectory(tools)
if(ENABLE_TEST)
    # build the unittest
    add_subdirectory(test)
endif()
