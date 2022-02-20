#include "Response.h"

void Response::parseHeader() {
  if (this->parsed) {
    return;
  }
  std::string header(this->header.begin(), this->header.end());
  std::string del_CRLF = "\r\n";
  //split the header with CRLF
  //we get the header line by line
  std::unique_ptr<std::vector<std::string> > res(Utility::split(header, del_CRLF));
  if (res->size() == 0) {
    std::cout << std::string(header.begin(), header.end());
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    return;
  }
  std::vector<std::string>::iterator it =
      res->begin();  //the first line is the status line
  if (it != res->end()) {
    this->first_line = *it;
    std::string sp = " ";
    std::unique_ptr<std::vector<std::string> > resp_line(
        Utility::split(this->first_line, sp));
    //get the response code

    if (resp_line->size() == 3) {
      this->response_code = (*resp_line)[1];
    }
    else if (resp_line->size() == 2) {
      this->response_code = (*resp_line)[0];
    }
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
    Utility::str_to_lowercase(key);

    for (int i = 0; i < key.size(); i++) {
      key[i] = std::tolower(key[i]);
    }
    std::string val = (*line)[1];

    this->header_kvs[key] = val;
    it++;
  }

  this->parsed = true;
  parseCacheControl();
  return;
}

/*
  cache control is in the format:
    Cache-Control: max-age=604800, must-revalidate, no-cache

    This function will store them in the hashMap
      {
        max-age : "604800",
        revalidate : "",
        no-cache : ""
      }
*/
void Response::parseCacheControl() {
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
    //set key to lower case
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
