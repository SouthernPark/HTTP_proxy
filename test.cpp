#include <arpa/inet.h>

#include <iostream>

#include "Utility.h"

int main(void) {
  std::string host_name = "baidu.com";
  std::string port = "80";
  struct addrinfo * servinfo;
  int fd = Utility::sock_(host_name, port, &servinfo);

  struct sockaddr_in * internet_addr = (struct sockaddr_in *)servinfo->ai_addr;

  printf("google.com is at: %s\n", inet_ntoa(internet_addr->sin_addr));

  int status = Utility::connect_(fd, servinfo);

  std::cout << "status is " << status << std::endl;
}