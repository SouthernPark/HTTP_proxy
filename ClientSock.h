#ifndef CLIENTSOCK_H
#define CLIENTSOCK_H
#include <unistd.h>

#include "Request.h"

#endif

class ClientSock {
  /*When the user connects to our proxy the listener 
  will generate a client sock object which will store the
  information about the user ip, port and the sockfd used
  to connect with the user.
    */

 public:
  int sockfd;
  struct sockaddr_storage client_addr;  //where the ip and port stored
  socklen_t addr_size;                  //set as the size of sockaddr_storeage

  ClientSock() : addr_size(sizeof client_addr) {}

  //close the sock
  ~ClientSock() { close(sockfd); }

  /*
    send the mess to the client 
    This function will utiliz the function in Utility::send_
    This function will send char in mess in series chunk
    with chunk size MESS_SIZE to the specified sock.
    @param: sockfd is the sock fd we want to send mess to
    @param: mess is the message we will send to the sockfd
    @return 
        -1  if an error happens
        elsee return the number of bytes we have sent
  */
  int send_(std::vector<char> & mess) {
    int status = Utility::send_(this->sockfd, mess);
    if (status == -1) {
      std::cerr << "The mess can not be sent" << std::endl;
      return -1;
    }

    return status;
  }
  /*
    This functoin will receive mess from the client
  */
  int recv_(std::vector<char> & mess) {
    int status = Utility::recv_(this->sockfd, mess);

    if (status == -1) {
      std::cerr << "Can not recv from client" << std::endl;
      return -1;
    }

    if (status == 0) {
      std::cout << "client disconnected (recv return 0)" << std::endl;
    }
    return status;
  }

  /*
    This function will get heep response from the sockfd and transfer
    the byte stream to Response object including the header and body.

    @param: resp where to store the resp

    return 
        -1 if can not recv
        0 if the sock is closed
  */
  int recv_http_request(Request & req);

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
  int recv_rest_request(Request & req, int haveReceive, int total);
};