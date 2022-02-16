#ifndef DAEMON_H
#define DAEMON_H

#include <unistd.h>

#include <iostream>

/*
    this daemon process will fork a new process
    and load the Main binary file into the new process

    The daemon process will then exit
*/

int main() {
  //fork
  int pid = fork();
  if (pid == 0) {
    //child process
    //load the Main program into the process with execve()
    char * argv[] = {NULL};
    char * envp[] = {NULL};
    int err = execve("Main", argv, envp);
    std::cout << "execve error" << std::endl;
    //execve does not return on success
  }
  else {
    //parent process
    return 0;
  }
  return 0;
}

#endif