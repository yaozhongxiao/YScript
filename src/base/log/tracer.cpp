/*
 * Copyright 2023 Develop Group Participants; All rights reserved.
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
 */

#include "log/tracer.h"
#include "log/log.h"

namespace base {

char Tracer::s_logger_buffer_[LOGGER_BUFFER_SIZE] = {0};
#if X_TRACE_TYPE == FTRACER // with ftrace
TraceMarker Tracer::trace_marker_;
#endif

int Tracer::GetTracerId() {
#if X_TRACE_TYPE == FTRACER
  return trace_marker_.GetFd();
#endif
  return -1;
}

AutoTracer::AutoTracer(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char buf[LOGGER_BUFFER_SIZE];
  vsnprintf(buf, LOGGER_BUFFER_SIZE, fmt, ap);
  va_end(ap);
  message_.assign(buf);
  Tracer::TraceBegin(buf);
}

AutoTracer::~AutoTracer() { Tracer::TraceEnd(message_.c_str()); }

} // namespace base
