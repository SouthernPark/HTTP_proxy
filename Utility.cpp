#include "Utility.h"
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

/*
  A helper function that wil help us send all the buf 
  to fd.
*/
int sendAll(int fd, char * buf, int * len) {
  int total = 0;  //how many bytes have been sent
  int bytesleft = *len;
  int n = 0;
  //continuous send until all the message is sent
  while (total < *len) {
    n = send(fd, buf + total, bytesleft, 0);
    if (n == -1) {
      //error
      break;
    }
    total += n;
    bytesleft -= n;
  }

  *len = total;  //number actuall sent

  //return -1 on failure, 0 on success
  return n == -1 ? -1 : 0;
}

/*
    This function will send char in mess in series chunk
    with chunk size MESS_SIZE to the specified sock.
    @param: sockfd is the sock fd we want to send mess to
    @param: mess is the message we will send to the sockfd
    @return 
        -1  if an error happens
        elsee return the number of bytes we have sent 
    */

int Utility::send_(int sockfd, std::vector<char> & mess) {
  int len = mess.size();
  int buffer_len;
  int status;
  char buffer[MESS_SIZE];
  int round = len / MESS_SIZE;
  int remain = len % MESS_SIZE;
  //record the start of each round
  int index = 0;
  for (int i = 0; i < round; i++) {
    //put mess into buffer
    for (int i = 0; i < MESS_SIZE; i++) {
      buffer[i] = mess[i + index];
    }

    //send the message to sockfd
    buffer_len = MESS_SIZE;
    status = sendAll(sockfd, buffer, &buffer_len);
    if (status == -1) {
      std::cerr << "send error" << std::endl;
      return status;
    }
    index += MESS_SIZE;
  }

  //send the remaining
  for (int i = 0; i < remain; i++) {
    buffer[i] = mess[i + index];
  }
  buffer_len = remain;
  status = sendAll(sockfd, buffer, &buffer_len);
  if (status == -1) {
    std::cerr << "send error" << std::endl;
    return status;
  }

  return status;
}

int Utility::recv_(int sockfd, std::vector<char> & mess) {
  //clear the data in mess
  mess.clear();
  char buffer[HEADER_SIZE];
  int status = 1;

  status = recv(sockfd, buffer, HEADER_SIZE, 0);

  if (status == -1) {
    std::cerr << "can not recv" << std::endl;
    return -1;
  }

  //on success, status is the number of char received
  for (int i = 0; i < status; i++) {
    mess.push_back(buffer[i]);
  }
  return status;
}
