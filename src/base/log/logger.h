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

#ifndef BASE_LOG_LOGGER_H_
#define BASE_LOG_LOGGER_H_

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <cstdlib>
#include <map>
#include <string>

#include "log/log.h"

namespace base {

class Logger {
public:
  Logger();
  ~Logger();

  static Logger *GetInstance() {
    static Logger log_instance;
    return &log_instance;
  }
  bool InitLogging(const std::string &config_file = "../conf/log.conf",
                   LogLevelType log_level = LOG_DEBUG);
  std::string Getloglevel(const std::string &log_name);
  void Setloglevel(const std::string &log_name, LogLevelType level);
  log4cplus::Logger GetLogger(const std::string &log_name);
  void Logging(const char *tag, char *buf, LogLevelType level);

  bool is_init_ = false;
  LogLevelType default_level_;
  log4cplus::Logger vm_logger_;
  std::map<std::string, log4cplus::Logger> log_instance_;

  static std::map<LogLevelType, log4cplus::LogLevel> level_maps_;
  static const char *s_base_logger_;
};

} // namespace base
#endif // BASE_LOG_LOGGER_H_
