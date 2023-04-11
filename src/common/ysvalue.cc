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

#include <stdio.h>
#include <string.h>

#include "common/memory.h"
#include "common/ysobject.h"
#include "common/ysvalue.h"

void initValueArray(ValueArray *array) {
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}

void writeValueArray(ValueArray *array, Value value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values =
        GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
  }
  array->values[array->count] = value;
  array->count++;
}

void freeValueArray(ValueArray *array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}

void printValue(Value value) {
#ifdef ENABLE_NAN_TAGGING
  if (IS_BOOL(value)) {
    printf(AS_BOOL(value) ? "true" : "false");
  } else if (IS_NIL(value)) {
    printf("nil");
  } else if (IS_NUMBER(value)) {
    printf("%g", AS_NUMBER(value));
  } else if (IS_OBJ(value)) {
    printObject(value);
  }
#else
  switch (value.type) {
  case VAL_BOOL:
    printf(AS_BOOL(value) ? "true" : "false");
    break;
  case VAL_NIL:
    printf("nil");
    break;
  case VAL_NUMBER:
    printf("%g", AS_NUMBER(value));
    break;

  case VAL_OBJ:
    printObject(value);
    break;
  }
#endif
}

bool valuesEqual(Value a, Value b) {
#ifdef ENABLE_NAN_TAGGING
  if (IS_NUMBER(a) && IS_NUMBER(b)) {
    return AS_NUMBER(a) == AS_NUMBER(b);
  }
  return a == b;
#else
  if (a.type != b.type)
    return false;
  switch (a.type) {
  case VAL_BOOL:
    return AS_BOOL(a) == AS_BOOL(b);
  case VAL_NIL:
    return true;
  case VAL_NUMBER:
    return AS_NUMBER(a) == AS_NUMBER(b);
    /* Strings strings-equal < Hash Tables equal
        case VAL_OBJ: {
          ObjString* aString = AS_STRING(a);
          ObjString* bString = AS_STRING(b);
          return aString->length == bString->length &&
              memcmp(aString->chars, bString->chars,
                     aString->length) == 0;
        }
     */

  case VAL_OBJ:
    return AS_OBJ(a) == AS_OBJ(b);

  default:
    YSCRIPT_UNREACHABLE();
    return false; // Unreachable.
  }

#endif
}
