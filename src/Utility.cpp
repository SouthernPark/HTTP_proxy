#include "Utility.h"
/*
    This function build a socket with the host and port provided and
    return the sock file descriptor.
    @param: host is the hostname or ip you want to connect 
            if hostname is "", that means NULL 
            if you want to build listener please specify host as ""
    @param: port is which port you want to connect
            "0" if you want to bind to a random available port

    return 
        -1 if can not create the sock
        else the sock file descriptor
    */
int Utility::sock_(std::string & hostname,
                   std::string & port,
                   struct addrinfo ** servinfo) {
  int status;
  struct addrinfo hints;

  char hostname_array[HOSTNAME_SIZE];
  char port_array[PORT_SIZE];

  char buffer[BUFFER_SIZE];

  memset(&hints, 0, sizeof hints);  // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;      // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

  strcpy(hostname_array, hostname.c_str());
  strcpy(port_array, port.c_str());

  //if hostname is "", we give null, else we give hostname array
  if ((status = getaddrinfo(
           hostname.size() == 0 ? NULL : hostname_array, port_array, &hints, servinfo)) !=
      0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    return -1;
  }

  //create socket
  int sockfd =
      socket((*servinfo)->ai_family, (*servinfo)->ai_socktype, (*servinfo)->ai_protocol);

  if (sockfd == -1) {
    std::cout << "Can not create the sock" << std::endl;
    return -1;
  }

  return sockfd;
}

int Utility::connect_(int sockfd, struct addrinfo * service_info) {
  int status;
  status = connect(sockfd, service_info->ai_addr, service_info->ai_addrlen);

  if (status == -1) {
    std::cerr << "Can not connect" << std::endl;
  }
  return status;
}

/*
  A helper function that wil help us send all the buf 
  to fd.
*/
int Utility::sendAll(int fd, char * buf, int * len) {
  int total = 0;  //how many bytes have been sent
  int bytesleft = *len;
  int n = 0;
  //continuous send until all the message is sent
  while (total < *len) {
    n = send(fd, buf + total, bytesleft, 0);
    if (n == -1) {
      //error
      break;
    }
    total += n;
    bytesleft -= n;
  }

  *len = total;  //number actuall sent

  //return -1 on failure, 0 on success
  return n == -1 ? -1 : 0;
}

/*
    This function will send char in mess in series chunk
    with chunk size MESS_SIZE to the specified sock.
    @param: sockfd is the sock fd we want to send mess to
    @param: mess is the message we will send to the sockfd
    @return 
        -1  if an error happens
        elsee return the number of bytes we have sent 
    */

int Utility::send_(int sockfd, std::vector<char> & mess) {
  int len = mess.size();
  int buffer_len;
  int status;
  char buffer[MESS_SIZE];
  int round = len / MESS_SIZE;
  int remain = len % MESS_SIZE;
  //record the start of each round
  int index = 0;
  for (int i = 0; i < round; i++) {
    //put mess into buffer
    for (int i = 0; i < MESS_SIZE; i++) {
      buffer[i] = mess[i + index];
    }

    //send the message to sockfd
    buffer_len = MESS_SIZE;
    status = sendAll(sockfd, buffer, &buffer_len);
    if (status == -1) {
      std::cerr << "send error" << std::endl;
      return status;
    }
    index += MESS_SIZE;
  }

  //send the remaining
  for (int i = 0; i < remain; i++) {
    buffer[i] = mess[i + index];
  }
  buffer_len = remain;
  status = sendAll(sockfd, buffer, &buffer_len);
  if (status == -1) {
    std::cerr << "send error" << std::endl;
    return status;
  }

  return status;
}

int Utility::recv_(int sockfd, std::vector<char> & mess) {
  //clear the data in mess
  mess.clear();
  char buffer[HEADER_SIZE];
  int status = 1;

  status = recv(sockfd, buffer, HEADER_SIZE, 0);

  if (status == -1) {
    std::cerr << "can not recv" << std::endl;
    return -1;
  }

  //on success, status is the number of char received
  for (int i = 0; i < status; i++) {
    mess.push_back(buffer[i]);
  }
  return status;
}

std::vector<std::string> * Utility::split(std::string & input, std::string & delimiter) {
  std::vector<std::string> * res = new std::vector<std::string>;

  int start = 0;
  int end = input.find(delimiter);

  while (end != -1) {
    res->push_back(input.substr(start, end - start));
    start = end + delimiter.size();
    end = input.find(delimiter, start);
  }

  if (start != input.size()) {
    res->push_back(input.substr(start, input.size() - start));
  }

  return res;
}

std::time_t Utility::get_current_time_gmt() {
  auto curr = std::chrono::system_clock::now();

  //convert to time_t
  time_t curr_time_t = std::chrono::system_clock::to_time_t(curr);
  return curr_time_t;
}

/*

  Mon, 14 Feb 2022 21:06:11 GMT
  %a, %d %b %Y %H:%M:%S GMT

  21:06:11
  "%H:%M:%S"
*/
std::time_t Utility::http_date_str_to_gmt(std::string http_date) {
  std::tm t = {};
  std::istringstream ss(http_date);

  ss.imbue(std::locale(""));

  ss >> std::get_time(&t, "%a, %d %b %Y %H:%M:%S GMT");
  if (ss.fail()) {
    std::cout << "http date is: " << http_date << std::endl;
    std::cout << "failed to get http date" << std::endl;
  }

  //http date in local time zone
  std::time_t local_date = std::mktime(&t);

  std::time_t gmt_date = local_date - EST_TIME_ZONE * 3600;

  return gmt_date;
}

std::string Utility::get_peer_ip(int sockfd) {
  socklen_t len;
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];  //ipv4
  len = sizeof addr;
  int port;
  getpeername(sockfd, (struct sockaddr *)&addr, &len);
  if (addr.ss_family == AF_INET) {
    //ipv4
    struct sockaddr_in * s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
  }
  else {
    struct sockaddr_in6 * s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
  }

  std::string res(ipstr);
  res += ":" + std::to_string(port);

  return res;
}
