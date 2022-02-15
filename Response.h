#ifndef RESPONSE_H
#define RESPONSE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utility.h"

class Response {
 public:
  std::string first_line;
  std::vector<char> header;
  std::vector<char> body;

  std::unordered_map<std::string, std::string> header_kvs;

  bool parsed;

  Response() : parsed(false) {}

  virtual void parseHeader();
};
#endif