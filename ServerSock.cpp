#include "ServerSock.h"

int ServerSock::recv_response_header(Response & resp) {
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

  //push the bytes after header into body
  while (it != header.end()) {
    resp.body.push_back(*it);
    it++;
  }

  return status;
}

int ServerSock::recv_http_response(Response & resp) {
  int status = recv_response_header(resp);

  if (status == -1) {
    std::cerr << "Can not recv from the server" << std::endl;
    throw recv_exception();
  }

  if (status == 0) {
    std::cerr << "Server closed" << std::endl;
  }

  resp.parseHeader();

  auto content_length = resp.header_kvs.find("content-length");

  //check if we have content-length in the header
  if (content_length == resp.header_kvs.end()) {
    //do not have content-length in the header
    std::cerr << "No content-length is found in the header. Check chunked " << std::endl;
    std::cerr << "Checking chunked transfer type" << std::endl;

    status = this->recv_rest_chunked(resp);
  }
  else {
    //we get the content length in the header
    status = this->recv_rest_response(resp, std::stoi((*content_length).second));
  }

  //parse the response header
  resp.parseCacheControl();

  return status;
}

int ServerSock::recv_rest_response(Response & resp, int total) {
  total -= resp.body.size();  //some part of the body has been stored in the resp.body
  std::vector<char> buffer;
  int status = 1;

  //recv the bytes until the total is 0
  while (total > 0) {
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

int ServerSock::recv_rest_chunked(Response & resp) {
  //use "0\r\n" to detect the end of the chunk
  const char * CRLF = "\r\n\r\n";

  std::vector<char> buffer;
  int status = 1;
  //check if the resp.body has already contained the CRLF
  auto it = std::search(resp.body.begin(), resp.body.end(), CRLF, CRLF + strlen(CRLF));
  while (it == resp.body.end()) {
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
    it = std::search(resp.body.begin(), resp.body.end(), CRLF, CRLF + strlen(CRLF));
  }
  //parse trailer
  parse_trailer(resp);

  return status;
}

int ServerSock::buildSocket() {
  int sockfd = Utility::sock_(this->hostname, this->port, &(this->servinfo));
  if (sockfd == -1) {
    std::cerr << "Can not create the sock" << std::endl;
    throw sock_exception();
  }

  this->sockfd = sockfd;
  return sockfd;
}

int ServerSock::connect() {
  int status = Utility::connect_(this->sockfd, this->servinfo);
  if (status == -1) {
    std::cerr << "Can not connect to the server" << std::endl;
    throw connect_exception();
  }

  return status;
}

int ServerSock::send_(std::vector<char> & mess) {
  int status = Utility::send_(this->sockfd, mess);
  if (status == -1) {
    std::cerr << "The mess can not be sent" << std::endl;
    throw send_exception();
  }

  return status;
}

void ServerSock::parse_trailer(Response & resp) {
  //find the "0\r\n"
  const char * del = "0\r\n";
  auto it = std::search(resp.body.begin(), resp.body.end(), del, del + strlen(del));

  std::string trailer(it + 3, resp.body.end());
  std::string CRLF = "\r\n";
  std::string del2 = ": ";
  //there is no trailer
  if (trailer.compare(CRLF) != 0) {
    //split the trailer using CRLF
    std::unique_ptr<std::vector<std::string> > res(Utility::split(trailer, CRLF));
    //split the trailer using ": "
    for (int i = 0; i < (*res).size(); i++) {
      //add kvs[0]:kvs[1] to the unordered_map
      std::unique_ptr<std::vector<std::string> > kvs(Utility::split((*res)[i], del2));
      if ((*kvs).size() >= 2) {
        if (!(*kvs)[0].empty() && !(*kvs)[1].empty()) {
          resp.header_kvs[(*kvs)[0]] = (*kvs)[1];
        }
      }
    }
  }
}
