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
 *
 */

#ifndef BASE_LOG_LOG_H_
#define BASE_LOG_LOG_H_

#include <assert.h>
#include <stdint.h>

#include <map>
#include <string>

namespace base {

enum LogLevelType : uint8_t {
  LOG_VERBOSE = 1,
  LOG_DEBUG = 2,
  LOG_INFO = 3,
  LOG_WARNING = 4,
  LOG_ERROR = 5,
  LOG_FATAL = 6
};

#define LOGGER_BUFFER_SIZE 1024

#define STRINGIFY(ARG) #ARG
// #define STRCAT(ARG1, ARG2) (STRINGIFY(ARG1) " " STRINGIFY(ARG2))
#define STRCAT(ARG1, ARG2) (STRINGIFY(ARG2))
#define LOG_TAG

#define LOG_V(...)                                                             \
  logWrite(LOG_VERBOSE, STRCAT([Verbose] :, LOG_TAG), __VA_ARGS__)
#define LOG_D(...) logWrite(LOG_DEBUG, STRCAT([Debug] :, LOG_TAG), __VA_ARGS__)
#define LOG_I(...) logWrite(LOG_INFO, STRCAT([Info] :, LOG_TAG), __VA_ARGS__)
#define LOG_W(...) logWrite(LOG_WARNING, STRCAT([Warn] :, LOG_TAG), __VA_ARGS__)
#define LOG_E(...) logWrite(LOG_ERROR, STRCAT([Error] :, LOG_TAG), __VA_ARGS__)
#define LOG_F(...)                                                             \
  do {                                                                         \
    logWrite(LOG_FATAL, STRCAT([Fatal] :, LOG_TAG), __VA_ARGS__);              \
    assert(0);                                                                 \
  } while (0);

LogLevelType getLogLevel();
void setLogLevel(const LogLevelType &level);
void logWrite(const LogLevelType &level, const char *tag, const char *fmt, ...);

} // namespace base
#endif // BASE_LOG_LOG_H_
