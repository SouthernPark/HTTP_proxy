
#ifndef PROXY_H
#define PROXY_H

#include "ClientSock.h"
#include "LRUCache.h"
#include "Request.h"
#include "Response.h"
#include "ServerSock.h"

class Proxy {
  /*

  */
 public:
  ServerSock server;
  ClientSock client;
  Request req;
  Response resp;

  Proxy() {}

  /*
    This function will receive the request from the client
    and what type fo requests it is and call for the handle*()
    func
  */
  void handleRequest(LRUCache & cache);

  /*
    This function will handle the get request from the client sock
    The response will be got from server sock
    Then the response will be given back to the client sock

    PS:: latter we will add cache to GET
  */

  void handleGet();
  void handlePOST();
  void handleCONNECT();
};

#endif