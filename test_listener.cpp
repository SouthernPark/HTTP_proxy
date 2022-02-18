#include <thread>

#include "LRUCache.h"
#include "ListenerSock.h"
#include "Proxy.h"

void handle_request(Proxy * proxy, LRUCache * cache) {
  //this function will handle the request
  try {
    proxy->handleRequest(*cache);
  }
  catch (std::exception & e) {
    std::cerr << "Can not handle the request";
  }

  std::cout << "Thread exit" << std::endl;
  //destroy the proxy
  delete proxy;
}

int main() {
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();
  LRUCache cache(LRUCACHE_SIZE);
  int count = 30;
  pid_t pid;
  while (count != 0) {
    //create proxy in the heap
    Proxy * proxy = new Proxy();

    //block here and waiting for connection
    listener.accept_(proxy->client);  //throw listener exception

    //once there is a connection spawn a new thread to handle the request
    std::thread thread(handle_request, proxy, &cache);
    thread.detach();  //detach the child thread from the parent thread
  }

  //std::cout << std::string(proxy.req.header.begin(), proxy.req.header.end());
  //std::cout << std::string(proxy.req.body.begin(), proxy.req.body.end());

  //std::cout << std::string(proxy.resp.header.begin(), proxy.resp.header.end());
  //std::cout << std::string(proxy.resp.body.begin(), proxy.resp.body.end());
}