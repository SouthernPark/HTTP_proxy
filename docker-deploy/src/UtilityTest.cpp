#include <fcntl.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "Response.h"

int main() {
  //Response res();
  // std::string str = "Mon, 14 Feb 2022 21:06:11 GMT";
  // Utility::http_date_str_to_gmt(str);

  int devNull = open("/dev/null", O_WRONLY);
  int status1 = dup2(devNull, STDIN_FILENO);   //close stdin
  int status2 = dup2(devNull, STDOUT_FILENO);  //close stdout
  int status3 = dup2(devNull, STDERR_FILENO);  //close stderr
  std::cout << status1 << std::endl;
  std::cout << status2 << std::endl;
  std::cout << status3 << std::endl;
  printf("test\n");

  return 0;
}
