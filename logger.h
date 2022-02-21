#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>

class logger {
  /*
        logger should have:
            1. file stream that writes to /var/log/erss/proxy.log
            2. a mutex that prevents two threads write at the same time
    */

  std::mutex log_mtx;
  std::ofstream outfile;
  std::stringstream ss;

 public:
  logger() { outfile.open("proxy.log", std::ios::out); }
  ~logger() { outfile.close(); }
  void writeToLog(std::string str) {
    if (outfile.is_open()) {
      log_mtx.lock();
      std::cout << "Write to log: " << str << std::endl;
      outfile << str;
      outfile.flush();
      log_mtx.unlock();
    }
    else {
      std::cerr << "Can not open the file" << std::endl;
    }
  }

  /* ID: "REQUEST" from IPFROM @ TIME */
  void log_new_request(long request_id, Request & req, int sockfd) {
    /* get ip of the request connection */
    std::string ip_port = Utility::get_peer_ip(sockfd);
    ss.str("");
    ss << request_id << ": " << req.request_line << " from " << ip_port;
    auto now = Utility::get_current_time_gmt();
    ss << " @ ";
    ss << Utility::time_t_to_string_gmt(now) << std::endl;
    writeToLog(ss.str());
  }

  void log_response(long request_id, Response & resp) {
    ss.str("");
    ss << request_id << ": "
       << "Responsing " << resp.first_line << std::endl;
    writeToLog(ss.str());
  }

  /* ID: not in cache */
  void log_not_in_cache(long request_id) {
    ss.str("");
    ss << request_id << ": not in cache" << std::endl;
    writeToLog(ss.str());
  }

  /* ID: in cache, but expired at EXPIREDTIME */
  void log_in_cache_but_expire(long request_id, std::time_t expire_time) {
    ss.str("");
    ss << request_id << ": in cache, but expired at ";
    ss << Utility::time_t_to_string_gmt(expire_time) << std::endl;
    writeToLog(ss.str());
  }
  /* ID: in cache, requires validation*/
  void log_in_cache_requre_validation(long request_id) {
    ss.str("");
    ss << request_id << ": in cache, requires validation" << std::endl;
    writeToLog(ss.str());
  }
  /* ID: in cache, valid  */
  void log_in_cache_valid(long request_id) {
    ss.str("");
    ss << request_id << ": in cache, valid" << std::endl;
    writeToLog(ss.str());
  }
  /* ID: ID: Requeseting REQUEST_LINE from HOST */
  void log_request_to_server(long request_id, Request & req) {
    ss.str("");
    ss << request_id << ": Requesting " << req.request_line;
    ss << " from " << req.header_kvs["host"] << std::endl;
    writeToLog(ss.str());
  }
  /* ID: ID: Received RESPONSE_LINE from HOST */
  void log_resp_from_server(long request_id, Request & req) {
    ss.str("");
    ss << request_id << ": Received " << req.request_line;
    ss << " from " << req.header_kvs["host"] << std::endl;
    writeToLog(ss.str());
  }

  /* 
    ID: not cacheable because REASON
    ID: cached, expires at EXPIRES
    ID: cached, but requires re-validation 
   */
  void log_resp_cache(long request_id, Request & req, Response & resp) {
    /* check not cachable */
    auto req_no_store_it = req.cache_control_kvs.find("no-store");
    auto resp_no_store_it = resp.cache_control_kvs.find("no-store");
    if (req_no_store_it != req.cache_control_kvs.end() ||
        resp_no_store_it != resp.cache_control_kvs.end()) {
      //the client or the server does not want to store in the cache
      ss.str("");
      ss << request_id << ": not cacheable because no-store in cache control "
         << resp.first_line << std::endl;
      writeToLog(ss.str());
      return;
    }

    /* check expires */
    auto req_no_cache_it = req.cache_control_kvs.find("no-cache");
    auto resp_no_cache_it = resp.cache_control_kvs.find("no-cache");
    auto max_age = resp.cache_control_kvs.find("max-age");

    bool expire_soon = req_no_cache_it != req.cache_control_kvs.end() ||
                       resp_no_cache_it != resp.cache_control_kvs.end();
    if (max_age != resp.cache_control_kvs.end()) {
      expire_soon |= std::stoi(max_age->second) == -1 ? true : false;
    }

    if (expire_soon) {
      ss.str("");
      ss << request_id << ": cached, expires at "
         << Utility::time_t_to_string_gmt(Utility::get_current_time_gmt()) << std::endl;
      writeToLog(ss.str());
    }
    else {
      //have expires attribute header ?
      auto expires_date = resp.header_kvs.find("expires");
      if (expires_date != resp.header_kvs.end()) {
        ss.str("");
        auto http_expire_date = Utility::http_date_str_to_gmt(expires_date->second);
        ss << request_id << ": cached, expires at "
           << Utility::time_t_to_string_gmt(http_expire_date) << std::endl;
        writeToLog(ss.str());
      }

      //has max-age?
      if (max_age != resp.cache_control_kvs.end()) {
        auto http_date_str = resp.header_kvs.find("date");
        if (http_date_str != resp.header_kvs.end()) {
          auto http_date = Utility::http_date_str_to_gmt(http_date_str->second);
          auto date_plus_max_age = http_date + std::stoi(max_age->second);
          ss.str("");
          ss << request_id << ": cached, expires at "
             << Utility::time_t_to_string_gmt(date_plus_max_age) << std::endl;
          writeToLog(ss.str());
        }
      }
    }

    /* checks revalidation */

    auto resp_reval = resp.cache_control_kvs.find("must-revalidate");
    if (resp_reval != resp.cache_control_kvs.end()) {
      ss.str("");
      ss << request_id << ": cached, but requires re-validation" << std::endl;
      writeToLog(ss.str());
    }
  }
  /* ID: NOTE not cachable bacause response code is */
  void log_status_not_200(long request_id, Response & resp) {
    ss.str("");
    ss << request_id << ": NOTE not cachable because response code is "
       << resp.response_code << std::endl;
    writeToLog(ss.str());
  }

  /* ID: NOTE resp updated in cache */

  void log_resp_update(long request_id) {
    ss.str("");
    ss << request_id << ": NOTE response updated in cache " << std::endl;
    writeToLog(ss.str());
  }

  /* ID: Tunnel closed */
  void log_tunnel_close(long request_id) {
    ss.str("");
    ss << request_id << ": Tunnel closed" << std::endl;
    writeToLog(ss.str());
  }
};

#endif