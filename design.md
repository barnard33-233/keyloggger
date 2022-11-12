# keylogger document

## args

0. -s/--start
1. -o/--output output_file_name
2. -n/--no-daemon
3. -k/--kill
4. -d/--device file
5. -h/--help

> usage: keyloggger -s | -k | -h \[-o log\]\[-d device\]\[-n\]
> > keyloggger -s \[-o log\]\[-d device\]\[-n\]
> >
> > keyloggger -h
> >
> > keyloggger -k


## Error information
### args

1. Lack of necessary option
2. Option conflict
3. Unknown option
4. Missing argument
5. Start: daemon already running
6. Kill: daemon not running
7. Output: unable to open log file
8. Device: unable to open event
9. Device: not a keyboard
10. Start: Fail
11. Kill: Fail

## Source File

1. `args.c`: argument parser and error handler.
2. `keylogger.c`: main
3. `start.c`: start daemon
4. `kill.c`: kill daemon
5. `mapping.c`: keymapping arrays

## Working Process

1. start_daemon: 
   
   open log --> find event -->
   
   create daemon --> start polling

2. kill_daemon:

   send SIGINT to daemon -->
   
   enter sigint handler --> close event --> release allocation --> close log --> kill daemon

## About Daemon

### Create

1. fork 1
   
   the parent should exit after the child exit, to avoid creating a zombie process.
2. setsid 

   if the shell exit, this process will not exit.
3. fork 2
   
   exit parent(the child must be a orphan instead of a zombie now).

4. close unused streams, change IO permission

## To develop
1. Add the function of specify keyboard mapping