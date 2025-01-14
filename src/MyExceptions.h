#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <exception>

class no_content_length : public std::exception {
  virtual const char * what() const throw() {
    return "no content-length find in the header\n";
  }
};

class recv_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not recv\n"; }
};

class listener_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not create listener\n"; }
};

class sock_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not build the sock\n"; }
};

class connect_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not connect\n"; }
};

class send_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not send\n"; }
};

class host_port_no_find_exception : public std::exception {
  virtual const char * what() const throw() { return "Can not get the host and port\n"; }
};

class no_host : public std::exception {
  virtual const char * what() const throw() { return "Can not get host\n"; }
};

class no_resp_cache : public std::exception {
  virtual const char * what() const throw() { return "Can not find resp in cache\n"; }
};

#endif