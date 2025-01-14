#include "ClientSock.h"

/*
    This function will get heep response from the sockfd and transfer
    the byte stream to Response object including the header and body.

    return 
        -1 if can not recv
        0 if the sock is closed
*/

int ClientSock::recv_http_request(Request & resp) {
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
  std::unordered_map<std::string, std::string>::iterator length =
      resp.header_kvs.find(std::string("content-length"));

  if (length == resp.header_kvs.end()) {
    //there should be content-length filed in the request header
    //some request has body with it like post request
    //while some does not
    return status;
  }
  this->recv_rest_request(resp, body_in_header, std::stoi(length->second));
  return status;
}

int ClientSock::recv_rest_request(Request & resp, int haveReceive, int total) {
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
  return status;
}

void ClientSock::send_response(Response & resp) {
  this->send_(resp.header);  //throw send exception
  this->send_(resp.body);
}
