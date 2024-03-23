#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

#define BUF_SIZE 32
#define DEBUG
#undef DEBUG

static char file[BUF_SIZE];

static char *get_pwd(const char *path_) {
  char *path = malloc(BUF_SIZE * sizeof(char));
  strcpy(path, path_);
  int len = strlen(path);
  for (int i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      return path + i + 1;
    }
  }
  return path;
}

static const char *join(const char *path, const char *file) {
  char *joined_path = malloc(BUF_SIZE * sizeof(char));
  int path_len = strlen(path);
  int file_len = strlen(file);
  for (int i = 0; i < path_len; i++) {
    joined_path[i] = path[i];
  }
  if (path[path_len - 1] == '/') {
    for (int i = 0; i < file_len; i++) {
      joined_path[path_len + i] = file[i];
    }
    joined_path[path_len + file_len] = '\0';
    return joined_path;
  }
  joined_path[path_len] = '/';
  for (int i = 0; i < file_len; i++) {
    joined_path[path_len + i + 1] = file[i];
  }
  joined_path[path_len + file_len + 1] = '\0';
  return joined_path;
}

static void find(const char *path_) {
  char *path = malloc(BUF_SIZE * sizeof(char));
  int fd;
  struct stat st;
  struct dirent de;
  strcpy(path, path_);

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "cannot open: %s\n", path);
    exit(1);
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "cannot stat: %s\n", path);
    exit(1);
  }

  if (st.type == T_FILE && strcmp(file, get_pwd(path)) == 0) {
    printf("%s\n", path);
  }
  else if (st.type == T_DIR) {
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0) {
        continue;
      }
      #ifdef DEBUG
      printf("!%s\n", de.name);
      #endif
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }
      find(join(path, de.name));
    }
  }
  close(fd);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(2, "Usage: %s <path> <file>\n", argv[0]);
    exit(1);
  }
  strcpy(file, argv[2]);
  find(argv[1]);
  exit(0);
}