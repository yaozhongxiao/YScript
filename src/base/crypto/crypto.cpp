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
 *
 */

#include "crypto/crypto.h"

#include <string>
#include <vector>

namespace base {

void base64Encode(char *input_data, uint32_t in_len, char *output_data,
                  uint32_t &out_len) {
  const char *table =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  uint32_t i = in_len;
  uint32_t j = 0;
  char *current = input_data;
  while (i > 2) {
    char a = current[0];
    char b = current[1];
    char c = current[2];
    i -= 3;
    current += 3;

    output_data[j++] = table[(a >> 2) & 0x3f];
    output_data[j++] = table[((b >> 4) & 0x0f) | ((a & 0x03) << 4)];
    output_data[j++] = table[((b & 0x0f) << 2) | (0x03 & (c >> 6))];
    output_data[j++] = table[c & 0x3f];
  }
  if (i == 1) {
    output_data[j++] = table[(current[0] >> 2) & 0x3f];
    output_data[j++] = table[(current[0] & 0x03) << 4];
    output_data[j++] = '=';
    output_data[j++] = '=';
  } else if (i == 2) {
    output_data[j++] = table[(current[0] >> 2) & 0x3f];
    output_data[j++] =
        table[((current[0] & 0x03) << 4) | (0x0f & (current[1] >> 4))];
    output_data[j++] = table[(current[1] & 0x0f) << 2];
    output_data[j++] = '=';
  }
  out_len = j;
}

bool base64Decode(char *input_data, uint32_t in_len, char *output_data,
                  uint32_t &out_len) {
  const char DecodeTable[] = {
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63, 52, 53, 54, 55, 56, 57,
      58, 59, 60, 61, -2, -2, -2, -2, -2, -2, -2, 0,  1,  2,  3,  4,  5,  6,
      7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
      25, -2, -2, -2, -2, -2, -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
      37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
      -2, -2, -2, -2};
  int bin = 0, i = 0;
  const char *current = input_data;
  char ch;
  uint32_t index = 0;
  if (in_len == 0) {
    return false;
  }

  if ((in_len & 0x03) != 0) {
    return false;
  }

  size_t padding = 0;
  if (input_data[in_len - 1] == '=')
    padding++;
  if (input_data[in_len - 2] == '=')
    padding++;

  size_t expect_len = (in_len >> 2) * 3 - padding;
  if (out_len < expect_len) {
    return false;
  }
  while (in_len-- > 0) {
    ch = *current++;
    if (ch == '=') {
      if (in_len != 0 && in_len != 1) {
        return false;
      }
      if ((in_len > 0) && (*current) != '=' && (i & 0x03) == 1) {
        return false;
      }
      continue;
    }
    ch = DecodeTable[static_cast<int>(ch)];

    if (ch < 0) {
      return false;
    }
    switch (i & 0x03) {
    case 0:
      bin = ch << 2;
      break;
    case 1:
      bin |= ch >> 4;
      output_data[index++] = bin;
      bin = (ch & 0x0f) << 4;
      break;
    case 2:
      bin |= ch >> 2;
      output_data[index++] = bin;
      bin = (ch & 0x03) << 6;
      break;
    case 3:
      bin |= ch;
      output_data[index++] = bin;
      break;
    }
    i++;
  }
  if (index != expect_len) {
    return false;
  }
  out_len = expect_len;
  return true;
}
} // namespace base
