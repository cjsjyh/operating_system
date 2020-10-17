#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
void check_addr(void*);

void get_argument(void*, void* [], int);
void exit(int);
void write(int, const char*, int);
pid_t exec(const char*);
int wait(pid_t);
int read(int, void*, size_t);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_addr (void* addr){
  if (!is_user_vaddr(addr)){
    exit(-1);
  }
  return;
}


static void
syscall_handler (struct intr_frame *f) 
{
  uint32_t* esp;
  void* args[4];
  void* buffer;
  int syscall_type;
  
  // Get stack pointer from interrupt frame
  esp = (uint32_t*)f->esp;
  //printf("[ESP: %p]\n", esp);

  // Get system call number form stack
  syscall_type = *(uint32_t*)esp;
  //printf("[ System Call #%d ]\n", syscall_type);
  switch (syscall_type){
    case SYS_HALT:
      // 0 args
      check_addr(esp);
      halt();
      break;
    case SYS_EXIT:
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      exit(*(int*)args[0]);
      break;
    case SYS_EXEC:
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      exec(args[0]);
      break;
    case SYS_WAIT:
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      wait(args[0]);
      break;
    case SYS_READ:
      // 3 args
      get_argument(esp, args, 3);
      check_addr(args[2]);
      read(*(int*)args[0], *(uint32_t*)args[1], *(int*)args[2]);
      break;
    case SYS_WRITE:
      // 3 args
      get_argument(esp, args, 3);
      check_addr(args[2]);
      write(*(int*)args[0], (const char*)(*(uint32_t*)args[1]), *(int*)args[2]);
      break;
    default:
      thread_exit();
  }

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
  printf("%s: exit(%d)\n",name, status);
  thread_exit();
}

// Terminate Pintos
void halt (){
  shutdown_power_off();
}

// Create child process (refer to process_execute() in userprog/process.c )
pid_t exec(const char *cmd_line){
  return process_execute(cmd_line);
}

// Wait child process until it finishes its work
int wait(pid_t pid){
  process_wait(pid);
  // Check that child thread ID is valid
  
  // Get the exit status from child thread when the child thread is dead
  
  // To prevent termination of process before return from wait(), 
  // you can use busy waiting technique or thread_yield() in threads/thread.c

}

// STDOUT
void write(int fd, const char* buffer, int size){
  //write to console
  if (fd == 1){
    putbuf(buffer, size);
    return size;
  }
}

// STDIN
int read(int fd, void* buffer, size_t size){
  int i;
  if (fd == 0){
    for (i=0; i<size; i++){
      if ( ((char*)buffer)[i] == '\0' )
        break;
    }
  }
  i = 0;
}

/*
enum 
  {
    // Projects 2 and later. 
    SYS_HALT,                   // Halt the operating system. 
    SYS_EXIT,                   // Terminate this process. 
    SYS_EXEC,                   // Start another process. 
    SYS_WAIT,                   // Wait for a child process to die. 
    SYS_CREATE,                 // Create a file. 
    SYS_REMOVE,                 // Delete a file. 
    SYS_OPEN,                   // Open a file. 
    SYS_FILESIZE,               // Obtain a file's size. 
    SYS_READ,                   // Read from a file. 
    SYS_WRITE,                  // Write to a file. 
    SYS_SEEK,                   // Change position in a file. 
    SYS_TELL,                   // Report current position in a file. 
    SYS_CLOSE,                  // Close a file. 

    // Project 3 and optionally project 4. 
    SYS_MMAP,                   // Map a file into memory. 
    SYS_MUNMAP,                 // Remove a memory mapping. 

    // Project 4 only. 
    SYS_CHDIR,                  // Change the current directory. 
    SYS_MKDIR,                  // Create a directory. 
    SYS_READDIR,                // Reads a directory entry. 
    SYS_ISDIR,                  // Tests if a fd represents a directory. 
    SYS_INUMBER                 // Returns the inode number for a fd. 
  };
*/
