#include "keylogger.h"
#include <signal.h>

void kill_daemon(){
  FILE* pid_fp = fopen(pid_file, "r");
  if(pid_fp == NULL){
    handle_error(stderr, 11);
  }
  pid_t pid;
  fscanf(pid_fp, "%d", &pid);
  fclose(pid_fp);
  if(pid <= 0){
    handle_error(stderr, 11);
  }
  if(kill(pid, SIGINT)){
    handle_error(stderr, 11);
  }
}