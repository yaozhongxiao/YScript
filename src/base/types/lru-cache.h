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

#ifndef BASE_TYPES_LRU_CACHE_H_
#define BASE_TYPES_LRU_CACHE_H_

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "log/log.h"
#include "utils/util.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "base::LRUCache-"
namespace base {
/**
 *  LRUCache holds the pair of <KEY, VALUE> in LRU caching strategy
 *  It's not thread-safe structure
 */
template <typename KEY, typename VALUE> class LRUCache {
public:
  using key_type = KEY;
  using value_type = VALUE;
  using item_type = std::pair<key_type, value_type>;
  using indexing_type = typename std::list<item_type>::iterator;

  explicit LRUCache(uint64_t max_size = 50)
      : max_size_(max_size), cur_size_(0) {}

  virtual ~LRUCache() { Clear(); }

  void Put(const key_type &key, const value_type &value) {
    // replace old item with new one
    items_list_.emplace_front(key, value);
    auto it = items_indexing_map_.find(key);
    if (it != items_indexing_map_.end()) {
      cur_size_ -= 1;
      items_list_.erase(it->second);
      items_indexing_map_.erase(it);
    }
    items_indexing_map_[key] = items_list_.begin();
    cur_size_ += 1;

    if (cur_size_ > max_size_) {
      indexing_type last = items_list_.end();
      last--;
      cur_size_ -= 1;
      items_indexing_map_.erase(last->first);
      items_list_.pop_back();
    }
  }

  bool Get(const key_type &key, value_type &val /*out*/) {
    auto it = items_indexing_map_.find(key);
    if (it != items_indexing_map_.end()) {
      if (it->second != items_list_.begin()) {
        items_list_.splice(items_list_.begin(), items_list_, it->second);
      }
      val = it->second->second;
      return true;
    }
    return false;
  }

  void Erase(const key_type &key) {
    auto it = items_indexing_map_.find(key);
    if (it != items_indexing_map_.end()) {
      cur_size_ -= 1;
      items_list_.erase(it->second);
      items_indexing_map_.erase(it);
    }
  }

  bool Contain(const key_type &key) {
    return items_indexing_map_.find(key) != items_indexing_map_.end();
  }

  size_t Size() { return items_indexing_map_.size(); }

  void Clear() {
    items_list_.clear();
    items_indexing_map_.clear();
  }

  size_t MaxSize() { return max_size_; }

  size_t SetMaxSize(size_t max_size) {
    if (max_size_ < max_size) {
      Clear();
    }
    max_size_ = max_size;
    return max_size_;
  }

  template <typename KEY_TYPE = std::string,
            typename std::enable_if<
                (std::is_same<KEY_TYPE, std::string>::value), int>::type = 0>
  void Dump() {
    uint32_t index = 1;
    LOG_D("%d items in LRUCache(max-size %d) with { code-hash, context, "
          "version }",
          items_list_.size(), max_size_);
    for (const auto &item : items_list_) {
      LOG_D("[%d] : { %s , %p , %u }\n", index++, item.first.c_str(),
            item.second.get(), item.second->Version());
    }
  }

  template <typename KEY_TYPE,
            typename std::enable_if<
                !(std::is_same<KEY_TYPE, std::string>::value), int>::type = 0>
  void Dump() {
    uint32_t index = 1;
    LOG_D("%d items in LRUCache(max-size %d) with { code-hash, context, "
          "version }",
          items_list_.size(), max_size_);
    for (const auto &item : items_list_) {
      LOG_D("[%d] : { %s , %p , %u }\n", index++, item.first.abridged().c_str(),
            item.second.get(), item.second->Version());
    }
  }

private:
  size_t max_size_;
  size_t cur_size_;
  std::list<item_type> items_list_;
  std::unordered_map<key_type, indexing_type> items_indexing_map_;

  DISALLOW_COPY_AND_ASSIGN(LRUCache)
};

} // namespace base
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#endif // BASE_TYPES_LRU_CACHE_H_
