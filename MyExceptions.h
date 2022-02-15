#include <exception>

class no_content_length : public std::exception {
  virtual const char * what() const throw() {
    return "no content-length find in the header";
  }
};