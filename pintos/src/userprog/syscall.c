#include "userprog/syscall.h"
#include "lib/user/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "process.h"
#include "threads/vaddr.h"

static struct lock syscall_lock;
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	lock_init (&syscall_lock);
}

static void
syscall_handler (struct intr_frame *f) 
{

int syscall_num = *(int*)f->esp;
int arg_1 = *((int*)f->esp + 1);
int arg_2 = *((int*)f->esp + 2);
int arg_3 = *((int*)f->esp + 3);

switch( syscall_num )
{
	case SYS_HALT:
		halt();
		break;

	case SYS_EXIT:
		exit( arg_1 );
		break;

	case SYS_EXEC:
		f->eax = exec( (char*)arg_1 );
		break;

	case SYS_WAIT:
		wait( (pid_t)arg_1 );
		break;

	case SYS_CREATE:
		f->eax = create( (char*)arg_1, (unsigned)arg_2 );
		break;

	case SYS_REMOVE:
		f->eax = remove( (char*)arg_1 );
		break;

	case SYS_OPEN:
		f->eax = open( (char*)arg_1 );
		break;

	case SYS_FILESIZE:
		f->eax = filesize( arg_1 );
		break;

	case SYS_READ:
		f->eax = read( arg_1, (void*)arg_2, (unsigned)arg_3 );
		break;

	case SYS_WRITE:
		f->eax = write( arg_1, (void*)arg_2, (unsigned)arg_3 );
		break;

	case SYS_SEEK:
		seek( arg_1, arg_2 );
		break;

	case SYS_TELL:
		f->eax = tell( arg_1 );
		break;

	case SYS_CLOSE:
		close( arg_1 );
		break;

	default:
		break;

}

}

/* Halt the operating system. */
void 
halt (void)
{
	shutdown_power_off();
}

/* Terminate this process. */
void 
exit (int status)
{
	lock_acquire (&syscall_lock);
	thread_exit();
	process_exit();
	lock_release (&syscall_lock);
}

/* Start another process. */
pid_t 
exec (const char *cmd_line)
{
	tid_t tid;
	lock_acquire (&syscall_lock);
	tid = process_execute (cmd_line);
	lock_release (&syscall_lock);
	return (pid_t)tid;
}

/* Wait for a child process to die. */
int 
wait (pid_t pid)
{
	//lock_acquire (&syscall_lock);
	//suspend parents and wait for child to finish
  	//lock_release (&syscall_lock);
	//return 0;
	int temp;
	lock_acquire (&syscall_lock);
	temp = process_wait(pid);
	lock_release (&syscall_lock);
	return temp;
}

/* Create a file. */
bool 
create (const char *file, unsigned initial_size)
{
	bool created;
	if (file == NULL)
		exit(-1); //TW 10.12	
	lock_acquire (&syscall_lock);
  created = filesys_create(file, (off_t)initial_size);
  lock_release (&syscall_lock);
	return created;
}

/* Delete a file. */
bool 
remove (const char *file)
{
	bool removed;
	if (file == NULL)
		exit(-1); //TW 10.12
	lock_acquire (&syscall_lock);
  removed = filesys_remove(file);
  lock_release (&syscall_lock);
	return removed;
}

/* Open a file. */
int 
open (const char *file)
{
	int rtn;
	lock_acquire (&syscall_lock);
	rtn = filesys_open(file);
	lock_release (&syscall_lock);
	return rtn;
}

/* Obtain a file's size. */
int 
filesize (int fd)
{
	int size;
	lock_acquire (&syscall_lock);
	size = file_length(fd);
	lock_release (&syscall_lock);
	return size;
}

/* Read from a file. */
int 
read (int fd, void *buffer, unsigned size)
{
	int bytes_read;
	lock_acquire (&syscall_lock);
	bytes_read = (int)file_read(fd, buffer, size);
	lock_release (&syscall_lock);
	return bytes_read;
}

/* Write to a file. */
int 
write (int fd, const void *buffer, unsigned size)
{
	int bytes_written;
	lock_acquire (&syscall_lock);
	bytes_written = (int)file_write(fd, buffer, size);
	lock_release (&syscall_lock);
	return bytes_written;
}

/* Change position in a file. */
void 
seek (int fd, unsigned position)
{
	if (fd == NULL)
		exit(-1); //TW 10.12
	lock_acquire (&syscall_lock);
	file_seek(fd, position);
	lock_release (&syscall_lock);
}

/* Report current position in a file. */
unsigned 
tell (int fd)
{
	if (fd == NULL)
		exit(-1); //TW 10.12	
	unsigned pos;
	lock_acquire (&syscall_lock);
	pos = (unsigned)file_tell(fd);
	lock_release (&syscall_lock);
	return pos;
}

/* Close a file. */
void 
close (int fd)
{
	if (fd == NULL)
		exit(-1);//TW 10.12 
	lock_acquire (&syscall_lock);
	file_close(fd);
	//free(fd); freeing file is handled in file_close
	lock_release (&syscall_lock);
}

