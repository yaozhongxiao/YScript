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

set(BuildFunction_FOUND true)

#---------------------------------------------------------#
#            funtion to build the binary                  #
#---------------------------------------------------------#
function(build_executable target)
  set(optionArgs WITH_LIBM INSTALL)
  set(oneValueArgs NAME DESTINATION)
  set(multiValueArgs SOURCES LIBS GROUP_LIBS)
  cmake_parse_arguments("X" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Optionally link libm
  if (X_WITH_LIBM AND (COMPILER_IS_CLANG OR COMPILER_IS_GNU))
    set(X_LIBS "${X_LIBS};m")
  endif ()

  add_executable(${target} ${X_SOURCES})
  add_dependencies(target_all ${target}) # the custome target
  if(COMPILER_IS_CLANG)
    target_link_libraries(${target} PUBLIC
        ${X_GROUP_LIBS} ${X_LIBS})
  else()
    target_link_libraries(${target} PUBLIC
        "-Wl,--start-group" ${X_GROUP_LIBS} "-Wl,--end-group"
        ${X_LIBS})
  endif()
  if (X_INSTALL)
    list(APPEND X_XCUTABLES ${target})
    set(X_XCUTABLES ${X_XCUTABLES} PARENT_SCOPE)
    # set the target copy-to-${DESTINATION} directiry
    set(TARGET_DIR "out/bin")
    if (X_DESTINATION)
      set(TARGET_DIR "out/bin/${X_DESTINATION}")
    endif ()
    add_custom_target(${target}-copy-to-bin ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${X_SOURCE_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${X_SOURCE_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endfunction()

#---------------------------------------------------------#
#          funtion to build the static library            #
#---------------------------------------------------------#
function(build_static_library target)
  set(optionArgs INSTALL)
  set(oneValueArgs DESTINATION)
  set(multiValueArgs SOURCES LIBS GROUP_LIBS)
  cmake_parse_arguments("X" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_library(${target} STATIC ${X_SOURCES})
  add_dependencies(target_all ${target}) # the custome target
  if(COMPILER_IS_CLANG)
    target_link_libraries(${target} PUBLIC
      ${X_GROUP_LIBS} ${X_LIBS})
  else()
    target_link_libraries(${target} PUBLIC
      "-Wl,--start-group" ${X_GROUP_LIBS} "-Wl,--end-group"
      ${X_LIBS})
  endif()
  if (X_INSTALL)
    list(APPEND X_INSTALL_LIBS ${target})
    set(X_INSTALL_LIBS ${X_INSTALL_LIBS} PARENT_SCOPE)
    set(TARGET_DIR "out/libs")
    if (X_INSTALL)
      set(TARGET_DIR "out/libs/${X_DESTINATION}")
    endif ()
    add_custom_target(${target}-copy-to-libs ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${X_SOURCE_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${X_SOURCE_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endfunction()

#---------------------------------------------------------#
#          funtion to build the shared library            #
#---------------------------------------------------------#
function(build_shared_library target)
  set(optionArgs INSTALL)
  set(oneValueArgs DESTINATION)
  set(multiValueArgs SOURCES LIBS MERGE_LIBS GROUP_LIBS)
  cmake_parse_arguments("X" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_library(${target} SHARED ${X_SOURCES})
  add_dependencies(target_all ${target}) # the custome target

  # NOTE(zhongxiao.yzx): X_MERGE_LIBS is the libs need to be merged into target
  # "A link flag: Item names starting with -, but not -l or -framework, are treated as linker flags"
  if(COMPILER_IS_CLANG)
    target_link_libraries(${target} PUBLIC
      "-Wl,-all_load" ${X_MERGE_LIBS} "-Wl,-noall_load"
      ${X_GROUP_LIBS} ${X_LIBS})
  else()
    target_link_libraries(${target} PUBLIC
      "-Wl,--whole-archive" ${X_MERGE_LIBS} "-Wl,--no-whole-archive"
      "-Wl,--start-group" ${X_GROUP_LIBS} "-Wl,--end-group"
      ${X_LIBS})
  endif()

  if (X_INSTALL)
    list(APPEND X_INSTALL_LIBS ${target})
    set(X_INSTALL_LIBS ${X_INSTALL_LIBS} PARENT_SCOPE)
    set(TARGET_DIR "out/libs")
    if (X_INSTALL)
      set(TARGET_DIR "out/libs/${X_DESTINATION}")
    endif ()
    add_custom_target(${target}-copy-to-libs ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${X_SOURCE_DIR}/${TARGET_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${X_SOURCE_DIR}/${TARGET_DIR}
      DEPENDS ${target}
    )
  endif ()
endfunction()
