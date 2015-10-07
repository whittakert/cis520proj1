#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

// max number of arguments that can be sent to a program
#define MAXARGS 50 

#endif /* userprog/process.h */
