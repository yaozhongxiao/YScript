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

set(TestUtil_FOUND false)

include(CTest)
# This must be set in the root directory for the tests to be run by
# 'make test' or ctest.
enable_testing()

#---------------------------------------------------------#
#        build the testing binary with google/gtest       #

# add_gtest (target_name
#    *.cc  *.cpp
#    INSTALL  ${install_dir}
#    LIBS  -lm -lpthread ...
# )
macro(add_gtest target)
  set(optionArgs)
  set(oneValueArgs INSTALL)
  set(multiValueArgs LIBS)
  cmake_parse_arguments("ADD_GTEST" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  add_executable(${target} ${ADD_GTEST_UNPARSED_ARGUMENTS})
  # FIXME(): there is no gtest_main in google/benchmark, use gmock_main instead
  target_link_libraries(${target} PUBLIC ${ADD_GTEST_LIBS} gmock_main)
  
  if (${INSTALL})
    # set the target copy-to-${DESTINATION} directiry
    set(TARGET_DIR ${INSTALL})
    add_custom_target(${target}-install ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BIN_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_BIN_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endmacro()

# add_gmock (target_name
#    *.cc  *.cpp
#    INSTALL  ${install_dir}
#    LIBS  -lm -lpthread ...
# )
macro(add_gmock target)
  set(optionArgs)
  set(oneValueArgs INSTALL)
  set(multiValueArgs LIBS)
  cmake_parse_arguments("ADD_GTEST" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  add_executable(${target} ${ADD_GTEST_UNPARSED_ARGUMENTS})
  target_link_libraries(${target} PUBLIC ${ADD_GTEST_LIBS} gmock_main ${CMAKE_THREAD_LIBS_INIT})
  
  if (${INSTALL})
    # set the target copy-to-${DESTINATION} directiry
    set(TARGET_DIR ${INSTALL})
    add_custom_target(${target}-install ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BIN_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_BIN_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endmacro()

#---------------------------------------------------------#
#    build the benchmark binary with google/benchamark    #

# add_benchmark (target_name
#    *.cc  *.cpp
#    INSTALL  ${install_dir}
#    LIBS  -lm -lpthread ...
# )
macro(add_benchmark target)
  set(optionArgs)
  set(oneValueArgs INSTALL)
  set(multiValueArgs LIBS)
  cmake_parse_arguments("ADD_GTEST" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
 
  add_executable(${target} ${ADD_GTEST_UNPARSED_ARGUMENTS})
  target_link_libraries(${target} PUBLIC ${ADD_GTEST_LIBS} benchmark_main)

  if (${INSTALL})
    # set the target copy-to-${DESTINATION} directiry
    set(TARGET_DIR ${INSTALL})
    add_custom_target(${target}-install ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BIN_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_BIN_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endmacro()

#---------------------------------------------------------#
#         build the test-suit with ctest                  #

# add_test (target_name
#    *.cc  *.cpp
#    INSTALL  ${install_dir}
#    LIBS  -lm -lpthread ...
# )
macro(add_ctest name)
    add_gtest(${ARGV})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES LABELS "unittest")
endmacro()

macro(add_benchmark_ctest name)
    add_benchmark(${ARGV})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES LABELS "benchmark")
endmacro()

#---------------------------------------------------------#
add_custom_target(target_all)

# print_property (target INTERFACE_INCLUDE_DIRECTORIES)
function(print_property target prop)
  add_custom_target(print_${target}_${prop}
    COMMAND ${CMAKE_COMMAND} -E
      echo ${target}.${prop}: $<TARGET_GENEX_EVAL:${target},$<TARGET_PROPERTY:${target},${prop}>>
  )
  add_dependencies(target_all print_${target}_${prop})
endfunction()

set(TestUtil_FOUND true)
