#ifndef REQUEST_H
#define REQUEST_H

#include "Response.h"

#endif

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

  virtual void parseHeader();
};