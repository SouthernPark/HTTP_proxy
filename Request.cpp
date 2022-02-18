#include "Request.h"

void parseRequestLine(Request & req) {
  std::string del = " ";
  std::unique_ptr<std::vector<std::string> > res(Utility::split(req.request_line, del));

  if ((*res).size() >= 3) {
    req.request_method = (*res)[0];
    req.request_url = (*res)[1];
    req.protocol = (*res)[2];
  }
}

void Request::parseHeader() {
  if (this->parsed) {
    return;
  }

  std::string header(this->header.begin(), this->header.end());
  //split the header
  std::string del_CRLF = "\r\n";
  std::unique_ptr<std::vector<std::string> > res(Utility::split(header, del_CRLF));

  /* get the first request line */
  std::vector<std::string>::iterator it = res->begin();

  if (it != res->end()) {
    this->request_line = *it;

    parseRequestLine(*this);
  }

  /* get the following key: value pairs */
  it++;
  std::string del = ": ";
  while (it != res->end()) {
    //check whether has ": "
    if ((*it).find(del) == -1) {
      it++;
      continue;
    }
    //split
    std::unique_ptr<std::vector<std::string> > line(Utility::split(*it, del));
    //make sure there are two elements in the kv pairs
    if ((*line).size() < 2) {
      it++;
      continue;
    }
    //set key
    std::string key = (*line)[0];
    //key to lower case
    for (int i = 0; i < key.size(); i++) {
      key[i] = std::tolower(key[i]);
    }
    std::string val = (*line)[1];
    //add key val pairs
    this->header_kvs[key] = val;
    it++;
  }

  /* parse port and hostname */
  std::string del2 = ":";
  auto host_it = this->header_kvs.find("host");
  if (host_it == this->header_kvs.end()) {
    throw no_host();
  }

  std::unique_ptr<std::vector<std::string> > host_and_port(
      Utility::split((*host_it).second, del2));

  if ((*host_and_port).size() == 2) {
    this->port = (*host_and_port)[1];
  }

  this->parsed = true;
  return;
}