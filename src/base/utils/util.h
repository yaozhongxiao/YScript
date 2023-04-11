/**
 * Copyright 2023 Develop Group Participants. All rights reserved.
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

#ifndef BASE_UTILS_UTIL_H_
#define BASE_UTILS_UTIL_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include "log/log.h"

namespace base {

#include <stdio.h>
#define X_PRINTF(...) fprintf(stderr, __VA_ARGS__);

/************************************ attributes
 * *********************************/
// Use __thread instead of the C++11 thread_local because Apple's clang doesn't
// support thread_local yet.
#define THREAD_LOCAL thread_local

#define SYMBOL_EXPORT __attribute__((visibility("default")))
#define SYMBOL_HIDDEN __attribute__((visibility("hidden")))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define PACKED_STRUCT(definition) definition __attribute__((packed));

/**************************** auxiliary variadic parameters
 * ************************/
// try to count the variadic parameters
/*
 * #define ARGS_COUNT(...) \
 *    ARG_COUNT_PRIVATE(0, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
 *
 *   ARGS_COUNT do NOT have the same behavior in different c++ standard,
 *   because, whether will ##__VA_ARGS__ swallow the comma is not c++ standard.
 *   when build with following command, you will get different output
 *       g++ -g tid.cpp -o tid [default | --std=c++11 | c++14 | c++17]
 *   Macos: ARGS_COUNT() == 0 with all build options
 *   Linux: ARGS_COUNT() == 0 in default options
 *          but ARGS_COUNT() == 1 with c++11 c++14 c++17
 * See :
 * ##__VA_ARGS__ not swallowing comma when zero args under C99
 * https://stackoverflow.com/questions/37206118/va-args-not-swallowing
 * -comma-when-zero-args-under-c99
 *
 * After Testing, we find the ARGS_COUNT_PLUGIN_1 Macro have the consistent
 * behavior in different compiler flags(default, c++11, c++14, c++17)
 * It seems link "##__VA_ARGS__" will always swallow comma when the prefix
 * args exists("__" in ARGS_COUNT_PLUGIN_1)
 *
 */

#define ARGS_COUNT(...)                                                        \
  ARG_COUNT_PRIVATE(0, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define ARGS_COUNT_PLUGIN_1(__, ...)                                           \
  ARG_COUNT_PRIVATE(0, ##__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define ARG_COUNT_PRIVATE(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

template <typename... args> struct ArgsCounter {
  constexpr const static int &value = sizeof...(args);
};
#define VA_ARGS_COUNT_PLUGIN(...) (ArgsCounter<__VA_ARGS__>::value)

// repeat the macro
#define MACRO_REPEAT0(m, p)
#define MACRO_REPEAT1(m, p) m(1, p)
#define MACRO_REPEAT2(m, p) m(1, p), m(2, p)
#define MACRO_REPEAT3(m, p) MACRO_REPEAT2(m, p), m(3, p)
#define MACRO_REPEAT4(m, p) MACRO_REPEAT3(m, p), m(4, p)
#define MACRO_REPEAT5(m, p) MACRO_REPEAT4(m, p), m(5, p)
#define MACRO_REPEAT6(m, p) MACRO_REPEAT5(m, p), m(6, p)
#define MACRO_REPEAT7(m, p) MACRO_REPEAT6(m, p), m(7, p)
#define MACRO_REPEAT8(m, p) MACRO_REPEAT7(m, p), m(8, p)
#define MACRO_REPEAT9(m, p) MACRO_REPEAT8(m, p), m(9, p)
#define MACRO_REPEAT10(m, p) MACRO_REPEAT9(m, p), m(10, p)

#define MACRO_REPEAT(n, m, p) MACRO_REPEAT##n(m, p)

// repeat generate the std::placeholders::_N
#define PLACE_HODLER(N, P) std::placeholders::_##N
#define PLACE_HODLER_REPEAT(n) MACRO_REPEAT(n, PLACE_HODLER, "_")

/**************************** utility methods ************************/
// forbid class copy and assign
#define DISALLOW_COPY_AND_ASSIGN(CLASS)                                        \
  CLASS(const CLASS &) = delete;                                               \
  CLASS &operator=(const CLASS &) = delete;                                    \
  CLASS(CLASS &&) = delete;                                                    \
  CLASS &operator=(CLASS &&) = delete;

template <typename Dst, typename Src> ALWAYS_INLINE Dst Bitcast(Src &&value) {
  static_assert(sizeof(Src) == sizeof(Dst), "Bitcast sizes must match.");
  Dst result;
  memcpy(&result, &value, sizeof(result));
  return result;
}

template <typename T> ALWAYS_INLINE void ZeroMemory(T &v) {
  X_STATIC_ASSERT(std::is_pod<T>::value);
  memset(&v, 0, sizeof(v));
}

// Placement construct
template <typename T, typename... Args>
void Construct(T &placement, Args &&... args) {
  new (&placement) T(std::forward<Args>(args)...);
}

// Placement destruct
template <typename T> void Destruct(T &placement) { placement.~T(); }

static inline uint32_t BytesToUInt32(const uint8_t *data) {
  uint32_t n = 0;
  for (size_t i = 0; i < sizeof(uint32_t); ++i) {
    n <<= 8;
    n |= data[i];
  }
  return n;
}

// little endian
static inline uint32_t BytesToUInt32Little(const uint8_t *data) {
  uint32_t n = 0;
  for (size_t i = 0; i < sizeof(uint32_t); ++i) {
    n <<= 8;
    n |= data[sizeof(uint32_t) - 1 - i];
  }
  return n;
}

// big endia
static inline std::vector<uint8_t> UInt32ToBytes(uint32_t value) {
  std::vector<uint8_t> bytes;
  bytes.push_back(value >> 24 & 0xff);
  bytes.push_back(value >> 16 & 0xff);
  bytes.push_back(value >> 8 & 0xff);
  bytes.push_back(value & 0xff);
  return bytes;
}

static inline std::string Bin2Hex(const std::string &input) {
  std::string out;
  const char *dict = "0123456789abcdef";
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    int h = 0x0f & (c >> 4);
    out.push_back(dict[h]);
    int l = 0x0f & c;
    out.push_back(dict[l]);
  }
  return out;
}

static inline std::string Bytes2Hex(const std::vector<uint8_t> &input) {
  std::string out;
  const char *dict = "0123456789abcdef";
  for (size_t i = 0; i < input.size(); i++) {
    uint8_t c = input[i];
    int h = 0x0f & (c >> 4);
    out.push_back(dict[h]);
    int l = 0x0f & c;
    out.push_back(dict[l]);
  }
  return out;
}

static inline int FromHexChar(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

static inline std::string Hex2Bin(const std::string &input) {
  std::string out = "";
  for (size_t i = 0; i < input.size(); i += 2) {
    int h = FromHexChar(input[i]);
    int l = FromHexChar(input[i + 1]);
    if (h != -1 && l != -1)
      out.push_back(static_cast<unsigned char>((h << 4) | l));
    else
      return std::string("");
  }
  return out;
}

static inline std::vector<uint8_t> Hex2Bytes(const std::string &input) {
  std::vector<uint8_t> out;
  for (size_t i = 0; i < input.size(); i += 2) {
    int h = FromHexChar(input[i]);
    int l = FromHexChar(input[i + 1]);
    if (h != -1 && l != -1)
      out.push_back(static_cast<unsigned char>((h << 4) | l));
    else
      return std::vector<uint8_t>();
  }
  return out;
}

} // namespace base
#endif // BASE_UTILS_UTIL_H_
