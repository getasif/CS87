#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {

  int pid, x, y, z;

  y = 10;
  z = 6;
  pid = fork();
  printf("pid = %d\n", pid);
  if(pid) {
    printf("I am the parent");
    printf("pid = %d\n", pid);
    fflush(stdout);
    wait(0);
  } else {

    printf("I am the child");
    fflush(stdout);

    // to stop this process so that we can attach a 
    // debugger, send ourself a STOP signal
    // then get child process' pid (ps -a)
    // then attach gdb to child:
    //    gdb <executable> <pid>
    kill(getpid(), SIGSTOP);

    x = y + z;

  }
  exit(0);
}
