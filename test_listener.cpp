#include "ListenerSock.h"
#include "ServerSock.h"

int main() {
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();

  //create a client sock
  ClientSock client;

  //listener will accept a client connect
  //and set the connecting sockfd in client
  int fd = listener.accept_(client);

  if (fd == -1) {
    std::cerr << "Can not create listerner" << std::endl;
    return -1;
  }

  //receive a request from the client
  Request req;
  client.recv_http_request(req);

  req.parseHeader();
  std::cout << std::string(req.header.begin(), req.header.end());
  //create a server sock
}