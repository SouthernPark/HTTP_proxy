#ifndef REQUEST_H
#define REQUEST_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utility.h"

/*
  Request same as response
*/
class Request {
 public:
  std::vector<char> header;
  std::vector<char> body;

  std::unordered_map<std::string, std::string> header_kvs;

  //
  std::string request_line;
  std::string request_method;
  std::string request_url;
  std::string protocol;

  bool parsed;

  Request() : parsed(false) {}

  void parseHeader();
};

#endif