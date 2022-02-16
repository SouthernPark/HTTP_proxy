#ifndef LISTENERSOCK_H
#define LISTENERSOCK_H

#include <sys/socket.h>

#include "ClientSock.h"
#include "macro.h"

class ListenerSock {
 public:
  int sockfd;
  const std::string hostname = LISTENER_HOST;
  std::string port;  //which port you want the listener bind4
  struct addrinfo * service_info;

  ListenerSock() : port(LISTENER_PORT) {}
  ListenerSock(std::string port) : port(port) {}

  ~ListenerSock() {
    close(sockfd);
    freeaddrinfo(service_info);
  }

  int start_up();
  /*
    This function will accept the connection and create a client sock
    The infomation of the client sock (including sockfd, sockaddr) is stored
    in the ClientSock object.

    return -1 if error happened
            elsee return the sockfd created

  */
  int accept_(ClientSock & client);
};

#endif