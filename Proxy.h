
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
      get the resp from the client directly and check if have no-cache
      
      if there is no cached response and 200 OK:
        cache the response.
      
      send the response to the user
        
       
  */
  void get_resp_from_cache_and_sent_to_client(LRUCache & cache);

  /*
    By calling this function, you ensure that their is no cached response. 
    This function will:
    get the resp from the client directly and check if have no-cache
      
    if there is no cached response and 200 OK:
        cache the response.
      
    send the response to the user
  */
  void handleNotCachedGet(LRUCache & cache);

  /*
    This function will:
      1. add E-tag to the req header if there is any
      2. add last-modified to the req header if there is any

      3. send the req to server 
      4. check response 
        a. 200 OK ?
            update the resp in the cache
        b. 304 not modified ?
            //do nothing about the cache
      5. send the the resp in the cache to client
  */
  void handleRevalidate(LRUCache & cache);

  /*
    helper function that will 
      1. add E-tag to the req header if there is any
      2. add last-modified to the req header if there is any
  */

  std::vector<char> makeRevalidateHeader();
};

#endif