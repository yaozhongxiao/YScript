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

#ifndef BASE_THREADS_SPINLOCK_H_
#define BASE_THREADS_SPINLOCK_H_

#include <atomic>
#include <mutex>  // NOLINT
#include <thread> // NOLINT

#include "utils/util.h"

namespace base {
/**
 * This class meet the BasicLockable requirements (lock, unlock )
 */
class SpinLock {
public:
  SpinLock() noexcept {}
  ~SpinLock() {}

  ALWAYS_INLINE void lock() noexcept {
    while (flags_.test_and_set(std::memory_order_acquire)) {
#ifndef PLUGINS_SANDBOX
      // yield have not supported yet in TEE
      std::this_thread::yield();
#endif
    };
  };

  ALWAYS_INLINE void unlock() noexcept {
    flags_.clear(std::memory_order_release);
  };

  ALWAYS_INLINE bool try_lock() noexcept {
    return !flags_.test_and_set(std::memory_order_acquire);
  };

private:
  std::atomic_flag flags_ = ATOMIC_FLAG_INIT;
};

} // namespace base
#endif // BASE_THREADS_SPINLOCK_H_
