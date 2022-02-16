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
  std::string del_CRLF = "\r\n";
  //split the header with CRLF
  //we get the header line by line
  std::unique_ptr<std::vector<std::string> > res(Utility::split(header, del_CRLF));

  std::vector<std::string>::iterator it =
      res->begin();  //the first line is the status line

  if (it != res->end()) {
    //then
    this->request_line = *it;
    //parse the request line
    parseRequestLine(*this);
  }

  it++;
  std::string del = ": ";
  while (it != res->end()) {
    //check whether has ": "
    if ((*it).find(del) == -1) {
      it++;
      continue;
    }
    std::unique_ptr<std::vector<std::string> > line(Utility::split(*it, del));
    if ((*line).size() < 2) {
      it++;
      continue;
    }

    std::string key = (*line)[0];

    for (int i = 0; i < key.size(); i++) {
      key[i] = std::tolower(key[i]);
    }
    std::string val = (*line)[1];

    this->header_kvs[key] = val;
    it++;
  }
  this->parsed = true;
  return;
}