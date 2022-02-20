#include "LRUCache.h"
#include "ListenerSock.h"
#include "Proxy.h"

int main() {
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();
  LRUCache cache(LRUCACHE_SIZE);
  int count = 30;
  while (count != 0) {
    Proxy proxy{};  //create a proxy

    //listener will accept a client connect
    //and set the connecting sockfd in client
    listener.accept_(proxy.client);  //throw listener exception

    //receive a request from the client
    proxy.handleRequest(cache);
    //std::cout << std::string(proxy.req.header.begin(), proxy.req.header.end());
    //std::cout << std::string(proxy.resp.header.begin(), proxy.resp.header.end());

    //std::cout << std::string(proxy.resp.body.begin(), proxy.resp.body.end());

    //std::cout << proxy.resp.header.size() + proxy.resp.body.size() << std::endl;
    //std::cout << "header size: " << proxy.resp.header.size() << std::endl;
    auto it = proxy.resp.cache_control_kvs.begin();
    // while (it != proxy.resp.cache_control_kvs.end()) {
    //   std::cout << "key: " << (*it).first << std::endl;
    //   std::cout << "value: " << (*it).second << std::endl;

    //   it++;
    // }
    // std::cout << proxy.resp.header_kvs["etag"] << std::endl;
    // std::cout << proxy.resp.response_code << std::endl;

    count--;
    std::cout << "=======================" << count << std::endl;
    //print cache-control
  }

  //std::cout << std::string(proxy.req.header.begin(), proxy.req.header.end());
  //std::cout << std::string(proxy.req.body.begin(), proxy.req.body.end());

  //std::cout << std::string(proxy.resp.header.begin(), proxy.resp.header.end());
  //std::cout << std::string(proxy.resp.body.begin(), proxy.resp.body.end());
}
