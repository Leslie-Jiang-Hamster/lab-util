#include "kernel/types.h"
#include "user/user.h"

#define DEBUG
#undef DEBUG

static const int MAX_PRIME = 35;
static const int BUF_SIZE = 40;

static void print_prime(int prime) {
  printf("prime %d\n", prime);
}

static void primes(int pipe_in, int pipe_out) {
  int n_bytes = 0, all_bytes = 0;
  int buf[BUF_SIZE];
  int next_pipefd[2];

  close(pipe_out);
  while ((n_bytes = read(pipe_in, (char *)buf + all_bytes, sizeof(buf) - all_bytes)) > 0) {
    all_bytes += n_bytes;
  }
  close(pipe_in);
  int all_integers = all_bytes / 4;

  #ifdef DEBUG
  printf("all_bytes: %d\n", all_bytes);
  printf("all_integers: %d\n", all_integers);
  for (int i = 0; i < all_integers; i++) {
    printf("buf[%d]: %d\n", i, buf[i]);
  }
  #endif

  if (all_integers < 1) {
    exit(0);
  }
  pipe(next_pipefd);
  if (fork() == 0) {
    primes(next_pipefd[0], next_pipefd[1]);
    exit(0);
  }
  else {
    close(next_pipefd[0]);
    int prime = buf[0];
    print_prime(prime);
    for (int i = 1; i < all_integers; i++) {
      if (buf[i] % prime == 0) {
        continue;
      }
      write(next_pipefd[1], buf + i, 4);
    }
    close(next_pipefd[1]);
    wait((int *)0);
    exit(0);
  }
}

int main(int argc, char **argv) {
  if (argc != 1) {
    fprintf(2, "Usage: %s\n", argv[0]);
    exit(1);
  }
  int pipefd[2];
  pipe(pipefd);
  if (fork() == 0) {
    primes(pipefd[0], pipefd[1]);
  }
  else {
    close(pipefd[0]);
    for (int i = 2; i <= MAX_PRIME; i++) {
      write(pipefd[1], &i, 4);
    }
    close(pipefd[1]);
    wait((int *)0);
  }
  exit(0);
}