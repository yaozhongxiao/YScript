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

#include "log/trace-marker.h"
#include "log/log.h"

#ifndef PLUGINS_SANDBOX
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#else
extern void _trace_printf(const char *name, char is_end);
#endif

namespace base {
static const char kTracerMarker[] = "/sys/kernel/debug/tracing/trace_marker";

TraceMarker::TraceMarker() : fd_(-1) {
#ifndef PLUGINS_SANDBOX
  fd_ = open(kTracerMarker, O_WRONLY); // O_WRONLY == 1;
#endif
}

TraceMarker::~TraceMarker() {
#ifndef PLUGINS_SANDBOX
  if (fd_ != -1) {
    close(fd_);
  }
#endif
}

ssize_t TraceMarker::WriteBegin(const char *message) {
  size_t write_size = 0;
#ifndef PLUGINS_SANDBOX
  if (fd_ == -1) {
    LOG_E("try to %s with invalid tracer marker!", __func__);
    return 0;
  }
  char buf[LOGGER_BUFFER_SIZE];
  int len = snprintf(buf, LOGGER_BUFFER_SIZE, "B|%d|%s", getpid(), message);
  write_size = write(fd_, buf, len);
#else
  _trace_printf(message, 0);
  write_size = strlen(message);
#endif
  return write_size;
}

ssize_t TraceMarker::WriteEnd(const char *message) {
  size_t write_size = 0;
#ifndef PLUGINS_SANDBOX
  if (fd_ == -1) {
    LOG_E("try to %s with invalid tracer marker!", __func__);
    return 0;
  }
  char c = 'E';
  write_size = write(fd_, &c, 1);
#else
  _trace_printf(message, 1);
  write_size = strlen(message);
#endif
  return write_size;
}

} // namespace base
