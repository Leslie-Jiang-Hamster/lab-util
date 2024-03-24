#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUF_SIZE 256
#define ARG_SIZE 32
#define NULL (void *)0

static char* my_args[MAXARG];
static char line[BUF_SIZE];

void parse(int pos) {
  char *space_pos = line, *next_space_pos;
  while ((next_space_pos = strchr(space_pos, ' ')) != NULL) {
    *next_space_pos = '\0';
    my_args[pos] = malloc(ARG_SIZE * sizeof(char));
    strcpy(my_args[pos], space_pos);
    pos++;
    space_pos = next_space_pos + 1;
  }
  next_space_pos = strchr(space_pos, '\n');
  *next_space_pos = '\0';
  my_args[pos] = malloc(ARG_SIZE * sizeof(char));
  strcpy(my_args[pos], space_pos);
  pos++;
  my_args[pos] = NULL;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(2, "Not enough arguments\n");
    exit(1);
  }
  for (int i = 1; i < argc; i++) {
    my_args[i - 1] = argv[i];
  }
  while (gets(line, BUF_SIZE) != NULL) {
    if (strlen(line) == 0) {
      break;
    }
    parse(argc - 1);
    if (fork() == 0) {
      exec(my_args[0], my_args);
      fprintf(2, "Exec failed\n");
      exit(1);
    }
    else {
      wait(NULL);
    }
  }
  exit(0);
}