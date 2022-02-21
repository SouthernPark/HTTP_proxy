#include <thread>

#include "LRUCache.h"
#include "ListenerSock.h"
#include "Proxy.h"

void handle_request(Proxy * proxy, LRUCache * cache, logger * req_log, long request_id) {
  //this function will handle the request
  try {
    proxy->handleRequest(*cache, *req_log, request_id);
  }
  catch (std::exception & e) {
    std::cerr << "Can not handle the request";
  }

  std::cout << "Thread exit" << std::endl;
  //destroy the proxy
  delete proxy;
}

int main() {
  //variable for listener
  ListenerSock listener;
  listener.start_up();

  //variable for cache
  LRUCache cache(LRUCACHE_SIZE);

  //variable for log
  logger req_log;
  std::mutex request_id_lock;
  long request_id = 0;

  //variable for thread
  pid_t pid;

  while (true) {
    //create proxy in the heap
    Proxy * proxy = new Proxy();

    //block here and waiting for connection
    listener.accept_(proxy->client);  //throw listener exception

    //once there is a connection spawn a new thread to handle the request
    std::thread thread(handle_request, proxy, &cache, &req_log, request_id);
    thread.detach();  //detach the child thread from the parent thread

    request_id_lock.lock();
    request_id++;
    request_id_lock.unlock();
  }
}