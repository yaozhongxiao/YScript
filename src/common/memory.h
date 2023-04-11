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

#ifndef YSCRIPT_COMMON_MEMORY_H_
#define YSCRIPT_COMMON_MEMORY_H_

#include "common/config.h"
#include "common/ysobject.h"

#define ALLOCATE(type, count)                                                  \
  static_cast<type *>(reallocate(NULL, 0, sizeof(type) * (count)))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, oldCount, newCount)                          \
  static_cast<type *>(reallocate(pointer, sizeof(type) * (oldCount),           \
                                 sizeof(type) * (newCount)))

#define FREE_ARRAY(type, pointer, oldCount)                                    \
  reallocate(pointer, sizeof(type) * (oldCount), 0)

void *reallocate(void *pointer, size_t oldSize, size_t newSize);

void markObject(Obj *object);

void markValue(Value value);

void collectGarbage();

void freeObjects();

#endif // YSCRIPT_COMMON_MEMORY_H_
