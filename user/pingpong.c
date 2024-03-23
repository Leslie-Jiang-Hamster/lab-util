#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 64

const static char PINGPONG = 42;
static int pipefd[2];

static void child() {
  char buf[BUF_SIZE];
  int my_pid = getpid();
  int n_bytes = read(pipefd[0], buf, sizeof(buf));
  close(pipefd[0]);
  if (n_bytes != 1 || buf[0] != PINGPONG) {
    fprintf(2, "pingpong failed\n");
    exit(1);
  }
  printf("%d: received ping\n", my_pid);
  write(pipefd[1], buf, n_bytes);
  close(pipefd[1]);
}

static void parent() {
  char buf[BUF_SIZE];
  int my_pid = getpid();
  int n_bytes = write(pipefd[1], &PINGPONG, sizeof(PINGPONG));
  close(pipefd[1]);
  n_bytes = read(pipefd[0], buf, sizeof(buf));
  close(pipefd[0]);
  if (n_bytes != 1 || buf[0] != PINGPONG) {
    fprintf(2, "pingpong failed\n");
    exit(1);
  }
  printf("%d: received pong\n", my_pid);
}

int main(int argc, char **argv) {
  if (argc != 1) {
    fprintf(2, "Usage: %s\n", argv[0]);
    exit(1);
  }
  pipe(pipefd);
  if (fork() == 0) {
    child();
  }
  else {
    parent();  
  }
  exit(0);
}