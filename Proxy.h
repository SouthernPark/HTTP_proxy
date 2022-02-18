
#ifndef PROXY_H
#define PROXY_H

#include <memory>

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

  /*
    This function will use the request header to find the response
    A. if there is cached response in LRU:
      1. check expiration
          (check Expires: Wed, 21 Oct 2015 07:28:00 GMT)
          (check no-cache)
          (check max-age )

          1. not expire? 
            send the cached response to client
            return
          2. expire ?
            revalidate the cache (we can write as another function)
              1. has etag?
                send If-None-Match to origin server
                1. 304 ? not modified?
                  send the cached resp and return
                2. 200 OK ?
                  update the LRU cache and send the cached resp and return
              2. not have etag but have last modified ?
                  send last modified ex Last-Modified: Wed, 21 Oct 2015 07:28:00 GMT
                  to the server.
                1. 304 ? not modified?
                  send the cached resp and return
                2. 200 OK ?
                  update the LRU cache and send the cached resp and return

    B. no cached resp in LRU
      get the resp from the client directly and check if have 


    if there is no cached response:
       
  */
  void get_resp_from_cache_and_sent_to_client();
};

#endif