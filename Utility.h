#include "Response.h"
#include "macro.h"

class Utility {
 public:
  /*
    This function will recv from the sock and put the receivng mess
    in chunk size HEADER_SIZE specified in macro.h
    to a vector mess  
    @param: sockfd is the sock fd used to recv mess from
    @param: mess where the mess will go 
            (the previous mess will be cleared)
    @return: 
        -1  if an error happens
        0   if the connection is closed
        else return the number of bytes have been received
    @guarantee: the recv_ can at least receive a whole header 
  */
  static int recv_(int sockfd, std::vector<char> & mess);

  /*
    This function will send char in mess in series chunk
    with chunk size MESS_SIZE to the specified sock.
    @param: sockfd is the sock fd we want to send mess to
    @param: mess is the message we will send to the sockfd
    @return 
        -1  if an error happens
        elsee return the number of bytes we have sent 
    */
  static int send_(int sockfd, std::vector<char> & mess);

  /*
    This function build a socket with the host and port provided and
    return the sock file descriptor.
    @param: host is the hostname or ip you want to connect 
            if you want to build listener please specify host as NULL
    @param: port is which port you want to connect
            "0" if you want to bind to a random available port
    @param: servinfo will store infor created by getaddrinfo()
            You will ues these info to connect

    return the sock file descriptor
    */
  static int sock_(std::string & hostname,
                   std::string & port,
                   struct addrinfo ** servinfo);

  /*
    This function will connect with the host specified in service_info
    @param sockfd is the socket fd used for connection
    @param service_info contains the info like dest host and port

    @return -1 if can not connect (remember to check)
  */
  static int connect_(int sockfd, struct addrinfo * service_info);

  /*
    this function is used to get the http response
    @param sockfd the fd you want to hear from server
    @param rsp where you store the response which includes
           the header and the body 
  */

  static int recv_Http_response(int sockfd, Response & rsp);
};