#include "macro.h"

/*
    Client <======> Proxy <======> Server 
    This class is used to connect with the client for proxy, 
    including the following steps 
*/
class Server {
  int sockfd;
  std::string hostname;
  std::string port;
  struct addrinfo * servinfo;

 public:
  Server(std::string hostname, std::string port) : hostname(hostname), port(port) {}
  int buildSocket();
  /*



  */
  int recv_http_response(int sockfd, );
};