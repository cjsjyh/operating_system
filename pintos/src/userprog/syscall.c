#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
static int check_address(void *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  void* esp;
  int sys_call_num;
  printf ("system call!\n");
  /*
  // Get stack pointer from interrupt frame
  esp = f->esp;
  if (check_address(esp) == 0)
    exit(-1);
  // Get system call number form stack
  sys_call_num = *(int*)esp;
  printf("SYS_CALL_NUM: %d\n", sys_call_num);
  switch (sys_call_num){
    case SYS_HALT:

      break;
    case SYS_EXIT:

      break;
    case SYS_EXEC:

      break;
    case SYS_WAIT:

      break;
    case SYS_READ:

      break;
    case SYS_WRITE:

      break;
    default:
      thread_exit();
  }
  */
  thread_exit();
}

static int check_address(void *addr){
  if (addr < 0x8048000 || addr > 0xc0000000)
    return 0;
  return 1;
}
