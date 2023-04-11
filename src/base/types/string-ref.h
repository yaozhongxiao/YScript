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

#ifndef BASE_TYPES_STRING_REF_H_
#define BASE_TYPES_STRING_REF_H_

#include <string_view>

namespace base {

class StringRef : public std::string_view {
public:
  /**
   *  Translate string to integer
   *
   *  @brief Translate string to integer
   *  @param str - The str format of the integer
   *  @return integer value
   */
  template <typename T> static T to_integer(const std::string &str) {
    T t;
    std::stringstream stream;
    stream << str << std::boolalpha;
    stream >> t;
    return t;
  }
};

} // namespace base
#endif // BASE_TYPES_STRING_REF_H_
