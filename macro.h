#ifndef MACRO_H
#define MACRO_H

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#define BUFFER_SIZE 65536
#define HOSTNAME_SIZE 100
#define PORT_SIZE 100
//used by send, this macro is used for sending chunked mess in Utility::send
#define MESS_SIZE 65536
//used by recv, the max header size supposed by server is 48KB, here we set it to 2^16
#define HEADER_SIZE 65536

#define LISTENER_PORT "12345"
#define LISTENER_HOST "0.0.0.0"
#define LISTENER_MAX_CONNECT 100

#define LRUCACHE_SIZE 100

#define EST_TIME_ZONE 5

#endif
