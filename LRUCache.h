#ifndef LRU_H
#define LRU_H

#include <deque>
#include <mutex>
#include <unordered_map>

#include "Request.h"
#include "Response.h"
class LRUCache {
 public:
  int cacheSize;
  std::unordered_map<Request, Response, RequestHashFunction> cache;
  std::deque<Request> req;
  std::mutex mtx;

  LRUCache(int capacity) {
    cache.clear();
    req.clear();
    cacheSize = capacity;
  }

  Response * get(Request key) {
    // not found
    if (cache.find(key) == cache.end()) {
      return NULL;
    }
    else {
      // found
      std::deque<Request>::iterator it = req.begin();
      //needs to overload != operator on request
      while (*it != key) {
        it++;
      }

      // update queue: update it to most recent used value
      req.erase(it);
      req.push_front(key);
      return &(cache[key]);
    }
  }

  void put(Request key, Response value) {
    // not present in cache
    if (cache.find(key) == cache.end()) {
      // check if cache is full
      if (cacheSize == req.size()) {
        Request last = req.back();
        req.pop_back();
        cache.erase(last);
      }
    }
    else {
      // present in cache, remove it from queue and unordered_map
      std::deque<Request>::iterator it = req.begin();
      while (*it != key) {
        it++;
      }

      req.erase(it);
      cache.erase(key);
    }

    // update the cache
    req.push_front(key);
    cache[key] = value;
  }
};

#endif