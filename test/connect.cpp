#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <string>

#define BUFFER_SIZE 1000000

int sendall(int s, char * buf, int * len) {
  int total = 0;         // how many bytes we've sent
  int bytesleft = *len;  // how many we have left to send
  int n;

  while (total < *len) {
    n = send(s, buf + total, bytesleft, 0);
    if (n == -1) {
      break;
    }
    total += n;
    bytesleft -= n;
  }

  *len = total;  // return number actually sent here

  return n == -1 ? -1 : 0;  // return -1 on failure, 0 on success
}

int main(void) {
  //add hints to the sock
  int status;
  struct addrinfo hints;
  struct addrinfo * servinfo;
  std::string hostname = "go.com";
  char hostname_array[100];
  char port_array[100];
  std::string port = "80";

  char buffer[BUFFER_SIZE];
  std::stringstream stream;
  int s;

  memset(&hints, 0, sizeof hints);  // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;      // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

  strcpy(hostname_array, hostname.c_str());
  strcpy(port_array, port.c_str());
  if ((status = getaddrinfo(hostname_array, port_array, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    return 0;
  }

  //create socket
  s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if (s == -1) {
    std::cout << "Can not create the sock" << std::endl;
    return 0;
  }

  //build up the header
  std::string method = "GET";
  std::string url = "/";

  std::string user_agent =
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/98.0.4758.80 Safari/537.36 Edg/98.0.1108.50";
  std::string accept_language = "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6";
  std::string accept_ =
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/"
      "*;q=0.8,application/signed-exchange;v=b3;q=0.9";
  stream << method << " " << url << " HTTP/1.1"
         << "\r\n"
         << "Host: " << hostname << "\r\n"
         << "Accept-Language: " << accept_language << "\r\n"
         << "accept: " << accept_ << "\r\n"
         << "\r\n";
  //connect the socket with the remote
  status = connect(s, servinfo->ai_addr, servinfo->ai_addrlen);
  if (status == -1) {
    std::cerr << "Can not connect" << std::endl;
    return 0;
  }

  std::string temp = stream.str();
  std::cout << temp;
  strcpy(buffer, temp.c_str());

  //send the header to the server
  int len = strlen(buffer) + 1;
  status = sendall(s, buffer, &len);
  if (status == -1) {
    std::cerr << "Can not send to server" << std::endl;
  }

  status = recv(s, buffer, BUFFER_SIZE, MSG_WAITALL);
  if (status == -1) {
    std::cerr << "Can not receive from the remote" << std::endl;
    return 0;
  }
  else if (status == 0) {
    std::cout << "Disconnected from the server\n";
  }

  //printf("%s", buffer);
  FILE * file = fopen("test.html", "w");
  int results = fputs(buffer, file);
  if (results == EOF) {
    fprintf(stderr, "Can not write to file\n");
    return 0;
  }
  return 0;
}
