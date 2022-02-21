#ifndef LISTENERSOCK_H
#define LISTENERSOCK_H

#include <errno.h>
#include <sys/socket.h>

#include "ClientSock.h"
#include "macro.h"
class ListenerSock {
 public:
  int sockfd;
  const std::string hostname = LISTENER_HOST;
  std::string port;  //which port you want the listener bind4
  struct addrinfo * service_info;

  ListenerSock() : sockfd(-1), port(LISTENER_PORT), service_info(NULL) {}
  ListenerSock(std::string port) : port(port) {}

  ~ListenerSock() {
    if (sockfd != -1) {
      std::cout << "Listenered closed" << std::endl;
      close(sockfd);
    }
    if (service_info != NULL) {
      freeaddrinfo(service_info);
    }
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