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
    Utility::str_to_lowercase(key);

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
  parseCacheControl();
  return;
}

void Request::parseCacheControl() {
  if (cache_parsed) {
    return;
  }
  parseHeader();

  //get cache control
  auto cache_control_it = header_kvs.find("cache-control");

  if (cache_control_it == header_kvs.end()) {
    //no cache_control
    return;
  }

  //split using comma
  std::string del = ", ";
  std::unique_ptr<std::vector<std::string> > splits(
      Utility::split((*cache_control_it).second, del));

  //for each element in the vector, split useing ":"
  std::string del2 = "=";
  auto it = (*splits).begin();
  while (it != (*splits).end()) {
    std::unique_ptr<std::vector<std::string> > kvs(Utility::split((*it), del2));
    if ((*kvs).size() < 1) {
      continue;
    }
    std::string key = (*kvs)[0];
    Utility::str_to_lowercase(key);
    if ((*kvs).size() == 1) {
      cache_control_kvs[key] = "";
    }
    if ((*kvs).size() == 2) {
      cache_control_kvs[key] = (*kvs)[1];
    }
    it++;
  }

  cache_parsed = true;
}