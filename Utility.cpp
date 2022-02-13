#include "Utility.h"

#include "macro.h"
/*
    This function build a socket with the host and port provided and
    return the sock file descriptor.
    @param: host is the hostname or ip you want to connect 
            if you want to build listener please specify host as NULL
    @param: port is which port you want to connect
            "0" if you want to bind to a random available port

    return 
        -1 if can not create the sock
        else the sock file descriptor
    */
int Utility::sock_(std::string & hostname,
                   std::string & port,
                   struct addrinfo ** servinfo) {
  int status;
  struct addrinfo hints;

  char hostname_array[HOSTNAME_SIZE];
  char port_array[PORT_SIZE];

  char buffer[BUFFER_SIZE];

  memset(&hints, 0, sizeof hints);  // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;      // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

  strcpy(hostname_array, hostname.c_str());
  strcpy(port_array, port.c_str());

  if ((status = getaddrinfo(hostname_array, port_array, &hints, servinfo)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    return -1;
  }

  //create socket
  int sockfd =
      socket((*servinfo)->ai_family, (*servinfo)->ai_socktype, (*servinfo)->ai_protocol);

  if (sockfd == -1) {
    std::cout << "Can not create the sock" << std::endl;
    return -1;
  }

  return sockfd;
}

int Utility::connect_(int sockfd, struct addrinfo * service_info) {
  int status;
  status = connect(sockfd, service_info->ai_addr, service_info->ai_addrlen);

  if (status == -1) {
    std::cerr << "Can not connect" << std::endl;
  }
  return status;
}