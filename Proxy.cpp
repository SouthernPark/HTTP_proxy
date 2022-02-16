#include "Proxy.h"

void Proxy::handleRequest() {
  int status = this->client.recv_http_request(this->req);
  this->req.parseHeader();
  if (status == -1) {
    std::cerr << "HandleGet in Proxy.cpp can not recv" << std::endl;
    throw recv_exception();
  }
  //check the method of header
  if (this->req.request_method.compare("GET") == 0) {
    this->handleGet();
  }
  else if (this->req.request_method.compare("POST") == 0) {
    this->handlePOST();
  }
  else if (this->req.request_method.compare("CONNECT") == 0) {
    this->handleCONNECT();
  }
}

void Proxy::handleGet() {
  this->req.parseHeader();

  //build up the server sock
  this->server.hostname = req.header_kvs["host"];
  this->server.port = req.port;

  server.buildSocket();  //throw sock_exception if sock can not build
  server.connect();      //throw connect rxception if sock can not connect

  //send header and body
  server.send_(req.header);  //throw send exception
  server.send_(req.body);

  server.recv_http_response(this->resp);  //throw recv exception
  client.send_(resp.header);              //throw send exception
  client.send_(resp.body);
}

void Proxy::handlePOST() {
  //we can just use handle get to handlePOST
  Proxy::handleGet();
}

void Proxy::handleCONNECT() {
}