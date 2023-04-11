/** / common/config.h
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

#ifndef BASE_STREAM_FILESTREAM_H_
#define BASE_STREAM_FILESTREAM_H_

#include <cassert>
#include <memory>
#include <vector>

// #include "common/common/config.h"
#include "stream/stream.h"

namespace base {

class FileStream : public Stream {
public:
  explicit FileStream(std::string_view filename, Stream *log_stream = nullptr);
  explicit FileStream(FILE *, Stream *log_stream = nullptr);
  FileStream(FileStream &&);
  FileStream &operator=(FileStream &&);
  ~FileStream();

  static std::unique_ptr<FileStream> CreateStdout();
  static std::unique_ptr<FileStream> CreateStderr();

  bool is_open() const { return file_ != nullptr; }

protected:
  Result WriteDataImpl(size_t offset, const void *data, size_t size) override;
  Result MoveDataImpl(size_t dst_offset, size_t src_offset,
                      size_t size) override;

private:
  FILE *file_;
  size_t offset_;
  bool should_close_;

  FileStream(const FileStream &) = delete;
  void operator=(const FileStream &) = delete;
};

} // namespace base
#endif // BASE_STREAM_FILESTREAM_H_
