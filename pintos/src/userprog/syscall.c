#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <string.h>
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"

#define GET_MAX(a,b) (((a)>(b))?(a):(b))
static void syscall_handler (struct intr_frame *);

#define SYSCALL_DEBUG_MODE 0
#define HEXDUMP_DEBUG_MODE 0

struct semaphore filesys_mutex;

void is_valid_addr(const void* addr)	
{
	if(!is_user_vaddr(addr))
	{
		syscall_exit(-1);
	}
	return;
}

void
syscall_init (void) 
{
  sema_init(&filesys_mutex,1);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
 //printf ("system call!\n");
 if(HEXDUMP_DEBUG_MODE)
	hex_dump((int)f->esp, f->esp, 150,true);
 if(SYSCALL_DEBUG_MODE)
	printf("tid %d sys call number %d\n",thread_current()->tid,*(int*)(f->esp));
 uint32_t* arg = (uint32_t*)f->esp;


 uint32_t syscall_num = arg[0];

  switch(syscall_num)
  {
	  case SYS_HALT:
		  is_valid_addr(arg);
		  syscall_halt();
		  break;
	  case SYS_EXEC:
		  is_valid_addr(arg+1);
		  f->eax = syscall_exec((char*)arg[1]);
		  break;
	  case SYS_WAIT:
		  is_valid_addr(arg+1);
		  f->eax = syscall_wait((int)arg[1]);
		  break;
	case SYS_FIBONACCI:
		  is_valid_addr(arg+1);
		  f->eax = syscall_fibonacci((int)arg[1]);
		  break;
    case SYS_MAX_OF_FOUR_INT:
		  is_valid_addr(arg+4);
		  f->eax = max_of_four_int((int)arg[1],(int)arg[2],(int)arg[3],(int)arg[4]);
	case SYS_EXIT:
		  is_valid_addr(arg+1);
		  syscall_exit((int)arg[1]);
		  break;
	case SYS_CREATE:
		  is_valid_addr(arg+2);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_create((const char*)arg[1], (unsigned)arg[2]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_REMOVE:
		  is_valid_addr(arg+1);

		  sema_down(&filesys_mutex);
		  f->eax = syscall_remove((const char*)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_OPEN:
		  is_valid_addr(arg+1);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_open((const char*)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_CLOSE:
		  is_valid_addr(arg+1);
		  sema_down(&filesys_mutex);
		  syscall_close((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_WRITE:
		  is_valid_addr(arg+3);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_write((int)arg[1],(const void*)arg[2],(unsigned)arg[3]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_READ:
		  is_valid_addr(arg+3);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_read((int)arg[1],(void*)arg[2],(unsigned)arg[3]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_FILESIZE:
		  is_valid_addr(arg+1);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_filesize((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_SEEK:
		  is_valid_addr(arg+2);
		  sema_down(&filesys_mutex);
		  syscall_seek((int)arg[1], (unsigned)arg[2]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_TELL:
		  is_valid_addr(arg+1);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_tell((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	}
  if(SYSCALL_DEBUG_MODE || HEXDUMP_DEBUG_MODE)
	  printf("-----------------------------------\n");
}

void syscall_halt(void)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL READ\n");
	shutdown_power_off();
}

tid_t syscall_exec(char *cmd_line)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL EXEC %s\n",cmd_line);
	
	int ret = -1;
	tid_t tid = process_execute(cmd_line);

	if(tid == TID_ERROR)
		ret = -1;
	
	struct process_info* p = process_info_find(tid);
	if(p == NULL)
		ret = -1;
	sema_down(&p->load_sema);	
	if(p->load == 1)
		ret= tid;
	else if(p->load == -1)
	{
		ret= -1;
	}
	sema_up(&p->load_sema);

	return ret;
}

int syscall_wait(tid_t pid)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL WAIT %d\n",pid);
	
	return  process_wait(pid);
}


void syscall_exit(int status)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL EXIT %d\n",status);

	const char* name = thread_name();
	int i = 0;
	while(name[i] != ' ' && name[i] != '\0')
		i++;
	i++;
	if(i > 16)
		i = 16;
	char tmp_name[16];
	strlcpy(tmp_name,name,i);

	printf("%s: exit(%d)\n",tmp_name,status);
	process_exit_status(status);
}

int syscall_fibonacci(int n)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL PIBONACCI %d\n",n);

	int i;
	int fibo[3] = {0,1,};
	if(n <= 1)
		return fibo[n];
	for(i = 2 ; i <= n;i++)
	{
		fibo[i%3] = fibo[(i-1)%3] + fibo[(i-2)%3];
	}


	return fibo[(i-1)%3];
}

int syscall_sum_four(int a, int b, int c, int d)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL SUM FOUR %d %d %d %d\n",a,b,c,d);

	return a+b+c+d;
}

bool syscall_create(const char *file, unsigned initial_size)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL CREATE %s %d\n",file,initial_size);

	if(file == NULL)
	{
		syscall_exit(-1);
	}
	return filesys_create(file,initial_size);
}

bool syscall_remove(const char *file)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL REMOVE %s\n",file);
	if(file == NULL)
	{
		syscall_exit(-1);
	}

	return filesys_remove(file);
}

int syscall_open(const char *filename)
{
	static int fd = 2;
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL OPEN %s\n",filename);

	if(filename == NULL)
		return -1;

	struct file *fp = filesys_open(filename);
	if(fp == NULL)
		return -1;

	struct process_info *pinfo = process_info_find(thread_current()->tid);
	struct fd_info *pnew =(struct fd_info *)malloc(sizeof(struct fd_info));

	pnew->fp = fp;
	pnew->fd = fd++;
	list_push_back(&pinfo->file_list, &pnew->elem);

	return pnew->fd;
}

void syscall_close(int fd)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL CLOSE %d\n",fd);
	
	struct fd_info *finfo= fd_info_find(fd);
	if(!finfo) return;

	list_remove(&finfo->elem);
	file_close(finfo->fp);
	free(finfo);

}

int syscall_write(int fd, const void *buffer, unsigned size)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL WRITE %u %p %u\n",fd,buffer,size);
	
	is_valid_addr(buffer);

	if(fd == 1)
	{
		putbuf(buffer,size);
		return size;
	}
	struct fd_info *finfo = fd_info_find(fd);
	if(!finfo) return -1;

	struct file *fp = finfo->fp;
	
	int ret = file_write(fp, buffer,size);	

	return ret;
}

int syscall_read(int fd, void *buffer, unsigned size)
{
	if(SYSCALL_DEBUG_MODE )
		printf("SYS CALL READ %u %p %u\n",fd,buffer,size);

	is_valid_addr(buffer);
		
	if(fd == 0)
	{
		unsigned i;
		for(i = 0 ; i <size;i++)
		{
			*(char*)(buffer+i) = input_getc();
		}
		return size;
	}

	struct fd_info *finfo = fd_info_find(fd);
	if(!finfo) return -1;

	struct file *fp = finfo->fp;

	int ret = file_read(fp, buffer,size);

	return ret;

}

int syscall_filesize(int fd) {
	
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL FILESIZE %d\n",fd);

	struct fd_info *finfo = fd_info_find(fd);
	if(!finfo) return 0;
	struct file *file=finfo->fp;
	int size=(int)file_length(file);

	return size;
}
void syscall_seek(int fd, unsigned position) {
	
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL SEEK %d %d\n",fd,position);

	struct fd_info *finfo = fd_info_find(fd);
	if(!finfo) return;

	struct file *file=finfo->fp;

	file_seek(file, position);
}

unsigned syscall_tell(int fd) {
	
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL TELL %d\n",fd);

	struct fd_info *finfo = fd_info_find(fd);
	if(!finfo) return 0;
	struct file *file=finfo->fp;;

	unsigned position = (unsigned)file_tell(file);
	return position;
}

int max_of_four_int(int a, int b, int c, int d){
  int temp = GET_MAX(a,b);
  temp = GET_MAX(temp, c);
  temp = GET_MAX(temp, d);
  return temp;
}