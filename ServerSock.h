#ifndef SERVERSOCK_H
#define SERVERSOCK_H

#include "Response.h"
#include "macro.h"

/*
    Client <======> Proxy <======> Server 
    This class is used to connect with the client for proxy, 
    including the following steps 
*/
class ServerSock {
 public:
  int sockfd;
  std::string hostname;
  std::string port;
  struct addrinfo * servinfo;

 public:
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

    return 
        -1 if can not recv
        0 if the sock is closed
  */
  int recv_http_response(Response & resp);

 private:
  /*
    After receive header, we have to receive the html.
    But when receive header we may receive part of the body.
    in recv_http_response, I have add the received body part into
    the body.
    This funcion will be used to receive the rest of the http response.
    @param: where you put the rest of the body
    @param: haveReceived is how many bytes you have received when receiving
            header
    @param: total is the size of message body which comes by parsing the
            html header
  */
  int recv_rest_response(Response & resp, int haveReceive, int total);
};

#endif