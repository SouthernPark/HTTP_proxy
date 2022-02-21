#include "LRUCache.h"
#include "ListenerSock.h"
#include "Proxy.h"
#include "logger.h"

int main() {
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();
  LRUCache cache(LRUCACHE_SIZE);
  logger req_log;

  std::mutex request_id_lock;
  long request_id = 0;
  int count = 30;
  while (count != 0) {
    Proxy proxy{};  //create a proxy

    //listener will accept a client connect
    //and set the connecting sockfd in client
    listener.accept_(proxy.client);  //throw listener exception

    //receive a request from the client
    proxy.handleRequest(cache, req_log, request_id);

    auto it = proxy.resp.cache_control_kvs.begin();

    count--;
    std::cout << "=======================" << count << std::endl;
    request_id_lock.lock();
    request_id++;
    request_id_lock.unlock();
  }
}
