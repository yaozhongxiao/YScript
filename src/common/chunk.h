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

#ifndef YSCRIPT_COMMON_CHUNK_H_
#define YSCRIPT_COMMON_CHUNK_H_

#include "common/config.h"
#include "common/opcode.h"
#include "common/ysvalue.h"

typedef struct {
  // count
  int count;
  // capacity
  int capacity;
  // chunk data
  uint8_t *code;
  // chunk-lines
  int *lines;
  // chunk-constants
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);

#endif // YSCRIPT_COMMON_CHUNK_H_
