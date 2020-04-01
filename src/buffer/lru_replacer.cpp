/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace cmudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
  head = nullptr;
  tail = nullptr;
}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  std::lock_guard<std::mutex> lock(latch);
  typename std::map<T, LruListNode<T>*>::iterator iter = lru_map.find(value);
  LruListNode<T>* node = nullptr;
  if (iter == lru_map.end()) {
    node = new LruListNode<T>(value);
    lru_map.insert(std::make_pair(value, node));
    if (head == nullptr){
      head = tail = node;
      return;
    }
    node->last = head;
    head->front = node;
    head = node;
  } else {
    node = iter->second;
    if (node == head) {
      return;
    }
    node->front->last = node->last;
    if (node == tail) {
      tail = node->front;
      tail -> last = nullptr;
    } else {
      node->last->front = node->front;
    }
    node->last = head;
    head->front = node;
    head = node;
  }
}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  std::lock_guard<std::mutex> lock(latch);
  if (lru_map.size() == 0) {
    return false;
  }
  value = tail->value;
  typename std::map<T, LruListNode<T>*>::iterator iter = lru_map.find(value);
  if (iter->second == head) {
    head = tail = nullptr;
  } else {
    iter->second->front->last = nullptr;
    tail = iter->second->front;
  }
  delete iter->second;
  lru_map.erase(iter);
  return true;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  std::lock_guard<std::mutex> lock(latch);
  typename std::map<T, LruListNode<T>*>::iterator iter = lru_map.find(value);
  if (iter == lru_map.end()) {
    return false;
  }
  LruListNode<T>* node = iter->second;
  if (node == head) {
    node->last->front = nullptr;
    head = node->last;
  } else if (node == tail) {
    node->front->last = nullptr;
    tail = node->front;
  } else {
    node->front->last = node->last;
    node->last->front = node->front;
  }
  delete iter->second;
  lru_map.erase(iter);
  return true;
}

template <typename T> size_t LRUReplacer<T>::Size() { 
  std::lock_guard<std::mutex> lock(latch);
  return lru_map.size();  
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb
