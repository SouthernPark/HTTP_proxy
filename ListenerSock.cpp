#include "ListenerSock.h"

int ListenerSock::start_up() {
  std::string hostname = "";
  int fd = Utility::sock_(hostname, this->port, &(this->service_info));
  if (fd == -1) {
    std::cerr << "The listener can not create sock" << std::endl;
    return -1;
  }
  this->sockfd = fd;
  int status = 1;

  //reuse the port if the port has been occupied
  int yes = 1;
  if (status =
          setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    std::cerr << "setsockopt report an error" << std::endl;
    return status;
  }

  //bind the listener sock to a port
  status =
      bind(this->sockfd, this->service_info->ai_addr, this->service_info->ai_addrlen);
  if (status == -1) {
    std::cerr << "Can not bind the sock" << std::endl;
    return -1;
  }
  //listen on the sock
  status = listen(sockfd, LISTENER_MAX_CONNECT);
  if (status == -1) {
    std::cerr << "Can not listen on the port" << std::endl;
    return status;
  }
  return status;
}

int ListenerSock::accept_(ClientSock & client) {
  int new_fd =
      accept(this->sockfd, (struct sockaddr *)&(client.client_addr), &(client.addr_size));
  if (new_fd == -1) {
    std::cerr << "Listener Can not accept" << std::endl;
    return -1;
  }
  client.sockfd = new_fd;
  return new_fd;
}
