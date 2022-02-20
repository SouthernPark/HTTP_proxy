#include "Proxy.h"

void Proxy::handleRequest(LRUCache & cache) {
  //recv the request header and body if there is
  int status = this->client.recv_http_request(this->req);
  if (status == -1) {
    std::cerr << "HandleGet in Proxy.cpp can not recv" << std::endl;
    throw recv_exception();
  }
  //parse the request header
  this->req.parseHeader();

  //check the method of header
  if (this->req.request_method.compare("GET") == 0) {
    get_resp_from_cache_and_sent_to_client(cache);
    //cache the GET request
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
  server.send_request(req);

  //get resp from server
  server.recv_http_response(this->resp);  //throw recv exception
  //send resp to client
  client.send_response(resp);  //throw send exceptio
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

//this function will check current time and
bool expires(Response & resp, Request & req) {
  //1. check if have expires filed like Expires: Wed, 21 Oct 2015 07:28:00 GMT
  auto expires_it = resp.header_kvs.find("expires");
  if (expires_it != resp.header_kvs.end()) {
    //expires can be -1 or 0
    if ((*expires_it).second.size() <= 5) {
      return true;
    }

    std::cout << "http_date in test1: " << expires_it->second << std::endl;
    std::time_t resp_expire_date = Utility::http_date_str_to_gmt(expires_it->second);
    std::time_t now = Utility::get_current_time_gmt();
    if (now > resp_expire_date) {
      std::cout << "Now is beyong the expireation time" << std::endl;
      return true;
    }
  }

  //2. check if the req resp has no-cache (which means the client or server always want to revalidate)
  auto req_no_cache = req.cache_control_kvs.find("no-cache");
  auto resp_no_cache = resp.cache_control_kvs.find("no-cache");
  if (req_no_cache != req.cache_control_kvs.end() ||
      resp_no_cache != resp.cache_control_kvs.end()) {
    std::cout << "No-cache" << std::endl;
    return true;
  }

  //3. check if has max-age
  auto resp_max_age = resp.cache_control_kvs.find("max-age");
  if (resp_max_age != resp.cache_control_kvs.end()) {
    //get the http date
    auto http_date = resp.header_kvs.find("date");
    if (http_date == resp.header_kvs.end()) {
      //no date
      std::cerr << "There is no date in http resp" << std::endl;
      return true;
    }
    //convert http date string to time_t in gmt/utc
    std::cout << "http_date in test2 is: " << http_date->second << std::endl;
    auto http_time = Utility::http_date_str_to_gmt(http_date->second);
    //convert max_age str to integer
    int max_age = std::stoi(resp_max_age->second);
    //get current time
    auto now = Utility::get_current_time_gmt();
    if (http_time + max_age < now /* date + max-age < local time */) {
      std::cout << "The http date in header is: " << http_date->second
                << ". The timestamp is: " << http_time << std::endl;

      std::cout << "Current time stamp is: " << now << std::endl;

      std::cout << "Beyong the max-age" << std::endl;

      return true;
    }
  }

  //4.if there is no cache-control header, we can think it as no cache restrictions
  return false;
}

void Proxy::get_resp_from_cache_and_sent_to_client(LRUCache & cache) {
  // req.parseHeader();
  // req.parseCacheControl();
  //check in cache or not
  Response * cached_resp = cache.get(req);
  if (cached_resp == NULL) {
    // not in cache
    std::cout << "ID: Not in Cache" << std::endl;  //TODO: write into log later
    handleNotCachedGet(cache);
    return;
  }

  //in cache
  // this->resp = *cached_resp;
  // std::cout << "----------------------------------------------" << std::endl;
  // std::cout << std::string(resp.header.begin(), resp.header.end());
  // std::cout << std::string(resp.body.begin(), resp.body.end());
  // std::cout << "In cache resp size: " << resp.body.size() << std::endl;
  // std::cout << "Head size: " << resp.header.size() << std::endl;
  // std::cout << "Body size: " << resp.body.size() << std::endl;
  cached_resp->parseHeader();

  //check expire
  if (!expires(*cached_resp, this->req)) {
    //not expire
    //check if have must-revalidate in cache-control
    auto must_reval_it_req = req.cache_control_kvs.find("must-revalidate");
    auto must_reval_it_resp = cached_resp->cache_control_kvs.find("must-revalidate");
    if (must_reval_it_req != req.cache_control_kvs.end() ||
        must_reval_it_resp != cached_resp->cache_control_kvs.end()) {
      std::cout << "ID: in cache, requires validation" << std::endl;
      handleRevalidate(cache);
    }
    else {
      std::cout << "ID: in cache, valid" << std::endl;

      //send the cached response to the client

      std::cout << "In cache resp size: " << resp.body.size() << std::endl;
      client.send_response(*cached_resp);
    }
  }
  else {
    //expired needs revalidate
    std::cout << "ID: in cache, but expired at EXPIREDTIME" << std::endl;
    handleRevalidate(cache);
  }
}

void Proxy::handleNotCachedGet(LRUCache & cache) {
  // this->req.parseHeader();

  //build up the server sock
  this->server.hostname = req.header_kvs["host"];
  this->server.port = req.port;

  server.buildSocket();  //throw sock_exception if sock can not build
  server.connect();      //throw connect rxception if sock can not connect

  std::cout << "Connected" << std::endl;

  //send header and body
  server.send_request(req);  //may throw send exception
  std::cout << "test1" << std::endl;

  //recv response from server
  server.recv_http_response(this->resp);  //throw recv exception
  std::cout << "test2" << std::endl;
  //std::cout << std::string(resp.header.begin(), resp.header.end()) << std::endl;

  //TODO: check whether response header has no-store in cache conrtol
  // resp.parseHeader();
  // resp.parseCacheControl();
  auto no_store_it = resp.cache_control_kvs.find("no-store");

  //std::cout << std::string(resp.body.begin(), resp.body.begin() + 500) << std::endl;
  if (resp.response_code.compare("200") == 0 &&
      no_store_it == resp.cache_control_kvs.end()) {
    //update the cache
    std::cout << "Status == 200 and no no-store, can sotre in the cache" << std::endl;
    cache.put(this->req, this->resp);
    client.send_response(resp);
    //std::cout << "Cached in handleNotCachedGet function" << std::endl;
  }
  else {
    std::cout << std::string(resp.header.begin(), resp.header.end()) << std::endl;
    std::cout << "status != 200 or no-store in the cache, does not store in the cache"
              << std::endl;
    client.send_response(resp);  //may throw send exception
  }
  // std::cout << std::string(resp.body.begin(), resp.body.end()) << std::endl;
  std::cout << "test3" << std::endl;
}

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
void Proxy::handleRevalidate(LRUCache & cache) {
  //build up the server sock
  this->server.hostname = req.header_kvs["host"];
  this->server.port = req.port;

  server.buildSocket();  //throw sock_exception if sock can not build
  server.connect();      //throw connect rxception if sock can not connect

  std::vector<char> reval_header = makeRevalidateHeader();
  server.send_(reval_header);
  Response reval_resp;
  server.recv_http_response(reval_resp);

  reval_resp.parseHeader();

  if (reval_resp.response_code.compare("304") == 0) {
    //not modified
    std::cout << "Revalidation: server response 304 not modified" << std::endl;
  }
  else if (reval_resp.response_code.compare("200") == 0) {
    std::cout << "Revalidation: server response 200 updated" << std::endl;
    //update the resp in the cache
    cache.put(req, reval_resp);
    std::cout << "Cached size resp header: " << cache.get(req)->header.size()
              << std::endl;
    std::cout << "Cached size resp body: " << cache.get(req)->body.size() << std::endl;
  }
  else {
    std::cout << "Revalidate other tha 304 or 200" << std::endl;
    handleNotCachedGet(cache);
  }

  //send the resp in the cache to client
  Response * cached_resp = cache.get(req);
  if (cached_resp == NULL) {
    throw no_resp_cache();
  }
  client.send_response(*cached_resp);
  std::cout << "Cached size resp header: " << cache.get(req)->header.size() << std::endl;
  std::cout << "Cached size resp body: " << cache.get(req)->body.size() << std::endl;
  return;
}

std::vector<char> Proxy::makeRevalidateHeader() {
  //1. get the req header
  std::vector<char> header = req.header;  //assignment operator

  //remove \r\n at the end
  header.erase(header.end() - 2, header.end());

  //2. insert Etag or last-modified to the header end
  auto etag_it = resp.header_kvs.find("etag");
  std::string etag = "ETag: ";
  if (etag_it != resp.header_kvs.end()) {
    header.insert(header.end(), etag.begin(), etag.end());
    header.insert(header.end(), (etag_it->second).begin(), (etag_it->second).end());
    header.push_back('\r');
    header.push_back('\n');
  }

  //3. insert last-modified to the header
  std::string last_modified = "Last-Modified: ";
  auto last_modified_it = resp.header_kvs.find("last-modified");
  if (last_modified_it != resp.header_kvs.end()) {
    header.insert(header.end(), last_modified.begin(), last_modified.end());
    header.insert(header.end(),
                  (last_modified_it->second).begin(),
                  (last_modified_it->second).end());
    header.push_back('\r');
    header.push_back('\n');
  }

  //add \r\n at the end
  header.push_back('\r');
  header.push_back('\n');

  return header;
}