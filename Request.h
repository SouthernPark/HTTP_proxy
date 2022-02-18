#ifndef REQUEST_H
#define REQUEST_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "MyExceptions.h"
#include "Utility.h"

/*
  Request same as response
*/
class Request {
 public:
  std::vector<char> header;
  std::vector<char> body;

  std::string port = "80";
  std::unordered_map<std::string, std::string> header_kvs;

  std::string request_line;
  std::string request_method;
  std::string request_url;
  std::string protocol;

  bool parsed;

  Request() : parsed(false) {}

  void parseHeader();

  //equal operator
  bool operator==(const Request & rhs) const {
    /* Two requests only differ by the request line + host */
    std::string req(this->request_line.begin(), this->request_line.end());
    auto it = this->header_kvs.find("host");
    if (it != this->header_kvs.end()) {
      req.insert(req.end(), (*it).second.begin(), (*it).second.end());
    }

    std::string req_rhs(rhs.request_line.begin(), rhs.request_line.end());
    it = rhs.header_kvs.find("host");
    if (it != rhs.header_kvs.end()) {
      req_rhs.insert(req_rhs.end(), (*it).second.begin(), (*it).second.end());
    }

    int cmp = req.compare(req_rhs);
    if (cmp == 0) {
      return true;
    }
    else {
      return false;
    }
  }

  bool operator!=(const Request & rhs) const { return !(*this == rhs); }
  //hash function
  std::size_t operator()(const Request & k) const;
};

class RequestHashFunction {
 public:
  size_t operator()(const Request & req) const {
    std::string ha(req.request_line.begin(), req.request_line.end());
    auto it = req.header_kvs.find("host");
    if (it != req.header_kvs.end()) {
      ha.insert(ha.end(), (*it).second.begin(), (*it).second.end());
    }

    //c++ provides default hash to string
    return std::hash<std::string>()(ha);
  }
};

#endif