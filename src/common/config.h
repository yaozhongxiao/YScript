/**
 * Copyright 2023 Develop Group Participants; All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YSCRIPT_COMMON_CONFIG_H_
#define YSCRIPT_COMMON_CONFIG_H_

#include <cassert>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// #define ENABLE_NAN_TAGGING

// #define ENABLE_FORCE_GC
// #define ENABLE_GC_LOGGING

// #define ENABLE_LOGGING

// #define ENABLE_INTERP_TRACE
// #define ENABLE_COMPILE_TRACE

#define UINT8_COUNT (UINT8_MAX + 1)

#define YSCRIPT_UNREACHABLE(msg) abort();

#endif // YSCRIPT_COMMON_CONFIG_H_