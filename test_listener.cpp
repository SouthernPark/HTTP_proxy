#include "LRUCache.h"
#include "ListenerSock.h"
#include "Proxy.h"

int main() {
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();
  LRUCache cache(LRUCACHE_SIZE);
  int count = 30;
  pid_t pid;
  while (count != 0) {
    Proxy proxy;  //create a proxy

    //listener will accept a client connect
    //and set the connecting sockfd in client
    listener.accept_(proxy.client);  //throw listener exception
    if ((pid = fork()) == 0) {
      //child process
      std::cout << "This is child process" << std::endl;
      //close(listener.sockfd);
      //listener.~ListenerSock();  //close the listener in child process
      proxy.handleRequest(cache);
      listener.~ListenerSock();
      std::cout << "++++++++++++++++++++++++++++++++++++";
      std::cout << "exit from child process" << std::endl;
      exit(0);  //exit parent process
    }
    //receive a request from the client
    std::cout << "This is parent process" << std::endl;
    proxy.client.~ClientSock();  //close the client in parent process
  }

  //std::cout << std::string(proxy.req.header.begin(), proxy.req.header.end());
  //std::cout << std::string(proxy.req.body.begin(), proxy.req.body.end());

  //std::cout << std::string(proxy.resp.header.begin(), proxy.resp.header.end());
  //std::cout << std::string(proxy.resp.body.begin(), proxy.resp.body.end());
}