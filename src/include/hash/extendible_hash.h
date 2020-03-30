/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <assert.h>

#include "hash/hash_table.h"

namespace cmudb {
template <typename K, typename V>
class ExtendibleBucket{
public:
  ExtendibleBucket(int local_depth_i): local_depth(local_depth_i){
  }
  ~ExtendibleBucket(){
  }
  int getLocalDepth(){
    return local_depth;
  }
  std::map<K, V>& getKvs(){
    return kvs;
  }
  void setLocalDepth(int local_depth_){
    local_depth = local_depth_;
  }
private:
  int local_depth;
  std::map<K, V> kvs;
};

template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {
public:
  // constructor
  ExtendibleHash(size_t size);
  // helper function to generate hash addressing
  size_t HashKey(const K &key);
  // helper function to get global & local depth
  int GetGlobalDepth() const;
  int GetLocalDepth(int bucket_id) const;
  int GetNumBuckets() const;
  // lookup and modifier
  bool Find(const K &key, V &value) override;
  bool Remove(const K &key) override;
  void Insert(const K &key, const V &value) override;

private:
  // add your own member variables here
  std::hash<K> hasher;
  std::vector<ExtendibleBucket<K, V>*> dict;
  size_t global_depth;
  size_t bucket_size;
  int bucket_num;
  int getDictKey(K key);
};
} // namespace cmudb
