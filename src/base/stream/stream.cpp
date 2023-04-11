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

#include "stream/stream.h"

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdarg>

#define DUMP_OCTETS_PER_LINE 16
#define DUMP_OCTETS_PER_GROUP 2
#define STREAM_LOGGER_BUFFER_SIZE 512

namespace base {

Stream::Stream(Stream *log_stream)
    : offset_(0), result_(Result::Ok), log_stream_(log_stream) {}

void Stream::AddOffset(size_t delta) { offset_ += delta; }

void Stream::WriteDataAt(size_t at, const void *src, size_t size,
                         const char *desc, PrintChars print_chars) {
  if (Failed(result_)) {
    return;
  }
  if (log_stream_) {
    log_stream_->WriteMemoryDump(src, size, at, print_chars, nullptr, desc);
  }
  result_ = WriteDataImpl(at, src, size);
}

void Stream::WriteData(const void *src, size_t size, const char *desc,
                       PrintChars print_chars) {
  WriteDataAt(offset_, src, size, desc, print_chars);
  offset_ += size;
}

void Stream::MoveData(size_t dst_offset, size_t src_offset, size_t size) {
  if (Failed(result_)) {
    return;
  }
  if (log_stream_) {
    log_stream_->Writef(
        "; move data: [%" PRIzx ", %" PRIzx ") -> [%" PRIzx ", %" PRIzx ")\n",
        src_offset, src_offset + size, dst_offset, dst_offset + size);
  }
  result_ = MoveDataImpl(dst_offset, src_offset, size);
}

void Stream::Writef(const char *format, ...) {
  char buffer[STREAM_LOGGER_BUFFER_SIZE] = {0};
  va_list ap;
  va_start(ap, format);
  uint32_t length = vsnprintf(buffer, STREAM_LOGGER_BUFFER_SIZE, format, ap);
  va_end(ap);
  WriteData(buffer, length);
}

void Stream::WriteMemoryDump(const void *start, size_t size, size_t offset,
                             PrintChars print_chars, const char *prefix,
                             const char *desc) {
  const uint8_t *p = static_cast<const uint8_t *>(start);
  const uint8_t *end = p + size;
  while (p < end) {
    const uint8_t *line = p;
    const uint8_t *line_end = p + DUMP_OCTETS_PER_LINE;
    if (prefix) {
      Writef("%s", prefix);
    }
    Writef("%07" PRIzx ": ", reinterpret_cast<intptr_t>(p) -
                                 reinterpret_cast<intptr_t>(start) + offset);
    while (p < line_end) {
      for (int i = 0; i < DUMP_OCTETS_PER_GROUP; ++i, ++p) {
        if (p < end) {
          Writef("%02x", *p);
        } else {
          WriteChar(' ');
          WriteChar(' ');
        }
      }
      WriteChar(' ');
    }

    if (print_chars == PrintChars::Yes) {
      WriteChar(' ');
      p = line;
      for (int i = 0; i < DUMP_OCTETS_PER_LINE && p < end; ++i, ++p)
        WriteChar(isprint(*p) ? *p : '.');
    }

    /* if there are multiple lines, only print the desc on the last one */
    if (p >= end && desc) {
      Writef("  ; %s", desc);
    }
    WriteChar('\n');
  }
}

Result OutputBuffer::WriteToFile(std::string_view filename) const {
#ifndef PLUGIN_SANDBOX
  std::string filename_str = filename.to_string();
  FILE *file = fopen(filename_str.c_str(), "wb");
  if (!file) {
    ERROR("unable to open %s for writing\n", filename_str.c_str());
    return Result::Error;
  }

  if (data.empty()) {
    fclose(file);
    return Result::Ok;
  }

  size_t bytes = fwrite(data.data(), 1, data.size(), file);
  if (bytes <= 0 || static_cast<size_t>(bytes) != data.size()) {
    ERROR("failed to write %" PRIzd " bytes to %s\n", data.size(),
          filename_str.c_str());
    fclose(file);
    return Result::Error;
  }

  fclose(file);
  return Result::Ok;
#endif
  return Result::Error;
}

MemoryStream::MemoryStream(Stream *log_stream)
    : Stream(log_stream), buf_(new OutputBuffer()) {}

MemoryStream::MemoryStream(std::unique_ptr<OutputBuffer> &&buf,
                           Stream *log_stream)
    : Stream(log_stream), buf_(std::move(buf)) {}

std::unique_ptr<OutputBuffer> MemoryStream::ReleaseOutputBuffer() {
  return std::move(buf_);
}

void MemoryStream::Clear() {
  if (buf_)
    buf_->clear();
  else
    buf_.reset(new OutputBuffer());
}

Result MemoryStream::WriteDataImpl(size_t dst_offset, const void *src,
                                   size_t size) {
  if (size == 0) {
    return Result::Ok;
  }
  size_t end = dst_offset + size;
  if (end > buf_->data.size()) {
    buf_->data.resize(end);
  }
  uint8_t *dst = &buf_->data[dst_offset];
  memcpy(dst, src, size);
  return Result::Ok;
}

Result MemoryStream::MoveDataImpl(size_t dst_offset, size_t src_offset,
                                  size_t size) {
  if (size == 0) {
    return Result::Ok;
  }
  size_t src_end = src_offset + size;
  size_t dst_end = dst_offset + size;
  size_t end = src_end > dst_end ? src_end : dst_end;
  if (end > buf_->data.size()) {
    buf_->data.resize(end);
  }

  uint8_t *dst = &buf_->data[dst_offset];
  uint8_t *src = &buf_->data[src_offset];
  memmove(dst, src, size);
  return Result::Ok;
}

void InitStdio() {
#if COMPILER_IS_MSVC
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  int result = _setmode(_fileno(stdout), _O_BINARY);
  if (result == -1) {
    perror("Cannot set mode binary to stdout");
  }
  result = _setmode(_fileno(stderr), _O_BINARY);
  if (result == -1) {
    perror("Cannot set mode binary to stderr");
  }
#endif
}

} // namespace base
