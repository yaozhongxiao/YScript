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

#ifndef YSCRIPT_COMMON_VERSION_H_
#define YSCRIPT_COMMON_VERSION_H_

#include <stdint.h>

//  v     0       1       01
//       major   minor    patch
#define FOREACH_VERSION(VERSION_DECLARE)                                       \
  VERSION_DECLARE(V0001 = 0, "YS-0.0.01")                                      \
  VERSION_DECLARE(V0101 = 1, "YS-0.1.01")

enum YSVersion {
#define VERSION_DECLARE(name, desc) name,
  FOREACH_VERSION(VERSION_DECLARE)
#undef VERSION_DECLARE
      VMAX
};

#define YSCRIPT_CURRENT_VERSION V0001

#define YSCRIPT_VERSION(block_version)                                         \
  (0xFFFF & (static_cast<uint64_t>(block_version) >> 48))

extern const char *kVersionDesc[YSVersion::VMAX];

#endif // YSCRIPT_COMMON_VERSION_H_
