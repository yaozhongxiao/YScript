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

#ifndef BASE_TRACE_MARKER_H_
#define BASE_TRACE_MARKER_H_

#include <assert.h>
#include <sys/types.h>

namespace base {

class TraceMarker {
public:
  TraceMarker();
  virtual ~TraceMarker();

  ssize_t WriteBegin(const char *message);
  ssize_t WriteEnd(const char *message);
  int GetFd() { return fd_; }

private:
  int fd_;
};

} // namespace base
#endif // BASE_TRACE_MARKER_H_
