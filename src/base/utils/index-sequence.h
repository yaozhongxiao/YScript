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

#ifndef BASE_UTILS_INDEX_SEQUENCE_H_
#define BASE_UTILS_INDEX_SEQUENCE_H_

#include <cstddef>

namespace base {

template <std::size_t... Ints> struct index_sequence {
  typedef std::size_t value_type;

  static std::size_t size() { return sizeof...(Ints); }

  // non standard extension
  typedef index_sequence type;
};

template <typename Left, typename Right> struct _make_index_sequence_join;

template <std::size_t... Left, std::size_t... Right>
struct _make_index_sequence_join<index_sequence<Left...>,
                                 index_sequence<Right...>>
    : index_sequence<Left..., (sizeof...(Left) + Right)...> {};

template <std::size_t N>
struct make_index_sequence
    : _make_index_sequence_join<typename make_index_sequence<N / 2>::type,
                                typename make_index_sequence<N - N / 2>::type> {
};

template <> struct make_index_sequence<1> : index_sequence<0> {};

template <> struct make_index_sequence<0> : index_sequence<> {};

} // namespace base
#endif // BASE_UTILS_INDEX_SEQUENCE_H_
