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
#include "vm/page.h"

#define GET_MAX(a,b) (((a)>(b))?(a):(b))
static void syscall_handler (struct intr_frame *);
void check_address(void *addr, void *esp);
void check_valid_buffer(void *buffer, unsigned size, void *esp, bool to_write);
void check_valid_string(const void *str, void *esp);

#define SYSCALL_DEBUG_MODE 0
#define HEXDUMP_DEBUG_MODE 0

struct semaphore filesys_mutex;

void is_valid_addr(const void* addr)	
{
	if(!is_user_vaddr(addr))
	{
		syscall_exit(-1, "is_valid_addr");
	}
	return;
}

void check_address(void *addr, void *esp)
{
	struct vm_entry *vme;
	/* if address is user_address */
	if(is_user_vaddr(addr))
	{
		/* find vm_entry if can't find vm_entry, exit the process */
		vme = find_vme(addr);
		/* if can't find vm_entry */
		if(vme == NULL)
			syscall_exit(-1, "check_addr vme not found");
	}
	else
		syscall_exit(-1, "check_addr not user space");
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
  void *esp = f->esp;
  check_address(esp, f->esp);
  uint32_t* arg = (uint32_t*)f->esp;
  uint32_t syscall_num = arg[0];
  //printf("[%d] syscall_num %d\n", thread_current()->tid, syscall_num);
  switch(syscall_num)
  {
	  case SYS_HALT:
	  	  check_address(arg, esp);
		  syscall_halt();
		  break;
	  case SYS_EXEC:
	  	  check_address(arg+1, esp);
	  	  check_valid_string((const void*)arg[1], f->esp);
		  f->eax = syscall_exec((char*)arg[1]);
		  break;
	  case SYS_WAIT:
	  	  check_address(arg+1, esp);
		  f->eax = syscall_wait((int)arg[1]);
		  break;
	case SYS_FIBONACCI:
		  check_address(arg+1, esp);
		  f->eax = syscall_fibonacci((int)arg[1]);
		  break;
    case SYS_MAX_OF_FOUR_INT:
		  check_address(arg+4, esp);
		  f->eax = max_of_four_int((int)arg[1],(int)arg[2],(int)arg[3],(int)arg[4]);
		  break;
	case SYS_EXIT:
		  check_address(arg+1, esp);
		  syscall_exit((int)arg[1], "syscall exit called");
		  break;
	case SYS_CREATE:
		  check_address(arg+2, esp);
		  check_valid_string((const void*)arg[1], f->esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_create((const char*)arg[1], (unsigned)arg[2]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_REMOVE:
		  check_address(arg+1, esp);
		  check_valid_string((const void*)arg[1], f->esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_remove((const char*)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_OPEN:
		  check_address(arg+1, esp);
		  check_valid_string((const void*)arg[1], f->esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_open((const char*)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_CLOSE:
		  check_address(arg+1, esp);
		  sema_down(&filesys_mutex);
		  syscall_close((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_WRITE:
		  check_address(arg+3, esp);
		  check_valid_buffer((void*)arg[2], (unsigned)arg[3], f->esp, false);
		  //check_valid_string((void*)arg[2],f->esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_write((int)arg[1],(const void*)arg[2],(unsigned)arg[3]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_READ:
		  check_address(arg+3, esp);
		  check_valid_buffer((void*)arg[2], (unsigned)arg[3], f->esp, true);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_read((int)arg[1],(void*)arg[2],(unsigned)arg[3]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_FILESIZE:
		  check_address(arg+1, esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_filesize((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_SEEK:
		  check_address(arg+2, esp);
		  sema_down(&filesys_mutex);
		  syscall_seek((int)arg[1], (unsigned)arg[2]);
		  sema_up(&filesys_mutex);
		  break;
	case SYS_TELL:
		  check_address(arg+1, esp);
		  sema_down(&filesys_mutex);
		  f->eax = syscall_tell((int)arg[1]);
		  sema_up(&filesys_mutex);
		  break;
	}
}

void check_valid_buffer(void *buffer, unsigned size, void *esp, bool to_write)
{
	struct vm_entry *vme;
	unsigned i;
	char *check_buffer = (char *)buffer;
	/* check buffer */
	for(i=0; i<size; i++)
	{
		//printf("Check valid buffer %u\n", check_buffer);
		check_address((void *)check_buffer, esp);
		vme = find_vme((void *)check_buffer);
		if(vme != NULL)
		{
			/* if to_write is true, vm_entry must writable.
			   so if to_write is true but vm_entry is not writable
			   exit the process. */
			if(to_write == true)
			{
				if(vme->writable == false)
					syscall_exit(-1, "check_valid_buffer");
			}
		}
		check_buffer++;
	}
}
void check_valid_string(const void *str, void *esp)
{
	char *check_str = (char *)str;
	check_address((void *)check_str,esp);
	/* check the all string's address */
	while(*check_str != 0)
	{
		check_str += 1;
		check_address(check_str, esp);
	}
}


void syscall_halt(void)
{
	shutdown_power_off();
}

tid_t syscall_exec(char *cmd_line)
{
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
	return  process_wait(pid);
}


void syscall_exit(int status, char* source)
{
	//printf("SYS CALL EXIT %d [%s]\n",status, source);

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

bool syscall_create(const char *file, unsigned initial_size)
{
	if(file == NULL)
	{
		syscall_exit(-1, "syscall_create");
	}
	//printf("file create %s %d\n",file, initial_size);
	return filesys_create(file,initial_size);
}

bool syscall_remove(const char *file)
{
	if(SYSCALL_DEBUG_MODE)
		printf("SYS CALL REMOVE %s\n",file);
	if(file == NULL)
	{
		syscall_exit(-1, "syscall remove");
	}

	return filesys_remove(file);
}

int syscall_open(const char *filename)
{
	static int fd = 2;
	if(filename == NULL)
		return -1;

	struct file *fp = filesys_open(filename);
	if(fp == NULL){
		//printf("fp null\n");
		return -1;
	}

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
	if(!finfo){
		return -1;
	}

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