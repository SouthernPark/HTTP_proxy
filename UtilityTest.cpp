#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "Response.h"

int main() {
  //Response res();
  std::string str = "date: Mon, 14 Feb 2022 03:28:47 GMT";
  std::string del = ": ";
  std::unique_ptr<std::vector<std::string> > spl(Utility::split(str, del));

  std::cout << (*spl)[0] << std::endl;
  std::cout << (*spl).size() << std::endl;
  std::cout << (*spl)[1] << std::endl;

  return 0;
}
