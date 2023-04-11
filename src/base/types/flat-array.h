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

#ifndef BASE_TYPES_FLAT_ARRAY_H_
#define BASE_TYPES_FLAT_ARRAY_H_

#include <stdlib.h>

#include <type_traits>

#include "conf/config.h"
#include "log/log.h"
#include "log/tracer.h"
#include "utils/util.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "base::FlatArray-"

namespace base {
/**
 * Class FlatArray used to maintain array of trivial Objects(flatable)
 * for high performance requirenments.
 * The Objects in FlatArray never be initialized by default,
 * thus, the objects need to be set before get them.
 */
template <class T> class FlatArray {
public:
  typedef T value_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef size_t size_type;

  template <typename = typename std::enable_if<std::is_trivial<T>::value>::type>
  explicit FlatArray(size_t size) {
    LOG_D("try to alloc %u elements(%u bytes)", size, size * sizeof(T));
    X_AUTO_TRACE("FlatArray(%u)", size);
    contents_ = static_cast<T *>(malloc(size * sizeof(T)));
    size_ = (contents_ == nullptr) ? 0 : size;
  }

  template <typename = typename std::enable_if<std::is_trivial<T>::value>::type>
  explicit FlatArray(size_t size, int ch) {
    LOG_D("try to alloc %u elements(%u bytes) with init value %u", size,
          size * sizeof(T), ch);
    X_AUTO_TRACE("FlatArray(%u, 0x%x)", size, ch);
    contents_ = static_cast<T *>(calloc(size, sizeof(T)));
    size_ = (contents_ == nullptr) ? 0 : size;
    if (contents_ && (0 != ch)) {
      memset(contents_, ch, size * sizeof(T));
    }
  }

  FlatArray(const FlatArray &rhs) {
    LOG_D("copy constructor try to alloc %u elements(%u bytes)", rhs.size_,
          rhs.size_ * sizeof(T));
    X_AUTO_TRACE("FlatArray(RHS{%u})", rhs.size_);
    contents_ = static_cast<T *>(malloc(rhs.size_ * sizeof(T)));
    size_ = (contents_ == nullptr) ? 0 : rhs.size_;
    if (X_LIKELY(contents_ && rhs.contents_)) {
      memcpy(contents_, rhs.contents_, size_ * sizeof(T));
    }
  }

  FlatArray &operator=(const FlatArray &rhs) {
    LOG_D("operator= try to copy %u elements(%u bytes)", rhs.size_,
          rhs.size_ * sizeof(T));
    X_AUTO_TRACE("FlatArray=(RHS{%u})", rhs.size_);
    if (this == &rhs) {
      return *this;
    }
    clear();
    contents_ = static_cast<T *>(malloc(rhs.size_ * sizeof(T)));
    size_ = (contents_ == nullptr) ? 0 : rhs.size_;
    if (X_LIKELY(contents_ && rhs.contents_)) {
      memcpy(contents_, rhs.contents_, size_ * sizeof(T));
    }
    return *this;
  }

  ~FlatArray() { clear(); }

  ALWAYS_INLINE void resize(size_t size) {
    LOG_D("resize %u elements(%u bytes)", size, size * sizeof(T));
    X_AUTO_TRACE("%s %u elements (%u bytes)", __func__, size, size * sizeof(T));
    if (X_UNLIKELY(size == 0)) {
      clear();
      return;
    }
    // alloc new memory for content data
    T *new_contents_ = static_cast<T *>(realloc((contents_), size * sizeof(T)));
    if (X_UNLIKELY(!new_contents_)) {
      LOG_E("resize alloc %u elements(%u bytes) failed", size,
            size * sizeof(T));
      return;
    }
    contents_ = new_contents_;
    if (X_LIKELY(size_ < size)) {
      // memset the expanded data buffer
      memset(contents_ + size_, 0, (size - size_) * sizeof(T));
    }
    size_ = size;
  }

  ALWAYS_INLINE reference operator[](size_type index) {
    if (X_LIKELY(index < size_)) {
      return contents_[index];
    }
    // FIXME(zhongxiao.yzx): try to terminate while overflow
    // assert(index < size_);
    // should never be here
    LOG_E("[%d] out of bound(%d)", index, size_);
    return end_;
  }

  ALWAYS_INLINE reference at(size_type index) {
    if (X_LIKELY(index < size_)) {
      return contents_[index];
    }
    // FIXME(zhongxiao.yzx): try to terminate while overflow
    // assert(index < size_);
    // should never be here
    LOG_E("[%d] out of bound(%d)", index, size_);
    return end_;
  }

  ALWAYS_INLINE size_type size() const { return size_; }

  ALWAYS_INLINE bool empty() const { return size_ == 0; }

  ALWAYS_INLINE void clear() {
    if (X_LIKELY(contents_)) {
      free(contents_);
      contents_ = nullptr;
      size_ = 0;
    }
  }

private:
  T *contents_ = nullptr;
  size_type size_ = 0;
  T end_;

  // FlatArray& operator=(const FlatArray&) = delete;
  FlatArray(FlatArray &&) = delete;
  FlatArray &operator=(FlatArray &&) = delete;
};

} // namespace base
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#endif // BASE_TYPES_FLAT_ARRAY_H_
