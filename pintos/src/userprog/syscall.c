#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
static int check_address(void *);
void get_argument(void*, void* [], int);
void exit(int);
void write(int, uint32_t*, int);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  void* esp;
  void* args[4];
  int syscall_type;
  
  // Get stack pointer from interrupt frame
  esp = f->esp;
  if (check_address(esp) == 0){
    printf("--- [ System Call Invalid esp ] ---\n");
    exit(-1);
  }
  // Get system call number form stack
  syscall_type = *(uint32_t *)esp;
  printf("--- [ System Call #%d ] ---\n", syscall_type);
  switch (syscall_type){
    case SYS_HALT:
      // 0 args

      break;
    case SYS_EXIT:
      // 1 args

      break;
    case SYS_EXEC:
      // 1 args

      break;
    case SYS_WAIT:
      // 1 args

      break;
    case SYS_READ:
      // 3 args

      break;
    case SYS_WRITE:
      // 3args
      get_argument(esp, args, 3);
      write(*(int*)args[0], *(uint32_t*)args[1], *(int*)args[2]);
      break;
    default:
      thread_exit();
  }

  thread_exit();
}

static int check_address(void *addr){
  if (addr < 0x8048000 || addr > 0xc0000000)
    return 0;
  return 1;
}

void get_argument(void *esp, void* args[], int count){
  //skip syscall type
  void* temp = esp + sizeof(uint32_t*);
  for(int i=0; i<count; i++){
    args[i] = temp;
    temp += sizeof(uint32_t*);
  }
}

// Terminate the current user program, returning status to the kernel
void exit (int status){
  const char* name = thread_name();
  printf("[Exiting...]\n");
  thread_exit();
}

// Terminate Pintos
void halt (){
  shutdown_power_off();
}

// Create child process (refer to process_execute() in userprog/process.c )
void exec(){

}

// Wait child process until it finishes its work
void wait(){
  // Check that child thread ID is valid
  
  // Get the exit status from child thread when the child thread is dead
  
  // To prevent termination of process before return from wait(), 
  // you can use busy waiting technique or thread_yield() in threads/thread.c

}

// STDOUT
void write(int fd, uint32_t* buffer, int size){
  //write to console
  if (fd == 1){
    putbuf(buffer, size);
    return size;
  }
}

// STDIN
void read(){

}
