#include "Main.h"

int main(void) {
  /* initial set up */
  //create the listener, start up
  ListenerSock listener;
  listener.start_up();

  /* 
        Becoming a daemon (fork and exit again)
        why? 
        1. Becoming orphan process and adopted by init. 
            Some parent(termina) will kill their child when exit
        2. Daemon is guranteed to not be a group leader
            why not a group leader?
                group leader can not create new session?
                    why create new sessioin?
                        dissociate with controlling tty
    */

  int pid = fork();

  if (pid != 0) {
    //parent process
    exit(0);
  }
  else {
    //orphan child process,

    /* dissociate from controlling tty by using setid */
    int sessionid = setsid();

    if (sessionid == -1) {
      std::cout << "Can not create new session" << std::endl;
      exit(0);
    }

    /* close stdin/stderr/stdout, open them to null */
    int status = 1;
    int devNull = open("/dev/null", O_WRONLY);
    status = dup2(0, devNull);  //close stdin
    status = dup2(1, devNull);  //close stdout
    status = dup2(2, devNull);  //close stderr

    /* change working dir to "/" */

    chdir("/");

    LRUCache cache(LRUCACHE_SIZE);

    /* life cycle */
    while (true) {
      Proxy proxy;  //create a proxy

      //listener will accept a client connect
      //and set the connecting sockfd in client
      listener.accept_(proxy.client);  //throw listener exception

      //receive a request from the client
      proxy.handleRequest(cache);
    }
  }

  //std::cout << std::string(proxy.req.header.begin(), proxy.req.header.end());
  //std::cout << std::string(proxy.req.body.begin(), proxy.req.body.end());

  //std::cout << std::string(proxy.resp.header.begin(), proxy.resp.header.end());
  //std::cout << std::string(proxy.resp.body.begin(), proxy.resp.body.end());
}