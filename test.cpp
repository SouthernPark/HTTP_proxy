#include <arpa/inet.h>
#include <unistd.h>

#include <fstream>

#include "ServerSock.h"
int main(void) {
  //create a server sock that will help us create sock, send request and receive response

  std::string host_name = "go.com";
  std::string port = "80";
  ServerSock sock(host_name, port);

  //create the sock
  sock.buildSocket();

  struct sockaddr_in * internet_addr = (struct sockaddr_in *)sock.servinfo->ai_addr;

  printf("ip is at: %s\n", inet_ntoa(internet_addr->sin_addr));

  //build up the request header
  std::stringstream stream;

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
         << "Host: " << host_name << "\r\n"
         << "Accept-Language: " << accept_language << "\r\n"
         << "accept: " << accept_ << "\r\n"
         << "\r\n";

  //connect
  int status = sock.connect();

  //send the header
  std::string header = stream.str();
  std::vector<char> buffer(header.begin(), header.end());
  status = sock.send(sock.sockfd, buffer);

  if (status == -1) {
    std::cerr << "Can not send\n";
    return 0;
  }
  //so not close the sock
  std::cout << "The message has been sent\n";

  Response resp;
  sock.recv_http_response(resp);

  //check the response header

  //check header
  std::string head(resp.header.begin(), resp.header.end());
  //std::cout << head << std::endl;
  std::ofstream header_file("test.header");
  header_file << head;

  //check body
  std::string body(resp.body.begin(), resp.body.end());
  //std::cout << body << std::endl;
  std::ofstream body_file("test.html");
  body_file << body;
}