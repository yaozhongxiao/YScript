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

#ifndef BASE_LOG_TRACER_H_
#define BASE_LOG_TRACER_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cstdarg>
#include <string>

#include "log/log.h"
#include "log/trace-marker.h"

namespace base {
#define FTRACER 1
#define LOG_TRACER 2

class Tracer {
public:
  static int GetTracerId();

  static inline bool TraceBegin(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(s_logger_buffer_, LOGGER_BUFFER_SIZE, fmt, ap);
    va_end(ap);
#if X_TRACE_TYPE == FTRACER // with ftrace
    LOG_D("FTRACER TraceBegin %s", s_logger_buffer_);
    trace_marker_.WriteBegin(s_logger_buffer_);
#else
    LOG_I("%s Begin", s_logger_buffer_);
#endif
    return true;
  }

  static inline bool TraceEnd(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(s_logger_buffer_, LOGGER_BUFFER_SIZE, fmt, ap);
    va_end(ap);
#if X_TRACE_TYPE == FTRACER // with ftrace
    trace_marker_.WriteEnd(s_logger_buffer_);
    LOG_D("FTRACER TraceEnd %s", s_logger_buffer_);
#else
    LOG_I("%s End", s_logger_buffer_);
#endif
    return true;
  }

private:
  // optimiaze the performace for debugger
  static char s_logger_buffer_[LOGGER_BUFFER_SIZE];
#if X_TRACE_TYPE == FTRACER // with ftrace
  static TraceMarker trace_marker_;
#endif
};

class AutoTracer : public Tracer {
public:
  explicit AutoTracer(const char *fmt, ...);
  ~AutoTracer();

private:
  std::string message_;
};

#ifdef X_TRACE_ENABLE // turn on tracing
#define X_TRACE_BEGIN(...) base::Tracer::TraceBegin(__VA_ARGS__)
#define X_TRACE_END(...) base::Tracer::TraceEnd(__VA_ARGS__)
#define X_AUTO_TRACE(...) base::AutoTracer _auto_trace_(__VA_ARGS__)

#else // trun off tracing
#define X_TRACE_BEGIN(...)
#define X_TRACE_END(...)
#define X_AUTO_TRACE(...)
#endif

} // namespace base
#endif // BASE_LOG_TRACER_H_
