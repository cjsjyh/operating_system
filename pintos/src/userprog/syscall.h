#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/thread.h"
void syscall_init (void);

extern struct semaphore filesys_mutex;

void is_valid_addr(const void* addr);
void syscall_halt(void);
tid_t syscall_exec(char *cmd_line);
int syscall_write(int fd, const void *buffer, unsigned size);
int  syscall_read(int fd, void *buffer, unsigned size);
int syscall_wait(tid_t pid);
void syscall_exit(int status, char*);
int syscall_fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);
bool syscall_create(const char *file, unsigned initial_size);
bool syscall_remove(const char *file);
int syscall_open(const char *filename);
void syscall_close(int fd);
int syscall_filesize(int fd);
void syscall_seek(int fd, unsigned position);
unsigned syscall_tell(int fd);
#endif /* userprog/syscall.h */
