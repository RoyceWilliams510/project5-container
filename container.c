#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <linux/limits.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <wait.h>
#include <string.h>
#include "change_root.h"

#define CONTAINER_ID_MAX 16
#define CONTAINER_IMG_MAX 64
#define MAX_ARGS 10

#define CHILD_STACK_SIZE 4096 * 10

typedef struct container {
  char id[CONTAINER_ID_MAX];
  // TODO: Add fields
  char image[PATH_MAX];
} container_t;

/**
 * `usage` prints the usage of `client` and exists the program with
 * `EXIT_FAILURE`
 */
void usage(char* cmd) {
  printf("Usage: %s [ID] [IMAGE] [CMD]...\n", cmd);
  exit(EXIT_FAILURE);
}

/**
 * `container_exec` is an entry point of a child process and responsible for
 * creating an overlay filesystem, calling `change_root` and executing the
 * command given as arguments.
 */
int container_exec(void* arg) {
  container_t* container = (container_t*)arg;
  // this line is required on some systems
  if (mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL) < 0) {
    err(1, "mount / private");
  }
  // if (stat("/some/directory", &st) == -1) {
  //   mkdir("/some/directory", 0700);
  // }
  // TODO: Create a overlay filesystem
  // `lowerdir`  should be the image directory: ${cwd}/images/${image}
  // `upperdir`  should be `/tmp/container/{id}/upper`
  // `workdir`   should be `/tmp/container/{id}/work`
  // `merged`    should be `/tmp/container/{id}/merged`
  // ensure all directories exist (create if not exists) and
  // call `mount("overlay", merged, "overlay", MS_RELATIME,
  //    lowerdir={lowerdir},upperdir={upperdir},workdir={workdir})`

  // TODO: Call `change_root` with the `merged` directory
  // change_root(merged)

  // TODO: use `execvp` to run the given command and return its return value
  return 0;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    usage(argv[0]);
  }

  /* Create tmpfs and mount it to `/tmp/container` so overlayfs can be used
   * inside Docker containers */
  if (mkdir("/tmp/container", 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create a directory to store container file systems");
  }
  if (errno != EEXIST) {
    if (mount("tmpfs", "/tmp/container", "tmpfs", 0, "") < 0) {
      err(1, "Failed to mount tmpfs on /tmp/container");
    }
  }

  /* cwd contains the absolute path to the current working directory which can
   * be useful constructing path for image */
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  container_t container;
  // TODO: store all necessary information to `container`

  //for the path for the image
  char path[PATH_MAX];
  strcpy(path,cwd);
  strcat(path,"/images/");
  strcat(path,argv[2]);

  //for the executable and arguments
  char *command_string = argv[3];
  char *argument_list[MAX_ARGS];
  argument_list[0] = command_string;
  int index = 0;
  int pointer = 1;
  if(argc>4){
    index = 4;
    while(index<argc){
      printf("CURRENT = %s\n", argv[index]);
      argument_list[pointer] = argv[index];
      pointer++;
      index ++;
    }
    argument_list[index] = NULL;
  }else{
    argument_list[1] = NULL;
  }
  printf("COMMAND: %s \n", command_string);
  for (int i = 1; i< pointer; i++) {
    printf("%s\n", argument_list[i]);
  }

  strncpy(container.id, argv[1], CONTAINER_ID_MAX);
  strncpy(container.image, path,PATH_MAX);

  printf("IMAGE: %s \n", container.image);


  /* Use `clone` to create a child process */
  // char child_stack[CHILD_STACK_SIZE];  // statically allocate stack for child
  // int clone_flags = SIGCHLD | CLONE_NEWNS | CLONE_NEWPID;
  // int pid = clone(container_exec, &child_stack, clone_flags, &container);
  // if (pid < 0) {
  //   err(1, "Failed to clone");
  // }

  // waitpid(pid, NULL, 0);
  // return EXIT_SUCCESS;
}
