#include "ServerSock.h"

/*
    This function will get heep response from the sockfd and transfer
    the byte stream to Response object including the header and body.

    return 
        -1 if can not recv
        0 if the sock is closed
*/

int ServerSock::recv_http_response(Response & resp) {
  int status = 1;
  std::vector<char> header;
  std::vector<char> buffer;
  std::vector<char>::iterator CRLF_index;
  const char * CRLF = "\r\n\r\n";
  //recv header end until we detec the CRLF
  while (1) {
    status = Utility::recv_(this->sockfd, buffer);
    if (status == -1) {
      return -1;
    }
    //add buffer to header
    header.insert(header.end(), buffer.begin(), buffer.end());
    //check if there is \r\n in the header
    CRLF_index = std::search(header.begin(), header.end(), CRLF, CRLF + strlen(CRLF));
    if (CRLF_index != header.end()) {
      break;
    }
  }
  //copy constrcutor
  std::vector<char>::iterator it = header.begin();
  //push header into response
  while (it != CRLF_index + 4) {
    resp.header.push_back(*it);
    it++;
  }
  //how many bytes are recved when we just want header
  int body_in_header = 0;
  //push the bytes after header into body
  while (it != header.end()) {
    resp.body.push_back(*it);
    it++;
    body_in_header++;
  }

  resp.parseHeader();

  auto content_length = resp.header_kvs.find("content-length");

  if (content_length == resp.header_kvs.end()) {
    std::cerr << "" << std::endl;
    return -2;
  }

  this->recv_rest_response(resp, body_in_header, std::stoi((*content_length).second));

  // this->recv_rest_response(resp, body_in_header, 100);

  return status;
}

int ServerSock::recv_rest_response(Response & resp, int haveReceive, int total) {
  total -= haveReceive;
  std::vector<char> buffer;
  int status = 1;

  //recv the bytes until the total is 0
  while (total != 0) {
    status = Utility::recv_(this->sockfd, buffer);
    if (status == -1) {
      std::cerr << "Can not recv from the client" << std::endl;
      return -1;
    }
    if (status == 0) {
      std::cerr << "Disconnect with the origin server" << std::endl;
      return 0;
    }
    resp.body.insert(resp.body.end(), buffer.begin(), buffer.end());
    total -= status;
  }

  //recv the bytes until the totoal is not \r\n
  // const char * CRLF = "\r\n\r\n";
  // auto it = std::search(resp.body.begin(), resp.body.end(), CRLF, CRLF + strlen(CRLF));
  // while (it == resp.body.end()) {
  //   status = Utility::recv_(this->sockfd, buffer);
  //   if (status == -1) {
  //     std::cerr << "Can not recv from the client" << std::endl;
  //     return -1;
  //   }
  //   if (status == 0) {
  //     std::cerr << "Disconnect with the origin server" << std::endl;
  //     return 0;
  //   }
  //   resp.body.insert(resp.body.end(), buffer.begin(), buffer.end());
  //   it = std::search(resp.body.begin(), resp.body.end(), CRLF, CRLF + strlen(CRLF));
  // }

  return status;
}

int ServerSock::buildSocket() {
  int sockfd = Utility::sock_(this->hostname, this->port, &(this->servinfo));
  if (sockfd == -1) {
    std::cerr << "Can not create the sock" << std::endl;
    return -1;
  }

  this->sockfd = sockfd;
  return sockfd;
}

int ServerSock::connect() {
  int status = Utility::connect_(this->sockfd, this->servinfo);
  if (status == -1) {
    std::cerr << "Can not connect to the server" << std::endl;
    return -1;
  }

  return status;
}

int ServerSock::send_(std::vector<char> & mess) {
  int status = Utility::send_(this->sockfd, mess);
  if (status == -1) {
    std::cerr << "The mess can not be sent" << std::endl;
    return -1;
  }

  return status;
}
