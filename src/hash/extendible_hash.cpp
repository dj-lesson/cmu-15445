#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"

namespace cmudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) {
  global_depth = 1;
  bucket_size = size;
  bucket_num = 2;
  dict.push_back(new ExtendibleBucket<K, V>(1));
  dict.push_back(new ExtendibleBucket<K, V>(1));
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
  return hasher(key);
}

/**
 * AnDJ(Junduo Dong)
 * get dict key
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::getDictKey(K key){
  return HashKey(key) & ((1 << global_depth) - 1);
}
/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  return global_depth;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  assert(bucket_id < bucket_num);
  return (dict[bucket_id])->getLocalDepth();
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  return bucket_num;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  int dict_key = getDictKey(key);
  if (dict_key >= dict.size()) {
    return false;
  }
  ExtendibleBucket<K, V>* bucket = dict[dict_key];
  typename std::map<K, V>::iterator res = bucket->getKvs().find(key);
  if (res != bucket->getKvs().end()){
    value = res->second;
    return true;
  }else{
    return false;
  }
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  int dict_key = getDictKey(key);
  if (dict_key >= dict.size()) {
    return true;
  }
  ExtendibleBucket<K, V>* bucket = dict[dict_key];
  return bucket->getKvs().erase(key);
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  ExtendibleBucket<K,V>* bucket = dict[getDictKey(key)];
  bucket->getKvs().insert(std::make_pair(key, value));
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
