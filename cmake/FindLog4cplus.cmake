# Copyright 2023 Develop Group Participants. All rights reserved.
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

set(Log4cplus_FOUND true)

include(ExternalProject)

set(LOG4CPLUS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libraries/third_party/log4cplus)
# must surround with "", otherwise configure errors
set(UPDATE_ARGS "")
option(OPTION_FORCE "Build with force options" OFF)
if(OPTION_FORCE)
    set(UPDATE_ARGS autoreconf --force --install ${LOG4CPLUS_SOURCE_DIR})
endif()
# set the build flags
set(LOG4CPLUS_CXXFLAGS "CXXFLAGS=-std=c++11 -I${LOG4CPLUS_SOURCE_DIR}/include")
set(CONFIGURE_ARGS ${LOG4CPLUS_CXXFLAGS} --disable-shared --enable-static --disable-maintainer-mode)
set(LOG4CPLUS_CONFIGURE_COMMAND ${LOG4CPLUS_SOURCE_DIR}/configure ${CONFIGURE_ARGS})
set(LOG4CPLUS_BUILD_TOOL make)
add_compile_options(-Wno-deprecated-declarations)

ExternalProject_Add(log4cplus
    PREFIX ${CMAKE_SOURCE_DIR}/libraries/third_party/build
    SOURCE_DIR ${LOG4CPLUS_SOURCE_DIR}
    BUILD_IN_SOURCE 0
    UPDATE_COMMAND ${UPDATE_ARGS}
    LOG_UPDATE 1
    CONFIGURE_COMMAND ${LOG4CPLUS_CONFIGURE_COMMAND}
    LOG_CONFIGURE 1
    BUILD_COMMAND ${LOG4CPLUS_BUILD_TOOL}
    LOG_BUILD 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(log4cplus SOURCE_DIR)
ExternalProject_Get_Property(log4cplus BINARY_DIR)
set(LOG4CPLUS_INCLUDE_DIR ${SOURCE_DIR}/include ${BINARY_DIR}/include)
set(LOG4CPLUS_LIB_DIR ${BINARY_DIR}/.libs)
set(LOG4CPLUS_LIBRARY ${LOG4CPLUS_LIB_DIR}/liblog4cplus.a)

# message(FATAL_ERROR ${LOG4CPLUS_INCLUDE_DIR})
add_library(log4cplus_static STATIC IMPORTED)
set_property(TARGET log4cplus_static PROPERTY IMPORTED_LOCATION ${LOG4CPLUS_LIBRARY})
add_dependencies(log4cplus_static log4cplus)
