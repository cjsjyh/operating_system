#include <stdio.h>
#include <syscall-nr.h>

#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"

#include "filesys/file.h"
#include "filesys/filesys.h"

#include "userprog/syscall.h"
#include "userprog/process.h"

#define GET_MAX(a,b) (((a)>(b))?(a):(b))

static void syscall_handler (struct intr_frame *);
void check_addr(void*);

void get_argument(void*, void* [], int);
void exit(int);
void halt();
int write(int, const char*, int);
pid_t exec(const char*);
int wait(pid_t);
int read(int, uint32_t*, size_t);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);

bool _create(const char *file, int initial_size);
bool _remove(const char *file);
int _open(const char* file);
int _filesize(int fd);
void _seek(int fd, int position);
int _tell(int fd);
void _close(int fd);

static int debug_mode = false;
struct semaphore file_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  sema_init(&file_lock, 1);
}

void check_addr (void* addr){
  if (!is_user_vaddr(addr)){
    if(debug_mode) printf("check_addr FAILED\n");
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

  // Get system call number form stack
  syscall_type = *(uint32_t*)esp;
  if(debug_mode && syscall_type != 6 && syscall_type != 9) printf("[ %s - %d : System Call - #%d ]\n", thread_current()->name, thread_current()->tid, syscall_type);
  //if(debug_mode) hex_dump(f->esp, f->esp, 100, 1);
  switch (syscall_type){
    case SYS_HALT: // #0
      // 0 args
      check_addr(esp);
      halt();
      break;
    case SYS_EXIT: // #1
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      exit(*(int*)args[0]);
      break;
    case SYS_EXEC: // #2
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = exec(*((uint32_t*)args[0]));
      break;
    case SYS_WAIT: // #3
      // 1 args
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = wait(*(int*)args[0]);
      break;
    case SYS_READ: // #8
      // 3 args
      get_argument(esp, args, 3);
      check_addr(args[2]);
      f->eax = read(*(int*)args[0], *(uint32_t*)args[1], *(int*)args[2]);
      break;
    case SYS_WRITE: // #9
      // 3 args
      get_argument(esp, args, 3);
      check_addr(args[2]);
      f->eax = write(*(int*)args[0], (const char*)(*(uint32_t*)args[1]), *(int*)args[2]);
      break;
    case SYS_FIBONACCI:
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = fibonacci(*(int*)args[0]);
      break;
    case SYS_MAX_OF_FOUR_INT:
      get_argument(esp, args, 4);
      check_addr(args[3]);
      f->eax = max_of_four_int(*(int*)args[0],*(int*)args[1],*(int*)args[2],*(int*)args[3]);
      break;
    case SYS_CREATE:
      get_argument(esp, args, 2);
      check_addr(args[1]);
      f->eax = _create((const char*)*(uint32_t*)args[0], *(int*)args[1]);
      break;
    case SYS_REMOVE:
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = _remove((const char*)*(uint32_t*)args[0]);
      break;
    case SYS_OPEN: // 6
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = _open((const char*)*(uint32_t*)args[0]);
      break;
    case SYS_FILESIZE:
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = _filesize(*(int*)args[0]);
      break;
    case SYS_SEEK:
      get_argument(esp, args, 2);
      check_addr(args[1]);
      _seek(*(int*)args[0], *(int*)args[1]);
      break;
    case SYS_TELL:
      get_argument(esp, args, 1);
      check_addr(args[0]);
      f->eax = _tell(*(int*)args[0]);
      break;
    case SYS_CLOSE: // 12
      get_argument(esp, args, 1);
      check_addr(args[0]);
      _close(*(int*)args[0]);
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

bool _create(const char *file, int initial_size){
  if (file == NULL) exit(-1);
  else if (!strcmp(file, "")) return false;
  return filesys_create(file, initial_size);
}
bool _remove(const char *file){
  if (file == NULL) exit(-1);
  return filesys_remove(file);
}
int _open(const char* file){
  if (file == NULL) exit(-1);
  else if (!strcmp(file, "")) return -1;

  sema_down(&file_lock);
  struct file* temp = filesys_open(file);
  if (temp == NULL) {
    sema_up(&file_lock);
    return -1;
  }
  if (!strcmp(thread_current()->name, file))
    file_deny_write(temp);
  struct thread_fd* t_fd = find_thread_fd();
  t_fd->fd[t_fd->fd_cnt] = temp;
  t_fd->fd_cnt++;
  sema_up(&file_lock);
  return t_fd->fd_cnt - 1;
}
int _filesize(int fd){
  struct thread_fd* t_fd = find_thread_fd();
  if (fd >= t_fd->fd_cnt) exit(-1);
  return file_length(t_fd->fd[fd]);
}
void _seek(int fd, int position){
  struct thread_fd* t_fd = find_thread_fd();
  if (fd >= t_fd->fd_cnt) exit(-1);
  file_seek(t_fd->fd[fd], position);
}
int _tell(int fd){
  struct thread_fd* t_fd = find_thread_fd();
  if (fd >= t_fd->fd_cnt) exit(-1);
  return file_tell(t_fd->fd[fd]);
}
void _close(int fd){
  struct thread_fd* t_fd = find_thread_fd();
  if (fd >= t_fd->fd_cnt) exit(-1);
  if (t_fd->fd[fd] == NULL) exit(-1);
  sema_down(&file_lock);
  file_close(t_fd->fd[fd]);
  remove_fd(fd);
  sema_up(&file_lock);
}

// Terminate the current user program, returning status to the kernel
void exit (int status){
  // Release semaphore if this thread is holding it
  sema_up(&file_lock);

  // Close all file descriptors
  struct thread_fd* t_fd = find_thread_fd();
  for(int i=3; i<t_fd->fd_cnt; i++)
    _close(i);

  pop_thread_fd();

  const char* name = thread_name();
  if(debug_mode)
    printf("%d- %s: exit(%d)\n", thread_current()->tid,name, status);
  else
    printf("%s: exit(%d)\n",name, status);
  thread_current()->exit_status = status;
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
  int exit_code = process_wait(pid);
  return exit_code;
}

// STDOUT
int write(int fd, const char* buffer, int size){
  int result;
  if (buffer == NULL)
    exit(-1);
  check_addr(buffer);
  sema_down(&file_lock); 
  //write to console
  if (fd == 1){
    putbuf(buffer, size);
    result = size;
  } else if(fd > 2){
    struct thread_fd* t_fd = find_thread_fd();
    if (fd >= t_fd->fd_cnt){ 
      sema_up(&file_lock);
      exit(-1);
    }
    if (t_fd->fd[fd]->deny_write)
      file_deny_write(t_fd->fd[fd]);
    result = file_write(t_fd->fd[fd], buffer, size);
  }
  sema_up(&file_lock);
  return result;
}

// STDIN
int read(int fd, uint32_t* buffer, size_t size){
  int result;
  if (buffer == NULL) exit(-1);
  check_addr(buffer);
  sema_down(&file_lock);
  if (fd == 0){
    for (int i=0; i<size; i++){
      if ( ((char*)buffer)[i] == '\0' ){
        result = i;
        break;
      }
    }
  } else if(fd > 2){
    struct thread_fd* t_fd = find_thread_fd();
    if (fd >= t_fd->fd_cnt){ 
      sema_up(&file_lock);
      exit(-1);
    }
    result = file_read(t_fd->fd[fd], buffer, size);
  }
  sema_up(&file_lock);
  return result;
}

int fibonacci(int n){
  int result = 1, last = 0, temp;
  if (n==0) return 0;

  for(int i=1; i<n; i++){
    temp = result;
    result += last;
    last = temp;
  }
  return result;
}

int max_of_four_int(int a, int b, int c, int d){
  int temp = GET_MAX(a,b);
  temp = GET_MAX(temp, c);
  temp = GET_MAX(temp, d);
  return temp;
}

/*
enum 
  {
    // Projects 2 and later. 
    0 SYS_HALT,                   // Halt the operating system. 
    1 SYS_EXIT,                   // Terminate this process. 
    2 SYS_EXEC,                   // Start another process. 
    3 SYS_WAIT,                   // Wait for a child process to die. 
    4 SYS_CREATE,                 // Create a file. 
    5 SYS_REMOVE,                 // Delete a file. 
    6 SYS_OPEN,                   // Open a file. 
    7 SYS_FILESIZE,               // Obtain a file's size. 
    8 SYS_READ,                   // Read from a file. 
    9 SYS_WRITE,                  // Write to a file. 
    10 SYS_SEEK,                   // Change position in a file. 
    11 SYS_TELL,                   // Report current position in a file. 
    12 SYS_CLOSE,                  // Close a file. 

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
