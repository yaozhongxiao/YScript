/*
 * Copyright 2023 Develop Group Participants. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "log/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <map>

#if !defined(PLUGINS_SANDBOX) && defined(FILE_LOGGER)
#include "logger.h" // NOLINT
#include <log4cplus/configurator.h>
#endif

namespace base {
static LogLevelType sLogLevel = LOG_INFO;

LogLevelType getLogLevel() { return sLogLevel; }

void setLogLevel(const LogLevelType &level) { sLogLevel = level; }

void logWrite(const LogLevelType &level, const char *tag, const char *fmt,
              ...) {
  if (level < sLogLevel) {
    // skip low level log
    return;
  }
  char buf[LOGGER_BUFFER_SIZE] = {0};
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, LOGGER_BUFFER_SIZE, fmt, ap);
  va_end(ap);
#ifdef PLUGINS_SANDBOX
  extern void logging(const uint32_t &level, const char *tag,
                      const char *message);
  logging(level, tag, buf);
#elif defined(FILE_LOGGER)
  Logger::GetInstance()->Logging(tag, buf, level);
#else
  printf("%s %s\n", tag, buf);
#endif
}

} // namespace base
