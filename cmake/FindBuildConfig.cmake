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

set(BuildConfig_FOUND FALSE)

message(STATUS "current compiler-id : ${CMAKE_C_COMPILER_ID}")
if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
  set(COMPILER_IS_CLANG 1)
  set(COMPILER_IS_GNU 0)
  set(COMPILER_IS_MSVC 0)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
  set(COMPILER_IS_CLANG 0)
  set(COMPILER_IS_GNU 1)
  set(COMPILER_IS_MSVC 0)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
  set(COMPILER_IS_CLANG 0)
  set(COMPILER_IS_GNU 0)
  set(COMPILER_IS_MSVC 1)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(COMPILER_IS_CLANG 1)
  set(COMPILER_IS_GNU 0)
  set(COMPILER_IS_MSVC 0)
else ()
  set(COMPILER_IS_CLANG 0)
  set(COMPILER_IS_GNU 0)
  set(COMPILER_IS_MSVC 0)
endif ()

option(BUILD_TESTS "Build GTest-based tests" ON)
option(BUILD_TOOLS "Build X commandline tools" ON)
option(RUN_RE2C "Run re2c" ON)
option(USE_ASAN "Use address sanitizer" OFF)
option(USE_MSAN "Use memory sanitizer" OFF)
option(USE_LSAN "Use leak sanitizer" OFF)
option(USE_UBSAN "Use undefined behavior sanitizer" OFF)
option(WITH_EXCEPTIONS "Build with exceptions enabled" OFF)
option(CODE_COVERAGE "Build with code coverage enabled" OFF)

option(FILE_LOGGER "use log4cplus to printf logs to file" OFF)
if(FILE_LOGGER)
  message(STATUS "print logs to file")
  add_definitions(-DFILE_LOGGER)
endif()

option(TARGET_INSTALL "Install the build output" OFF)
set(X_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/engine/bin CACHE PATH "bin")
set(X_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/engine/libs CACHE PATH "libs")
set(X_INSTALL_INC_DIR ${CMAKE_INSTALL_PREFIX}/engine/include CACHE PATH "header")

include(CheckIncludeFile)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

include(CheckSymbolExists)
check_symbol_exists(snprintf "stdio.h" HAVE_SNPRINTF)
check_symbol_exists(sysconf "unistd.h" HAVE_SYSCONF)
check_symbol_exists(strcasecmp "strings.h" HAVE_STRCASECMP)

include(CheckTypeSize)
check_type_size(ssize_t SSIZE_T)
check_type_size(size_t SIZEOF_SIZE_T)

# disable -Wunused-parameter: this is really common when implementing
#   interfaces, etc.
# disable -Wpointer-arith: this is a GCC extension, and doesn't work in MSVC.
add_definitions(
  -Wall -Wextra -Wno-unused-parameter -Wpointer-arith -Wuninitialized
  -Werror -Wno-old-style-cast
)

if (NOT WITH_EXCEPTIONS)
  add_definitions(-fno-exceptions)
endif ()

# Need to define __STDC_*_MACROS because C99 specifies that C++ shouldn't
# define format (e.g. PRIu64) or limit (e.g. UINT32_MAX) macros without the
# definition, and some libcs (e.g. glibc2.17 and earlier) follow that.
add_definitions(-D__STDC_LIMIT_MACROS=1 -D__STDC_FORMAT_MACROS=1)

if (COMPILER_IS_GNU)
  # disable -Wclobbered: it seems to be guessing incorrectly about a local
  # variable being clobbered by longjmp.
  add_definitions(-Wno-clobbered)
endif ()

# set code sanitizer
function(SANITIZER NAME FLAGS)
  if (${NAME})
    if (USE_SANITIZER)
      message(FATAL_ERROR "Only one sanitizer allowed")
    endif ()
    set(USE_SANITIZER TRUE PARENT_SCOPE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}" PARENT_SCOPE)
  endif ()
endfunction()
set(USE_SANITIZER FALSE)
SANITIZER(USE_ASAN "-fsanitize=address")
SANITIZER(USE_MSAN "-fsanitize=memory")
SANITIZER(USE_LSAN "-fsanitize=leak")

if (USE_UBSAN)
  # -fno-sanitize-recover was deprecated, see if we are compiling with a newer
  # clang that requires -fno-sanitize-recover=all.
  set(UBSAN_BLACKLIST ${X_SOURCE_DIR}/ubsan.blacklist)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-fsanitize=undefined -fno-sanitize-recover -Wall -Werror" HAS_UBSAN_RECOVER_BARE)
  if (HAS_UBSAN_RECOVER_BARE)
    SANITIZER(USE_UBSAN "-fsanitize=undefined -fno-sanitize-recover -fsanitize-blacklist=${UBSAN_BLACKLIST}")
  endif ()
  CHECK_CXX_COMPILER_FLAG("-fsanitize=undefined -fno-sanitize-recover=all -Wall -Werror" HAS_UBSAN_RECOVER_ALL)
  if (HAS_UBSAN_RECOVER_ALL)
    SANITIZER(USE_UBSAN "-fsanitize=undefined -fno-sanitize-recover=all -fsanitize-blacklist=${UBSAN_BLACKLIST}")
  endif ()
  if (NOT USE_SANITIZER)
    message(FATAL_ERROR "UBSAN is not supported")
  endif ()
endif ()

# default built type = Release
option(FORCE_OPTIMIZE "enable optimize flags forcely" ON)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message(STATUS "Build in Debug mode")
  add_definitions(-DX_DEBUG -g)
  if (FORCE_OPTIMIZE)
     message(STATUS "force optimize is enabled")
     add_compile_options(-O3)
  endif(FORCE_OPTIMIZE)

  if(CODE_COVERAGE AND COMPILER_IS_GNU)
    message(STATUS "\n\n-- try to generate code coverage\n\n")
    add_compile_options(-g -fprofile-arcs -ftest-coverage)
    link_libraries(gcov -fprofile-arcs -ftest-coverage)
    set(BUILD_TESTS ON)
  endif()
else()
  message(STATUS "Build in Release mode")
  message(STATUS "Build in Debug mode using command 'cmake -DCMAKE_BUILD_TYPE=Debug'")
  add_compile_options(-O3)
endif()

# keep the same with front-end language
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wold-style-cast")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# set the X_TRACE with trace type, they are:
# [FTRACE | ftrace] - 1
# [ LOG   | log   ] - 2
# [      ON       ] - 2
# [      OFF      ] - disable tracing
option(X_TRACE "tracing smart contract execution" OFF)
if(X_TRACE)
  add_definitions(-DX_TRACE_ENABLE)
  add_definitions(-DX_LOG_ENABLE)
  # only support ftrace in linux system
  if (COMPILER_IS_GNU AND
      (X_TRACE STREQUAL "FTRACE" OR X_TRACE STREQUAL "ftrace"))
      add_definitions(-DX_TRACE_TYPE=1)
      message(STATUS "execution smart contract with ftrace tracing")
  else()
      add_definitions(-DX_TRACE_TYPE=2)
      message(STATUS "execution smart contract with log tracing")
  endif()
endif()

set(BuildConfig_FOUND TRUE)
