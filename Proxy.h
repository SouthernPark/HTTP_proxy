
#ifndef PROXY_H
#define PROXY_H

#include "ClientSock.h"
#include "ServerSock.h"

class Proxy {
  ServerSock server;
  ClientSock client;
};

#endif