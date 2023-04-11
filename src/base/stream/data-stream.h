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
#ifndef BASE_STREAM_DATASTREAM_H_
#define BASE_STREAM_DATASTREAM_H_

#include <assert.h>
#include <string.h>

#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "log/log.h"
#include "types/types.h"
#include "types/varint.h"
#include "types/varuint.h"

#ifndef BOOL_ASSERT
#define BOOL_ASSERT(condition, err, msg)                                       \
  if (!(condition)) {                                                          \
    set_errno(err);                                                            \
    LOG_E("%s\n", (msg));                                                      \
    return false;                                                              \
  }
#endif

#ifndef STREAM_ASSERT
#define STREAM_ASSERT(condition, err, msg)                                     \
  if (!(condition)) {                                                          \
    ds.set_errno(err);                                                         \
    LOG_E("%s\n", (msg));                                                      \
    return ds;                                                                 \
  }
#endif

namespace base {

namespace datastream {
enum ErrorCode : uint32_t {
  SUCCESS = 0,
  TRAP_OUT_OF_STREAM = 1,
  TRAP_OUT_OF_CAPACITY,
  TRAP_LENGTH_MISMATCH,
  TRAP_UNKNOWN_ERROR
};

// FIXME(zhongxiao.yzx) : only take non-nested container into consideration
constexpr static uint32_t kMaxMemorySize = 1024 * 1024;
constexpr static uint32_t kMaxElementNum = 1024 * 1024;
} // namespace datastream

/**
 *  data stream for reading and writing data in the form of bytes
 *
 *  @brief %A data stream for reading and writing data in the form of bytes.
 *  @param T - Type of the DataStream buffer
 */
template <typename T> class DataStream {
public:
  /**
   * Construct a new DataStream object given the size of the buffer and start
   * position of the buffer
   *
   * @brief Construct a new DataStream object
   * @param start - The start position of the buffer
   * @param s - The size of the buffer
   */
  DataStream(T start, size_t s)
      : start_(start), pos_(start), end_(start + s),
        errno_(datastream::SUCCESS) {}

  /**
   *  Skips a specified number of bytes from this stream
   *
   *  @brief Skips a specific number of bytes from this stream
   *  @param s - The number of bytes to skip
   */
  inline void skip(size_t s) { pos_ += s; }

  /**
   *  Reads a specified number of bytes from the stream into a buffer
   *
   *  @brief Reads a specified number of bytes from this stream into a buffer
   *  @param d - The pointer to the destination buffer
   *  @param s - the number of bytes to read
   *  @return true
   */
  inline bool read(char *d, size_t s) {
    BOOL_ASSERT(static_cast<size_t>(end_ - pos_) >= static_cast<size_t>(s),
                base::datastream::TRAP_OUT_OF_STREAM, "read");
    memcpy(d, pos_, s);
    pos_ += s;
    return true;
  }

  /**
   *  Writes a specified number of bytes into the stream from a buffer
   *
   *  @brief Writes a specified number of bytes into the stream from a buffer
   *  @param d - The pointer to the source buffer
   *  @param s - The number of bytes to write
   *  @return true
   */
  inline bool write(const char *d, size_t s) {
    BOOL_ASSERT(end_ - pos_ >= static_cast<int32_t>(s),
                base::datastream::TRAP_OUT_OF_STREAM, "write");
    memcpy(rebaseret_cast<void *>(pos_), d, s);
    pos_ += s;
    return true;
  }

  /**
   *  Writes a byte into the stream
   *
   *  @brief Writes a byte into the stream
   *  @param c byte to write
   *  @return true
   */
  inline bool put(char c) {
    BOOL_ASSERT(pos_ < end_, base::datastream::TRAP_OUT_OF_STREAM, "put");
    *pos_ = c;
    ++pos_;
    return true;
  }

  /**
   *  Reads a byte from the stream
   *
   *  @brief Reads a byte from the stream
   *  @param c - The reference to destination byte
   *  @return true
   */
  inline bool get(unsigned char &c) {
    return get(*(rebaseret_cast<char *>(&c)));
  }

  /**
   *  Reads a byte from the stream
   *
   *  @brief Reads a byte from the stream
   *  @param c - The reference to destination byte
   *  @return true
   */
  inline bool get(char &c) {
    BOOL_ASSERT(pos_ < end_, base::datastream::TRAP_OUT_OF_STREAM, "get");
    c = *pos_;
    ++pos_;
    return true;
  }

  /**
   *  Retrieves the current position of the stream
   *
   *  @brief Retrieves the current position of the stream
   *  @return T - The current position of the stream
   */
  T pos() const { return pos_; }

  /**
   *  Check validity
   *
   *  @return true if pos_ is within the range
   *  @return false if pos_ is not within the range
   */
  inline bool valid() const { return pos_ <= end_ && pos_ >= start_; }

  /**
   *  Sets the position within the current stream
   *
   *  @brief Sets the position within the current stream
   *  @param p - The offset relative to the origin
   *  @return true if p is within the range
   *  @return false if p is not within the range
   */
  inline bool seekp(size_t p) {
    pos_ = start_ + p;
    return pos_ <= end_;
  }

  /**
   *  Gets the position within the current stream
   *
   *  @brief Gets the position within the current stream
   *  @return p - The position within the current stream
   */
  inline size_t tellp() const { return size_t(pos_ - start_); }

  /**
   *  @brief Returns the number of remaining bytes that can be read/skipped
   *  @return size_t - The number of remaining bytes
   */
  inline size_t remaining() const { return end_ - pos_; }

  /**
   *  @brief Set the error no for operations on DataStream
   *  @return error_no - the error no set for operations on DataStream
   */
  void set_errno(const base::datastream::ErrorCode &error_no) {
    errno_ = error_no;
  }

  /**
   *  @brief Returns the error no for operations on DataStream
   *  @return ErrorCode - Returns the error no for operations on DataStream
   */
  base::datastream::ErrorCode get_errno() { return errno_; }

private:
  /**
   * The start position of the buffer
   * @brief The start position of the buffer
   */
  T start_;
  /**
   * The current position of the buffer
   * @brief The current position of the buffer
   */
  T pos_;
  /**
   * The end position of the buffer
   * @brief The end position of the buffer
   */
  T end_;
  /**
   * The error number for operations on datastream
   * @brief The error number for operations on datastream
   */
  base::datastream::ErrorCode errno_;
};

/**
 * @brief Specialization of DataStream used to help determine the final size of
 * a serialized value. Specialization of DataStream used to help determine the
 * final size of a serialized value
 */
template <> class DataStream<size_t> {
public:
  /**
   * Construct a new specialized DataStream object given the initial size
   *
   * @brief Construct a new specialized DataStream object
   * @param init_size - The initial size
   */
  DataStream(size_t init_size = 0) : size_(init_size) {}

  /**
   *  Increment the size by s. This behaves the same as write( const char*
   * ,size_t s ).
   *
   *  @brief Increase the size by s
   *  @param s - The amount of size to increase
   *  @return true
   */
  inline bool skip(size_t s) {
    size_ += s;
    return true;
  }

  /**
   *  Increment the size by s. This behaves the same as skip( size_t s )
   *
   *  @brief Increase the size by s
   *  @param s - The amount of size to increase
   *  @return true
   */
  inline bool write(const char *, size_t s) {
    size_ += s;
    return true;
  }

  /**
   *  Increment the size by one
   *
   *  @brief Increase the size by one
   *  @return true
   */
  inline bool put(char) {
    ++size_;
    return true;
  }

  /**
   *  Check validity. It's always valid
   *
   *  @brief Check validity
   *  @return true
   */
  inline bool valid() const { return true; }

  /**
   * Set new size
   *
   * @brief Set new size
   * @param p - The new size
   * @return true
   */
  inline bool seekp(size_t p) {
    size_ = p;
    return true;
  }

  /**
   * Get the size
   *
   * @brief Get the size
   * @return size_t - The size
   */
  inline size_t tellp() const { return size_; }

  /**
   * Always returns 0
   *
   * @brief Always returns 0
   * @return size_t - 0
   */
  inline size_t remaining() const { return 0; }

private:
  /**
   * The size used to determine the final size of a serialized value.
   *
   * @brief The size used to determine the final size of a serialized value.
   */
  size_t size_;
};

namespace datastream {
/**
 * Check if type T is a pointer
 *
 * @brief Check if type T is a pointer
 * @tparam T - The type to be checked
 * @return true if T is a pointer
 * @return false otherwise
 */
template <typename T> constexpr bool is_pointer() {
  return std::is_pointer<T>::value || base::is_null_pointer<T>::value ||
         std::is_member_pointer<T>::value;
}

/**
 * Check if type T is a primitive type
 *
 * @brief Check if type T is a primitive type
 * @tparam T - The type to be checked
 * @return true if T is a primitive type
 * @return false otherwise
 */
template <typename T> constexpr bool is_primitive() {
  return std::is_arithmetic<T>::value || std::is_enum<T>::value;
}

/**
 * Unpack data inside a fixed size buffer as T
 *
 * @brief Unpack data inside a fixed size buffer as T
 * @tparam T - Type of the unpacked data
 * @param buffer - Pointer to the buffer
 * @param len - Length of the buffer
 * @return T - The unpacked data
 */
template <typename T> T unpack(const uint8_t *buffer, size_t len) {
  T result;
  DataStream<const uint8_t *> ds(buffer, len);
  ds >> result;
  return result;
}

/**
 * Unpack data inside a variable size buffer as T
 *
 * @brief Unpack data inside a variable size buffer as T
 * @tparam T - Type of the unpacked data
 * @param bytes - Buffer
 * @return T - The unpacked data
 */
template <typename T> T unpack(const std::vector<uint8_t> &bytes) {
  return unpack<T>(bytes.data(), bytes.size());
}

/**
 * Get the size of the packed data
 *
 * @brief Get the size of the packed data
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return size_t - Size of the packed data
 */
template <typename T> size_t pack_size(const T &value) {
  DataStream<size_t> ps;
  ps << value;
  return ps.tellp();
}

/**
 * Get packed data in vector
 *
 * @brief Get packed data
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return bytes - The packed data
 */
template <typename T> std::vector<uint8_t> pack(const T &value) {
  std::vector<uint8_t> result;
  result.resize(pack_size(value));
  DataStream<uint8_t *> ds(result.data(), result.size());
  ds << value;
  return result;
}

/**
 * Get packed data in string
 *
 * @brief Get packed data
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @param result - The packed data
 * @return size - The packed data size
 */
template <typename T> size_t pack(const T &value, std::string &result /*out*/) {
  size_t size = pack_size(value);
  result.resize(size);
  if (size > 0) {
    DataStream<char *> ds(&(result[0]), size);
    ds << value;
  }
  return size;
}

} // namespace datastream

/**
 *  Serialize a bool into a stream
 *
 *  @brief  Serialize a bool into a stream
 *  @param ds - The stream to read
 *  @param d - The value to serialize
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
inline DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const bool &d) {
  return ds << uint8_t(d);
}

/**
 *  Deserialize a bool from a stream
 *
 *  @brief Deserialize a bool
 *  @param ds - The stream to read
 *  @param d - The destination for deserialized value
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
inline DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, bool &d) {
  uint8_t t = 0;
  ds >> t;
  d = t;
  return ds;
}

/**
 *  Serialize a string into a stream
 *
 *  @brief Serialize a string
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const std::string &v) {
  ds << VarUInt(v.size());
  if (v.size())
    ds.write(v.data(), v.size());
  return ds;
}

/**
 *  Deserialize a string from a stream
 *
 *  @brief Deserialize a string
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::string &v) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(s.value() <= datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "deserialize to vector out-of-max-size");
  if (s.value() > 0) {
    v.resize(s);
    ds.read(const_cast<char *>(v.data()), v.size());
  }
  return ds;
}

/**
 *  Serialize a fixed size array into a stream
 *
 *  @brief Serialize a fixed size array
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds,
                               const std::array<T, N> &v) {
  ds << VarUInt(N);
  for (const auto &i : v)
    ds << i;
  return ds;
}

/**
 *  Deserialize a fixed size array from a stream
 *
 *  @brief Deserialize a fixed size array
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::array<T, N> &v) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(N == s.value() && N < datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "std::array size and unpacked size don't match");
  for (auto &i : v)
    ds >> i;
  return ds;
}

/**
 *  Pointer should not be serialized, so this function will always throws an
 * error
 *
 *  @brief Deserialize a a pointer
 *  @param ds - The stream to read
 *  @tparam T - Type of the pointer
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 *  @post Throw an exception if it is a pointer
 */
template <typename Buffer, typename T,
          base::enable_if_t<datastream::is_pointer<T>()> * = nullptr>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, T) {
  static_assert(!datastream::is_pointer<T>(),
                "Pointers should not be serialized");
  return ds;
}

/**
 *  Serialize a fixed size array of non-primitive and non-pointer type
 *
 *  @brief Serialize a fixed size array of non-primitive and non-pointer type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam T - Type of the pointer
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N,
          base::enable_if_t<!datastream::is_primitive<T>() &&
                            !datastream::is_pointer<T>()> * = nullptr>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const T (&v)[N]) {
  ds << VarUInt(N);
  for (uint32_t i = 0; i < N; ++i)
    ds << v[i];
  return ds;
}

/**
 *  Serialize a fixed size array of non-primitive type
 *
 *  @brief Serialize a fixed size array of non-primitive type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam T - Type of the pointer
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N,
          base::enable_if_t<datastream::is_primitive<T>()> * = nullptr>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const T (&v)[N]) {
  ds << VarUInt(N);
  ds.write(static_cast<char *>(&v[0]), sizeof(v));
  return ds;
}

/**
 *  Deserialize a fixed size array of non-primitive and non-pointer type
 *
 *  @brief Deserialize a fixed size array of non-primitive and non-pointer type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N,
          base::enable_if_t<!datastream::is_primitive<T>() &&
                            !datastream::is_pointer<T>()> * = nullptr>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, T (&v)[N]) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(N == s.value() && N < datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "T[] size and unpacked size don't match");
  for (uint32_t i = 0; i < N; ++i)
    ds >> v[i];
  return ds;
}

/**
 *  Deserialize a fixed size array of non-primitive type
 *
 *  @brief Deserialize a fixed size array of non-primitive type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T, std::size_t N,
          base::enable_if_t<datastream::is_primitive<T>()> * = nullptr>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, T (&v)[N]) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(N == s.value() && N < datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "T[] size and unpacked size don't match");
  ds.read(static_cast<char *>(&v[0]), sizeof(v));
  return ds;
}

/**
 *  Serialize a vector of char
 *
 *  @brief Serialize a vector of char
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds,
                               const std::vector<char> &v) {
  ds << VarUInt(v.size());
  ds.write(v.data(), v.size());
  return ds;
}

/**
 *  Serialize a vector
 *
 *  @brief Serialize a vector
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam T - Type of the object contained in the vector
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds,
                               const std::vector<T> &v) {
  ds << VarUInt(v.size());
  for (const auto &i : v)
    ds << i;
  return ds;
}

/**
 *  Deserialize a vector of char
 *
 *  @brief Deserialize a vector of char
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::vector<char> &v) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(s.value() <= datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "deserialize to vector out-of-max-size");
  if (s.value() > 0) {
    v.resize(s);
    ds.read(v.data(), v.size());
  }
  return ds;
}

/**
 *  Deserialize a vector
 *
 *  @brief Deserialize a vector
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the vector
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::vector<T> &v) {
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(s.value() <= datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "deserialize to vector out-of-max-size");
  v.resize(s);
  for (auto &i : v)
    ds >> i;
  return ds;
}

template <typename Buffer, typename T>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const std::set<T> &s) {
  ds << VarUInt(s.size());
  for (const auto &i : s) {
    ds << i;
  }
  return ds;
}

template <typename Buffer, typename T>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::set<T> &s) {
  s.clear();
  VarUInt sz;
  ds >> sz;
  STREAM_ASSERT(sz.value() <= datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "deserialize to set out-of-max-size");
  for (uint32_t i = 0; i < sz; ++i) {
    T v;
    ds >> v;
    s.emplace(std::move(v));
  }
  return ds;
}

/**
 *  Serialize a map
 *
 *  @brief Serialize a map
 *  @param ds - The stream to write
 *  @param m - The value to serialize
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename K, typename V>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds,
                               const std::map<K, V> &m) {
  ds << VarUInt(m.size());
  for (const auto &i : m) {
    ds << i.first << i.second;
  }
  return ds;
}

/**
 *  Deserialize a map
 *
 *  @brief Deserialize a map
 *  @param ds - The stream to read
 *  @param m - The destination for deserialized value
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename K, typename V>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, std::map<K, V> &m) {
  m.clear();
  VarUInt s;
  ds >> s;
  STREAM_ASSERT(s.value() <= datastream::kMaxElementNum,
                base::datastream::TRAP_OUT_OF_CAPACITY,
                "deserialize to set out-of-max-size");
  for (uint32_t i = 0; i < s; ++i) {
    K k;
    V v;
    ds >> k >> v;
    m.emplace(std::move(k), std::move(v));
  }
  return ds;
}

/**
 *  Serialize a primitive type
 *
 *  @brief Serialize a primitive type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam T - Type of the primitive type
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T,
          base::enable_if_t<datastream::is_primitive<T>()> * = nullptr>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const T &v) {
  ds.write(rebaseret_cast<const char *>(&v), sizeof(T));
  return ds;
}

/**
 *  Deserialize a primitive type
 *
 *  @brief Deserialize a primitive type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the primitive type
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream<Buffer>& - Reference to the DataStream
 */
template <typename Buffer, typename T,
          base::enable_if_t<datastream::is_primitive<T>()> * = nullptr>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, T &v) {
  ds.read(rebaseret_cast<char *>(&v), sizeof(T));
  return ds;
}

/**
 *  Serialize an VarUInt object with as few bytes as possible
 *
 *  This provides more efficient serialization of 32-bit unsigned int.
 *  It serialuzes a 32-bit unsigned integer in as few bytes as possible.
 *  following [LEB128](https://en.wikipedia.org/wiki/LEB128)
 *
 *  @brief Serialize an VarUInt object with as few bytes as possible
 *  @param ds - The stream to write
 *  @param var - The value to serialize
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const VarUInt &var) {
  uint64_t val = var.value();
  do {
    uint8_t b = uint8_t(val) & 0x7f;
    val >>= 7;
    b |= ((val > 0) << 7);
    ds.write(rebaseret_cast<char *>(&b), 1); //.put(b);
  } while (val);
  return ds;
}

/**
 *  Deserialize an VarUInt object
 *
 *  This provides more efficient deserialization of 32-bit unsigned int.
 *  following [LEB128](https://en.wikipedia.org/wiki/LEB128)
 *
 *  @brief Deserialize an VarUInt object
 *  @param ds - The stream to read
 *  @param var - The destination for deserialized value
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, VarUInt &var) {
  uint64_t v = 0;
  char b = 0;
  uint8_t by = 0;
  do {
    STREAM_ASSERT(ds.get(b), base::datastream::TRAP_OUT_OF_STREAM,
                  "out of stream");
    v |= uint32_t(uint8_t(b) & 0x7f) << by;
    by += 7;
  } while (uint8_t(b) & 0x80);
  var = static_cast<uint32_t>(v);
  return ds;
}

/**
 *  Serialize an VarInt object with as few bytes as possible
 *
 *  This provides more efficient serialization of 32-bit signed int.
 *  It serialuzes a 32-bit signed integer in as few bytes as possible.
 *  following [LEB128](https://en.wikipedia.org/wiki/LEB128)
 *
 *  @brief Serialize an VarInt object with as few bytes as possible
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator<<(DataStream<Buffer> &ds, const VarInt &var) {
  bool more = true;
  int32_t val = var.value();
  do {
    uint8_t b = uint8_t(val) & 0x7f;
    val >>= 7;
    more = !(((val == 0) && ((b & 0x40) == 0)) ||
             ((val == -1) && ((b & 0x40) != 0)));
    b |= ((more) << 7);
    ds.write(rebaseret_cast<char *>(&b), 1); //.put(b);
  } while (more);
  return ds;
}

/**
 *  Deserialize an VarInt object
 *
 *  This provides more efficient deserialization of 32-bit signed int.
 *  following [LEB128](https://en.wikipedia.org/wiki/LEB128)
 *
 *  @brief Deserialize an VarInt object
 *  @param ds - The stream to read
 *  @param vi - The destination for deserialized value
 *  @tparam Buffer - Type of DataStream buffer
 *  @return DataStream& - Reference to the DataStream
 */
template <typename Buffer>
DataStream<Buffer> &operator>>(DataStream<Buffer> &ds, VarInt &var) {
  uint32_t v = 0;
  char b = 0;
  uint8_t by = 0;
  do {
    STREAM_ASSERT(ds.get(b), base::datastream::TRAP_OUT_OF_STREAM,
                  "out of stream");
    v |= uint32_t(uint8_t(b) & 0x7f) << by;
    by += 7;
  } while (uint8_t(b) & 0x80);
  // set the sign bit
  v |= (v >> (by - 1)) & 0x01 ? static_cast<uint32_t>(-1) << by : 0;
  var = static_cast<int32_t>(v);
  return ds;
}

} // namespace base
#endif // BASE_STREAM_DATASTREAM_H_
