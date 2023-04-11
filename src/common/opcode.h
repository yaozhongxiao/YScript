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

#ifndef YSCRIPT_COMMON_OPCODE_H_
#define YSCRIPT_COMMON_OPCODE_H_

typedef enum {
  // op-constant
  OP_CONSTANT,
  // Types of Values literal-ops
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  // Global Variables pop-op
  OP_POP,
  // Local Variables get-local-op
  OP_GET_LOCAL,
  // Local Variables set-local-op
  OP_SET_LOCAL,
  // Global Variables get-global-op
  OP_GET_GLOBAL,
  // Global Variables define-global-op
  OP_DEFINE_GLOBAL,
  // Global Variables set-global-op
  OP_SET_GLOBAL,
  // Closures upvalue-ops
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  // Classes and Instances property-ops
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  // Superclasses get-super-op
  OP_GET_SUPER,
  // Types of Values comparison-ops
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  // A Virtual Machine binary-ops
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  // Types of Values not-op
  OP_NOT,
  // A Virtual Machine negate-op
  OP_NEGATE,
  // Global Variables op-print
  OP_PRINT,
  // Jumping Back and Forth jump-op
  OP_JUMP,
  // Jumping Back and Forth jump-if-false-op
  OP_JUMP_IF_FALSE,
  // Jumping Back and Forth loop-op
  OP_LOOP,
  // Calls and Functions op-call
  OP_CALL,
  // Methods and Initializers invoke-op
  OP_INVOKE,
  // Superclasses super-invoke-op
  OP_SUPER_INVOKE,
  // Closures closure-op
  OP_CLOSURE,
  // Closures close-upvalue-op
  OP_CLOSE_UPVALUE,
  // Return function/method
  OP_RETURN,
  // Classes and Instances class-op
  OP_CLASS,
  // Superclasses inherit-op
  OP_INHERIT,
  // Methods and Initializers method-op
  OP_METHOD
} OpCode;

#endif // YSCRIPT_COMMON_OPCODE_H_
