#include <arpa/inet.h>

#include <iostream>

#include "Utility.h"

int main(void) {
  std::string host_name = "127.0.0.1";
  std::string port = "4444";
  struct addrinfo * servinfo;
  int fd = Utility::sock_(host_name, port, &servinfo);

  struct sockaddr_in * internet_addr = (struct sockaddr_in *)servinfo->ai_addr;

  printf("ip is at: %s\n", inet_ntoa(internet_addr->sin_addr));

  int status = Utility::connect_(fd, servinfo);

  std::cout << "status is " << status << std::endl;

  //test send
  std::string mess = "123456789012345678901234567890";
  std::vector<char> buffer(mess.begin(), mess.end());
  status = Utility::send_(fd, buffer);
  if (status == -1) {
    std::cerr << "Can not send\n";
    return 0;
  }
  //so not close the sock
  std::cout << "The message has been sent\n";
  while (1) {
  }
  freeaddrinfo(servinfo);
}