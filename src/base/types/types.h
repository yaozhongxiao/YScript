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

#ifndef BASE_TYPES_TYPES_H_
#define BASE_TYPES_TYPES_H_

#include <memory>
#include <type_traits>
#include <utility>

namespace base {
#define PRIindex "u"
#define PRIaddress "u"
#define PRIoffset PRIzx

typedef uint32_t Index;   // An index into one of the many index spaces.
typedef uint32_t Address; // An address or size in linear memory.
typedef size_t Offset;    // An offset into a host's file or memory buffer.

struct v128 {
  uint32_t v[4];
};

static const Address kInvalidAddress = ~0;
static const Index kInvalidIndex = ~0;
static const Offset kInvalidOffset = ~0;

/**
 * redefine the types for lower compiler standard(c++11) and platform
 * dependencies.
 */
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <class T> using remove_cv_t = typename std::remove_cv<T>::type;

template <class T>
struct is_null_pointer : std::is_same<std::nullptr_t, remove_cv_t<T>> {};

/**
 * dynamic pointer cast functionality for unique_ptr
 * they won't always pass ownership back to the caller.
 */
template <typename Derived, typename Base>
inline std::unique_ptr<const Derived>
dynamic_unique_pointer_cast(std::unique_ptr<const Base> &&base) {
  return std::unique_ptr<const Derived>(
      static_cast<const Derived *>(base.release()));
};

template <typename Derived, typename Base>
inline std::unique_ptr<Derived>
dynamic_unique_pointer_cast(std::unique_ptr<Base> &&base) {
  return std::unique_ptr<Derived>(dynamic_cast<Derived *>(base.release()));
}

// This is named make_unique_ptr instead of make_unique because make_unique has
// the potential to conflict with std::make_unique if it is defined.
//
// On gcc/clang, we currently compile with c++11, which doesn't define
// std::make_unique, but on the newest C++ version is always used, which
// includes std::make_unique. If an argument from the std namespace is used, it
// will cause ADL to find std::make_unique, and an unqualified call to
// make_unique will be ambiguous. We can work around this by fully qualifying
// the call (i.e. base::make_unique_ptr), but it's simpler to just use a
// different name.
template <typename T, typename... Args>
std::unique_ptr<T> make_unique_ptr(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace base
#endif // BASE_TYPES_TYPES_H_
