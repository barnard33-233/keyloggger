/*
  linux keyloggger
  @author Liu Mohan

  parse arguments passed from command line
  arguments:
  0. -s/--start
  1. -o/--output output_file_name
  2. -n/--no-daemon
  3. -k/--kill
  4. -d/--device file
  5. -h/--help
*/
#include <getopt.h>
#include "keylogger.h"

struct option long_options[] = {
  {"start", no_argument, NULL, 's'},
  {"kill", no_argument, NULL, 'k'},
  {"help", no_argument, NULL, 'h'},
  {"output", required_argument, NULL, 'o'},
  {"device", required_argument, NULL, 'd'},
  {"no-daemon", no_argument, NULL, 'n'},
  {0,0,0,0}
};

char * options = "skho:d:n";

arguments parse_args(int argc, char ** argv){
  arguments args = {
    .operation = NONE,
    .arg_log = NULL,
    .arg_dev = NULL,
    .arg_daemon_flag = 0
  };
  int parse_res;
  int index;
  while(EOF != (parse_res = getopt_long(argc, argv, options, long_options, &index))){

    switch (parse_res){

    case 's':
      if(args.operation != NONE){
        handle_error(stderr, 1);
      }
      args.operation = START;
      break;

    case 'k':
      if(args.operation != NONE){
        handle_error(stderr, 2);
      }
      args.operation = KILL;
      break;

    case 'h':
      if(args.operation != NONE){
        handle_error(stderr, 2);
      }
      args.operation = HELP;
      break;

    case 'o':
      args.arg_log = malloc(strlen(optarg) + 1);
      strcpy(args.arg_log, optarg);
      break;

    case 'd':
      args.arg_dev = malloc(strlen(optarg) + 1);
      strcpy(args.arg_dev, optarg);
      break;

    case 'n':
      args.arg_daemon_flag = 1;
      break;

    case '?':
      handle_error(stderr, 3);
      break;

    case ':':
      handle_error(stderr, 4);
      break;

    default:break;
    }
  }

  if(args.operation == NONE){
    handle_error(stderr, 1);
  }
  if(args.operation == START && args.arg_dev == NULL){
    handle_error(stderr, 9);
  }

  return args;
}