#include "Proxy.h"

void Proxy::handleRequest(LRUCache & cache) {
  int status = this->client.recv_http_request(this->req);
  this->req.parseHeader();
  if (status == -1) {
    std::cerr << "HandleGet in Proxy.cpp can not recv" << std::endl;
    throw recv_exception();
  }
  //check the method of header
  if (this->req.request_method.compare("GET") == 0) {
    std::cout << "GET";
    //check LRU cache
    Response * resp = cache.get(this->req);
    if (resp != NULL) {
      // std::cout << "Fetched from LRU" << std::endl;
      this->resp = *resp;
      client.send_(this->resp.header);  //throw send exception
      client.send_(this->resp.body);
      return;
    }
    this->handleGet();
    //cache the GET request

    cache.put(this->req, this->resp);
  }
  else if (this->req.request_method.compare("POST") == 0) {
    std::cout << "POST";
    this->handlePOST();
  }
  else if (this->req.request_method.compare("CONNECT") == 0) {
    std::cout << "CONNECT";
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
  this->req.parseHeader();

  //build up the server sock
  std::string del = ":";  //the host of CONNECT is in the format www.youtube.com:443
  std::unique_ptr<std::vector<std::string> > host_port(
      Utility::split(this->req.header_kvs["host"], del));

  if ((*host_port).size() == 2) {
    this->server.hostname = (*host_port)[0];
    this->server.port = (*host_port)[1];
  }
  else if ((*host_port).size() == 1) {
    //port 80 is used
    this->server.hostname = (*host_port)[0];
    this->server.port = "80";
  }
  else {
    //error
    throw host_port_no_find_exception();
  }

  server.buildSocket();  //throw sock_exception if sock can not build
  server.connect();      //throw connect exception if sock can not connect

  std::string connect_ok_str = "HTTP/1.1 200 OK\r\nconnection: keep-alive\r\n\r\n";
  std::vector<char> connect_ok(connect_ok_str.begin(), connect_ok_str.end());
  this->client.send_(connect_ok);

  //std::cout << "Connection built in CONNECT" << std::endl;

  //send header to server

  fd_set readfds;
  FD_ZERO(&readfds);                      //pre-clear the sock
  FD_SET(this->client.sockfd, &readfds);  //add the client sock
  FD_SET(this->server.sockfd, &readfds);  //add the server sock
  //proxy receive mess from both side
  while (true) {
    int max = this->server.sockfd < this->server.sockfd ? this->server.sockfd
                                                        : this->server.sockfd;
    select(max + 1, &readfds, NULL, NULL, NULL);
    bool exit = false;
    if (FD_ISSET(this->client.sockfd, &readfds)) {
      //client has sent you the data
      std::vector<char> mess;
      //recv mess from client
      int status = this->client.recv_(mess);
      // std::cout << "Received message from the client. recv: " << status << std::endl;
      if (status == 0) {
        //client disconnected
        exit = true;  //client may still have something to send
      }
      if (status == -1) {
        throw recv_exception();
      }

      this->server.send_(mess);  //send mess to server
    }

    if (FD_ISSET(this->server.sockfd, &readfds)) {
      //server has sent you the data
      std::vector<char> mess;
      int status = this->server.recv_(mess);
      // std::cout << "Received message from the server. recv: " << status << std::endl;
      if (status == 0) {
        exit = true;
      }
      if (status == -1) {
        throw recv_exception();
      }

      this->client.send_(mess);  //send mess to client
    }

    if (exit) {
      return;
    }

    //after calling select. remeber to reset readfds
    //it only contains the fds that currently sending you data
    FD_ZERO(&readfds);                      //pre-clear the sock
    FD_SET(this->client.sockfd, &readfds);  //add the client sock
    FD_SET(this->server.sockfd, &readfds);  //add the server sock
  }
}