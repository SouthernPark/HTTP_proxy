#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#define BUFFER_SIZE 1000000
#define HOSTNAME_SIZE 100
#define PORT_SIZE 100
//this macro is used for sending chunked mess in Utility::send
#define MESS_SIZE 65536
//the max header size supposed by server is 48KB, here we set it to 2^16
#define HEADER_SIZE 65536