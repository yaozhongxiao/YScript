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

#include "stream/file-stream.h"

namespace base {

FileStream::FileStream(std::string_view filename, Stream *log_stream)
    : Stream(log_stream), file_(nullptr), offset_(0), should_close_(false) {
  std::string filename_str = filename.to_string();
  file_ = fopen(filename_str.c_str(), "wb");

  // TODO(zhongxiao.yzx) : this is pretty cheesy, should come up with a better
  // API.
  if (file_) {
    should_close_ = true;
  } else {
    ERROR("fopen name=\"%s\" failed, errno=%d\n", filename_str.c_str(), errno);
  }
}

FileStream::FileStream(FILE *file, Stream *log_stream)
    : Stream(log_stream), file_(file), offset_(0), should_close_(false) {}

FileStream::FileStream(FileStream &&other) { *this = std::move(other); }

FileStream &FileStream::operator=(FileStream &&other) {
  file_ = other.file_;
  offset_ = other.offset_;
  should_close_ = other.should_close_;
  other.file_ = nullptr;
  other.offset_ = 0;
  other.should_close_ = false;
  return *this;
}

FileStream::~FileStream() {
  // We don't want to close existing files (stdout/sterr, for example).
  if (should_close_) {
    fclose(file_);
  }
}

Result FileStream::WriteDataImpl(size_t at, const void *data, size_t size) {
  if (!file_) {
    return Result::Error;
  }
  if (size == 0) {
    return Result::Ok;
  }
  if (at != offset_) {
    if (fseek(file_, at, SEEK_SET) != 0) {
      ERROR("fseek offset=%" PRIzd " failed, errno=%d\n", size, errno);
      return Result::Error;
    }
    offset_ = at;
  }
  if (fwrite(data, size, 1, file_) != 1) {
    ERROR("fwrite size=%" PRIzd " failed, errno=%d\n", size, errno);
    return Result::Error;
  }
  offset_ += size;
  return Result::Ok;
}

Result FileStream::MoveDataImpl(size_t dst_offset, size_t src_offset,
                                size_t size) {
  if (!file_) {
    return Result::Error;
  }
  if (size == 0) {
    return Result::Ok;
  }
  // TODO(zhongxiao.yzx) : implement if needed.
  ERROR0("FileWriter::MoveData not implemented!\n");
  return Result::Error;
}

// static
std::unique_ptr<FileStream> FileStream::CreateStdout() {
  // initialize stdio on-command
  InitStdio();
  return std::unique_ptr<FileStream>(new FileStream(stdout));
}

// static
std::unique_ptr<FileStream> FileStream::CreateStderr() {
  // initialize stdio on-command
  InitStdio();
  return std::unique_ptr<FileStream>(new FileStream(stderr));
}

} // namespace base
