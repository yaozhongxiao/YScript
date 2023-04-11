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

#ifndef YSCRIPT_COMPILER_HASHTABLE_H_
#define YSCRIPT_COMPILER_HASHTABLE_H_

#include "common/config.h"
#include "common/ysvalue.h"

typedef struct {
  ObjString* key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);

bool tableGet(Table* table, ObjString* key, Value* value);
bool tableSet(Table* table, ObjString* key, Value value);
bool tableDelete(Table* table, ObjString* key);

void tableAddAll(Table* from, Table* to);

ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash);

void tableRemoveWhite(Table* table);
void markTable(Table* table);

#endif // YSCRIPT_COMPILER_HASHTABLE_H_
