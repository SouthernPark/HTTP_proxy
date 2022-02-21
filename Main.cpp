#include "Main.h"

void handle_request(Proxy * proxy, LRUCache * cache, logger * req_log, long request_id) {
  //this function will handle the request
  try {
    proxy->handleRequest(*cache, *req_log, request_id);
  }
  catch (std::exception & e) {
    std::cerr << "Can not handle the request";
  }

  std::cout << "Thread exit" << std::endl;
  //destroy the proxy
  delete proxy;
}

int main(void) {
  /* initial set up */

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
    status = dup2(devNull, STDIN_FILENO);   //close stdin
    status = dup2(devNull, STDOUT_FILENO);  //close stdout
    status = dup2(devNull, STDERR_FILENO);  //close stderr

    /* change working dir to "/" */

    chdir("/");
    umask(0);  //set umask to 0 give the daemon premission (open file, write file)

    //fork again (not be a session leader)
    pid = fork();
    if (pid != 0) {
      //parent process
      exit(0);
    }
    else {
      //variable for listener
      ListenerSock listener;
      listener.start_up();

      //variable for cache
      LRUCache cache(LRUCACHE_SIZE);

      //variable for log
      logger req_log;
      std::mutex request_id_lock;
      long request_id = 0;

      //variable for thread
      pid_t pid;
      /* life cycle */
      while (true) {
        //create proxy in the heap
        Proxy * proxy = new Proxy();

        //block here and waiting for connection
        listener.accept_(proxy->client);  //throw listener exception

        //once there is a connection spawn a new thread to handle the request
        std::thread thread(handle_request, proxy, &cache, &req_log, request_id);
        thread.detach();  //detach the child thread from the parent thread

        request_id_lock.lock();
        request_id++;
        request_id_lock.unlock();
      }
    }
  }
}