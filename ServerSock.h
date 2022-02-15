#ifndef SERVERSOCK_H
#define SERVERSOCK_H

#include <memory>
#include <string>

#include "MyExceptions.h"
#include "Response.h"
#include "macro.h"
/*
    Client <======> Proxy <======> Server 
    This class is used to connect with the client for proxy, 
    including the following steps 
*/
class ServerSock {
 private:
  /*
    This fucntion will be used to recv the http response header
    Note that: cause the response is sent in stream, we may also recv
    part of the body during recving header.
    Those part of body will be stored in resp.body.
    @ return 
        -1 if can not recv
        0 if the sock is closed
  */
  int recv_response_header(Response & resp);

 public:
  int sockfd;
  std::string hostname;
  std::string port;
  struct addrinfo * servinfo;

  ServerSock(std::string hostname, std::string port) : hostname(hostname), port(port) {}

  ~ServerSock() {
    close(sockfd);
    freeaddrinfo(servinfo);
  }

  /*
    This function will create a sock using the given hostname and port.
    this->sockfd will be set on success
    return -1 if can not build the sock
  */
  int buildSocket();

  /*
    This function will connect the sock to the server with this->hostname and
    this->port
    return -1 if can not connect
  */
  int connect();

  /*
    This function will call the Utility::send() to send bytes in mess.
    This function will send char in mess in series chunk
    with chunk size MESS_SIZE to the specified sock.
    @param: sockfd is the sock fd we want to send mess to
    @param: mess is the message we will send to the sockfd
    @return 
        -1  if an error happens
        elsee return the number of bytes we have sent 
  */
  int send_(std::vector<char> & mess);

  /*
    This function will get heep response from the sockfd and transfer
    the byte stream to Response object including the header and body.

    @param: resp where to store the resp

    @ return 
        -1 if can not recv
        0 if the sock is closed
    @Throws
        no_content_length if there is no content_length arrtibute in the header      
  */
  int recv_http_response(Response & resp);

 private:
  /*
    This function is used when the response header has the content-length
    filed. Else, we need to check transfer encoding "transfer-encoding"
    to see which kind of encoding shema the response is using

    After receive header, we have to receive the html.
    But when receive header we may receive part of the body.
    in recv_http_response, I have add the received body part into
    the body.
    This funcion will be used to receive the rest of the http response.
    @param: where you put the rest of the body
    @param: total is the size of message body which comes by parsing the
            html header
  */
  int recv_rest_response(Response & resp, int total);

  /*
    This function will receive the rest of the chunked data and store
    them in the resp.body.
    return 
        -1 if can not recv
        0 if disconnect from the server

    PS: refer 
    https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
    for more info about chunked data

    The chuked data is combosed by:
    num\r\n
    ...\r\n
    ...\r\n
    ...\r\n
    0\r\n
    Trailers\r\n
    \r\n

    The chunked data is end with \r\n\r\n 
  */
  int recv_rest_chunked(Response & resp);

  /*
    This function will parse the trailer
    trailer has key value pairs that has the same effect
    in the header.
    Therefore it will be added to the resp.header_kvs field.
    This function will be used in the recv_rest_chunked method
    after receiving the chunked data
  */
  void parse_trailer(Response & resp);
};

#endif