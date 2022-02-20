#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "Response.h"

int main() {
  //Response res();
  // std::string str = "Mon, 14 Feb 2022 21:06:11 GMT";
  // Utility::http_date_str_to_gmt(str);

  std::time_t now = Utility::get_current_time_gmt();
  std::cout << now << std::endl;

  return 0;
}
