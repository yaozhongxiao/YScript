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

#include "logger.h" // NOLINT

#include <log4cplus/configurator.h>
#include <stdio.h>

#include <map>
#include <string>

#include "file/file.h"
#include "log/log.h"

namespace base {

const char *Logger::s_base_logger_ = "runtime";
static const char *kLoggerDefaultConf = R"(
    log4cplus.logger.runtime=TRACE,runtime_log
    log4cplus.appender.runtime_log=log4cplus::RollingFileAppender
    log4cplus.appender.runtime_log.MaxFileSize=100MB
    log4cplus.appender.runtime_log.MaxBackupIndex=10
    log4cplus.appender.runtime_log.File=log/runtime.log
    log4cplus.appender.runtime_log.CreateDirs=true
    log4cplus.appender.runtime_log.layout=log4cplus::PatternLayout
    log4cplus.appender.runtime_log.layout.ConversionPattern=[%D{%Y-%m-%d %H:%M:%S:%Q}] [p:%i/t:%T] [%c:%-5p] %m%n
    log4cplus.appender.runtime_log.filters.1=log4cplus::spi::LogLevelRangeFilter
    log4cplus.appender.runtime_log.filters.1.LogLevelMin=TRACE
    log4cplus.appender.runtime_log.filters.1.LogLevelMax=FATAL
    log4cplus.appender.runtime_log.filters.1.AcceptOnMatch=true
    log4cplus.appender.runtime_log.filters.2=log4cplus::spi::DenyAllFilter
)";

std::map<LogLevelType, log4cplus::LogLevel> Logger::level_maps_ = {
    {LogLevelType::LOG_VERBOSE, log4cplus::TRACE_LOG_LEVEL},
    {LogLevelType::LOG_DEBUG, log4cplus::DEBUG_LOG_LEVEL},
    {LogLevelType::LOG_INFO, log4cplus::INFO_LOG_LEVEL},
    {LogLevelType::LOG_WARNING, log4cplus::WARN_LOG_LEVEL},
    {LogLevelType::LOG_ERROR, log4cplus::ERROR_LOG_LEVEL},
    {LogLevelType::LOG_FATAL, log4cplus::FATAL_LOG_LEVEL},
};

Logger::Logger() { InitLogging(); }

Logger::~Logger() { log4cplus::Logger::shutdown(); }

bool Logger::InitLogging(const std::string &config_file,
                         LogLevelType log_level) {
  if (is_init_) {
    return true;
  }
  log4cplus::initialize();
  if (!File::IsFile(config_file)) {
#ifdef X_DEBUG
    fprintf(stderr,
            "can not find logger conf %s\ntry to use default logger conf:\n%s",
            config_file.c_str(), kLoggerDefaultConf);
#endif
    log4cplus::tistringstream conf_stream(kLoggerDefaultConf);
    log4cplus::PropertyConfigurator configer(conf_stream);
    configer.configure();
  } else {
    log4cplus::PropertyConfigurator::doConfigure(config_file);
  }
  is_init_ = true;

  // try to set log level
  vm_logger_ = GetLogger(s_base_logger_);
  log4cplus::LogLevel cplus_level = vm_logger_.getChainedLogLevel();
  for (auto level_item : level_maps_) {
    if (level_item.second == cplus_level) {
      log_level = level_item.first;
      break;
    }
  }
  setLogLevel(log_level);
  return true;
}

void Logger::Setloglevel(const std::string &log_name, LogLevelType log_level) {
  vm_logger_.setLogLevel(level_maps_[log_level]);
}

std::string Logger::Getloglevel(const std::string &log_name) {
  return log4cplus::getLogLevelManager().toString(
      vm_logger_.getChainedLogLevel());
}

log4cplus::Logger Logger::GetLogger(const std::string &log_name) {
  return log4cplus::Logger::getInstance(log_name);
}

void Logger::Logging(const char *tag, char *buf, LogLevelType level) {
  switch (level) {
  case LogLevelType::LOG_VERBOSE:
    LOG4CPLUS_TRACE_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  case LogLevelType::LOG_DEBUG:
    LOG4CPLUS_DEBUG_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  case LogLevelType::LOG_INFO:
    LOG4CPLUS_INFO_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  case LogLevelType::LOG_WARNING:
    LOG4CPLUS_WARN_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  case LogLevelType::LOG_ERROR:
    LOG4CPLUS_ERROR_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  case LogLevelType::LOG_FATAL:
    LOG4CPLUS_FATAL_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  default:
    LOG4CPLUS_DEBUG_FMT(vm_logger_, "%s %s", tag, buf);
    break;
  }
}

} // namespace base
