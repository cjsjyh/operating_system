#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>

#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "userprog/syscall.h"
#include "vm/page.h"

#define WAIT_DEBUG_MODE 0
#define ARG_DEBUG_MODE 0

static thread_func start_process NO_RETURN;
static bool load (const char *cmdline, void (**eip) (void), void **esp);
void argument_passing(char *file_name, void **esp, int argc, int arg_len)
{
	
	if(ARG_DEBUG_MODE)
		printf("--------------ARGUMENT PASSING----------------\n");
	char **argv;
	int *argl;
	void *tmp_esp = *esp;

	int word_size = 4;
	int word_align = (word_size - (arg_len) % word_size) % word_size;
	//word_align:

	int i = 0;

	//parsing in reverse order and store in argv, argl
	argv = (char**)malloc(sizeof(char*) * argc);
	argl = (int*)malloc(sizeof(int) * argc);

	char *parse = file_name;
	
	for(i = argc - 1; i>=0 ;i-- )
	{
		argl[i] = strlen(parse) + 1;
		argv[i] = parse;
		memcpy(argv[i],parse,argl[i]);
		parse += argl[i];
	}

	//push str data in stack
	for(i = 0;i<argc;i++)
	{
		*esp -= argl[i];
		memcpy(*esp, argv[i], argl[i]);
	}

	//push word align
	*esp -= word_align;
	memset(*esp, 0, word_align);

	//push null pointer
	*esp -= word_size;
	memset(*esp, 0 , word_size);

	//push argument address
	for(i = 0 ; i < argc;i++)
	{
		*esp -= word_size;
		tmp_esp -= argl[i];
		memcpy(*esp,&tmp_esp,4);
	}
	//push argument address start point
	*esp -= word_size;
	*((int*)*esp) = (int)*esp + 4;

	//push argument count
	*esp -= word_size;
	memcpy(*esp,&argc,4);
	//push return address
	*esp -= word_size;
	memset(*esp,0,4);

	free(argv);
	free(argl);

	if(ARG_DEBUG_MODE)
	{
		hex_dump((int)*esp,*esp ,150,true);
		printf("-------------------------------------------\n");
	}
}

/* Starts a new thread running a user program loaded from
   FILENAME.  The new thread may be scheduled (and may even exit)
   before process_execute() returns.  Returns the new process's
   thread id, or TID_ERROR if the thread cannot be created. */
tid_t
process_execute (const char *file_name) 
{
  char *fn_copy;
  tid_t tid;

  /* Make a copy of FILE_NAME.
     Otherwise there's a race between the caller and load(). */
  fn_copy = palloc_get_page (0);
  if (fn_copy == NULL)
    return TID_ERROR;
  strlcpy (fn_copy, file_name, PGSIZE);

  /* Create a new thread to execute FILE_NAME. */
  tid = thread_create (file_name, PRI_DEFAULT, start_process, fn_copy);

  if (tid == TID_ERROR)
    palloc_free_page (fn_copy); 
  
 
  return tid;
}

/* A thread function that loads a user process and starts it
   running. */
static void
start_process (void *file_name_)
{
  char *file_name = file_name_;
  struct intr_frame if_;
  bool success;

  vm_init(&thread_current()->vm);

  /* Initialize interrupt frame and load executable. */
  memset (&if_, 0, sizeof if_);
  if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
  if_.cs = SEL_UCSEG;
  if_.eflags = FLAG_IF | FLAG_MBS;
  success = load (file_name, &if_.eip, &if_.esp);

  /* If load failed, quit. */
  palloc_free_page (file_name);

  struct process_info *p = process_info_find(thread_current()->tid);

  if (!success)
	  p->load = -1;
  else
  	  p->load = 1;
  sema_up(&p->load_sema);

  if (!success)
	  process_exit_status(-1);
  /* Start the user process by simulating a return from an
     interrupt, implemented by intr_exit (in
     threads/intr-stubs.S).  Because intr_exit takes all of its
     arguments on the stack in the form of a `struct intr_frame',
     we just point the stack pointer (%esp) to our stack frame
     and jump to it. */
  asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (&if_) : "memory");
  NOT_REACHED ();
}

/* Waits for thread TID to die and returns its exit status.  If
   it was terminated by the kernel (i.e. killed due to an
   exception), returns -1.  If TID is invalid or if it was not a
   child of the calling process, or if process_wait() has already
   been successfully called for the given TID, returns -1
   immediately, without waiting.

   This function will be implemented in problem 2-2.  For now, it
   does nothing. */
int
process_wait (tid_t child_tid) 
{
  if(WAIT_DEBUG_MODE)
  	printf("JAEHOON PROCESS WAIT CHILD %d\n",child_tid);

  if(child_tid <= 0)
	  return -1;

  struct process_info *pinfo = process_info_find(child_tid);

  if(pinfo==NULL)
  {
	  return -1;
  }
  sema_down(&pinfo->sema);

  int ret = pinfo->status;
  //pinfo->tid = -1;
 
  sema_down(&delete_sema);
  list_remove(&pinfo->elem);
  free(pinfo);

  sema_up(&delete_sema);
  
  return ret;

}

/* Free the current process's resources. */
void
process_exit (void)
{
  struct thread *cur = thread_current ();
  uint32_t *pd;


  file_close(cur->executable_file);
  struct process_info *pinfo = process_info_find(thread_current()->tid);

  if(pinfo)
  {
	 while(!list_empty(&pinfo->file_list))
	 {
		 struct list_elem *e = list_begin(&pinfo->file_list);
		 struct fd_info *finfo = list_entry(e,struct fd_info, elem);
		 list_remove(e);

		 file_close(finfo->fp);
		 free(finfo);
	 }
	  sema_up(&pinfo->sema);
  }

  vm_destroy(&cur->vm);
  /* Destroy the current process's page directory and switch back
     to the kernel-only page directory. */
  pd = cur->pagedir;
  if (pd != NULL) 
    {
      /* Correct ordering here is crucial.  We must set
         cur->pagedir to NULL before switching page directories,
         so that a timer interrupt can't switch back to the
         process page directory.  We must activate the base page
         directory before destroying the process's page
         directory, or our active page directory will be one
         that's been freed (and cleared). */
      cur->pagedir = NULL;	
      pagedir_activate (NULL);
      pagedir_destroy (pd);
    }
}

/* Sets up the CPU for running user code in the current
   thread.
   This function is called on every context switch. */
void
process_activate (void)
{
  struct thread *t = thread_current ();

  /* Activate thread's page tables. */
  pagedir_activate (t->pagedir);

  /* Set thread's kernel stack for use in processing
     interrupts. */
  tss_update ();
}

/* We load ELF binaries.  The following definitions are taken
   from the ELF specification, [ELF1], more-or-less verbatim.  */

/* ELF types.  See [ELF1] 1-2. */
typedef uint32_t Elf32_Word, Elf32_Addr, Elf32_Off;
typedef uint16_t Elf32_Half;

/* For use with ELF types in printf(). */
#define PE32Wx PRIx32   /* Print Elf32_Word in hexadecimal. */
#define PE32Ax PRIx32   /* Print Elf32_Addr in hexadecimal. */
#define PE32Ox PRIx32   /* Print Elf32_Off in hexadecimal. */
#define PE32Hx PRIx16   /* Print Elf32_Half in hexadecimal. */

/* Executable header.  See [ELF1] 1-4 to 1-8.
   This appears at the very beginning of an ELF binary. */
struct Elf32_Ehdr
  {
    unsigned char e_ident[16];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
  };

/* Program header.  See [ELF1] 2-2 to 2-4.
   There are e_phnum of these, starting at file offset e_phoff
   (see [ELF1] 1-6). */
struct Elf32_Phdr
  {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
  };

/* Values for p_type.  See [ELF1] 2-3. */
#define PT_NULL    0            /* Ignore. */
#define PT_LOAD    1            /* Loadable segment. */
#define PT_DYNAMIC 2            /* Dynamic linking info. */
#define PT_INTERP  3            /* Name of dynamic loader. */
#define PT_NOTE    4            /* Auxiliary info. */
#define PT_SHLIB   5            /* Reserved. */
#define PT_PHDR    6            /* Program header table. */
#define PT_STACK   0x6474e551   /* Stack segment. */

/* Flags for p_flags.  See [ELF3] 2-3 and 2-4. */
#define PF_X 1          /* Executable. */
#define PF_W 2          /* Writable. */
#define PF_R 4          /* Readable. */

static bool setup_stack (void **esp);
static bool validate_segment (const struct Elf32_Phdr *, struct file *);
static bool load_segment (struct file *file, off_t ofs, uint8_t *upage,
                          uint32_t read_bytes, uint32_t zero_bytes,
                          bool writable);

/* Loads an ELF executable from FILE_NAME into the current thread.
   Stores the executable's entry point into *EIP
   and its initial stack pointer into *ESP.
   Returns true if successful, false otherwise. */

bool
load (const char *file_name, void (**eip) (void), void **esp) 
{
  struct thread *t = thread_current ();
  struct Elf32_Ehdr ehdr;
  struct file *file = NULL;
  off_t file_ofset;
  bool success = false;
  int i;

  /* Allocate and activate page directory. */
  t->pagedir = pagedir_create ();
  if (t->pagedir == NULL) 
    goto done;
  process_activate ();


  /* Open executable file. */
  
  char *tmp_fname = (char*)malloc(sizeof(char) * 50);
   int j = 0;
  int argc = 0;
  i = 0;
  while(true)
  {
	  if(file_name[i] == ' ')
		  i++;
	  else if(file_name[i] == '\0')
		  break;
	  else
	  {
		  while(file_name[i] != ' ' && file_name[i] != '\0')
			  tmp_fname[j++] = file_name[i++];
		  tmp_fname[j++] = '\0';
		  argc++;
	  }
  }
  
  file = filesys_open (tmp_fname);
  if (file == NULL) 
  {
      printf ("load: %s: open failed\n", tmp_fname);
      goto done; 
  }

	file_deny_write(file);

  /* Read and verify executable header. */
  if (file_read (file, &ehdr, sizeof ehdr) != sizeof ehdr
      || memcmp (ehdr.e_ident, "\177ELF\1\1\1", 7)
      || ehdr.e_type != 2
      || ehdr.e_machine != 3
      || ehdr.e_version != 1
      || ehdr.e_phentsize != sizeof (struct Elf32_Phdr)
      || ehdr.e_phnum > 1024) 
    {
      printf ("load: %s: error loading executable\n",tmp_fname);
      goto done;
    }

  /* Read program headers. */
  file_ofset = ehdr.e_phoff;
  for (i = 0; i < ehdr.e_phnum; i++) 
    {
      struct Elf32_Phdr phdr;

      if (file_ofset < 0 || file_ofset > file_length (file))
        goto done;
      file_seek (file, file_ofset);

      if (file_read (file, &phdr, sizeof phdr) != sizeof phdr)
        goto done;
      file_ofset += sizeof phdr;
      switch (phdr.p_type) 
        {
        case PT_NULL:
        case PT_NOTE:
        case PT_PHDR:
        case PT_STACK:
        default:
          /* Ignore this segment. */
          break;
        case PT_DYNAMIC:
        case PT_INTERP:
        case PT_SHLIB:
          goto done;
        case PT_LOAD:
          if (validate_segment (&phdr, file)) 
            {
              bool writable = (phdr.p_flags & PF_W) != 0;
              uint32_t file_page = phdr.p_offset & ~PGMASK;
              uint32_t mem_page = phdr.p_vaddr & ~PGMASK;
              uint32_t page_offset = phdr.p_vaddr & PGMASK;
              uint32_t read_bytes, zero_bytes;
              if (phdr.p_filesz > 0)
                {
                  /* Normal segment.
                     Read initial part from disk and zero the rest. */
                  read_bytes = page_offset + phdr.p_filesz;
                  zero_bytes = (ROUND_UP (page_offset + phdr.p_memsz, PGSIZE)
                                - read_bytes);
                }
              else 
                {
                  /* Entirely zero.
                     Don't read anything from disk. */
                  read_bytes = 0;
                  zero_bytes = ROUND_UP (page_offset + phdr.p_memsz, PGSIZE);
                }
              if (!load_segment (file, file_page, (void *) mem_page,
                                 read_bytes, zero_bytes, writable))
                goto done;
            }
          else
            goto done;
          break;
        }
    }

  /* Set up stack. */
  if (!setup_stack (esp))
    goto done;

  argument_passing(tmp_fname,esp,argc, j);

  /* Start address. */
  *eip = (void (*) (void)) ehdr.e_entry;

  success = true;

 done:
  /* We arrive here whether the load is successful or not. */
 	free(tmp_fname);

   thread_current()->executable_file = file;
  return success;
}

/* load() helpers. */

static bool install_page (void *upage, void *kpage, bool writable);

/* Checks whether PHDR describes a valid, loadable segment in
   FILE and returns true if so, false otherwise. */
static bool
validate_segment (const struct Elf32_Phdr *phdr, struct file *file) 
{
  /* p_offset and p_vaddr must have the same page offset. */
  if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK)) 
    return false; 

  /* p_offset must point within FILE. */
  if (phdr->p_offset > (Elf32_Off) file_length (file)) 
    return false;

  /* p_memsz must be at least as big as p_filesz. */
  if (phdr->p_memsz < phdr->p_filesz) 
    return false; 

  /* The segment must not be empty. */
  if (phdr->p_memsz == 0)
    return false;
  
  /* The virtual memory region must both start and end within the
     user address space range. */
  if (!is_user_vaddr ((void *) phdr->p_vaddr))
    return false;
  if (!is_user_vaddr ((void *) (phdr->p_vaddr + phdr->p_memsz)))
    return false;

  /* The region cannot "wrap around" across the kernel virtual
     address space. */
  if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
    return false;

  /* Disallow mapping page 0.
     Not only is it a bad idea to map page 0, but if we allowed
     it then user code that passed a null pointer to system calls
     could quite likely panic the kernel by way of null pointer
     assertions in memcpy(), etc. */
  if (phdr->p_vaddr < PGSIZE)
    return false;

  /* It's okay. */
  return true;
}

/* Loads a segment starting at offset OFS in FILE at address
   UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
   memory are initialized, as follows:

        - READ_BYTES bytes at UPAGE must be read from FILE
          starting at offset OFS.

        - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.

   The pages initialized by this function must be writable by the
   user process if WRITABLE is true, read-only otherwise.

   Return true if successful, false if a memory allocation error
   or disk read error occurs. */
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
              uint32_t read_bytes, uint32_t zero_bytes, bool writable) 
{
  ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
  ASSERT (pg_ofs (upage) == 0);
  ASSERT (ofs % PGSIZE == 0);

  file_seek (file, ofs);
  while (read_bytes > 0 || zero_bytes > 0) 
    {
      /* Calculate how to fill this page.
         We will read PAGE_READ_BYTES bytes from FILE
         and zero the final PAGE_ZERO_BYTES bytes. */
      size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
      size_t page_zero_bytes = PGSIZE - page_read_bytes;

      struct vm_entry *new_entry = (struct vm_entry*)malloc(sizeof(struct vm_entry));
      new_entry->type = VM_BIN;
      new_entry->vaddr = upage;
      new_entry->writable = writable;

      new_entry->pinned = false;
      new_entry->is_loaded = false;
      new_entry->file = file_reopen(file);

      new_entry->offset = ofs;
      new_entry->read_bytes = page_read_bytes;
      new_entry->zero_bytes = page_zero_bytes;
      
      insert_vme(&thread_current()->vm, new_entry);
      // -------------------------
      //         ORIGINAL
      // -------------------------
      /*
      // Get a page of memory.
      uint8_t *knpage = palloc_get_page (PAL_USER);
      if (knpage == NULL)
        return false;

      // Load this page.
      if (file_read (file, knpage, page_read_bytes) != (int) page_read_bytes)
        {
          palloc_free_page (knpage);
          return false; 
        }
      memset (knpage + page_read_bytes, 0, page_zero_bytes);

      // Add the page to the process's address space.
      if (!install_page (upage, knpage, writable)) 
        {
          palloc_free_page (knpage);
          return false; 
        }
      */

      /* Advance. */
      read_bytes -= page_read_bytes;
      zero_bytes -= page_zero_bytes;
      ofs += page_read_bytes;
      upage += PGSIZE;
    }
  return true;
}

/* Create a minimal stack by mapping a zeroed page at the top of
   user virtual memory. */
static bool
setup_stack (void **esp) 
{
  uint8_t *kpage;
  bool success = false;

  kpage = palloc_get_page (PAL_USER | PAL_ZERO);
  if (kpage != NULL) 
    {
      success = install_page (((uint8_t *) PHYS_BASE) - PGSIZE, kpage, true);
      if (success)
        *esp = PHYS_BASE;
      else
        palloc_free_page (kpage);
    }

  struct vm_entry *new_entry = (struct vm_entry*)malloc(sizeof(struct vm_entry));
  new_entry->type = VM_ANON;
  new_entry->vaddr = pg_round_down(((uint8_t *) PHYS_BASE) - PGSIZE);
  new_entry->writable = true;

  new_entry->pinned = true;
  new_entry->is_loaded = true;
  //kpage->new_entry = new_etnry;
  success = insert_vme(&thread_current()->vm, new_entry);

  return success;
}

/* Adds a mapping from user virtual address UPAGE to kernel
   virtual address KPAGE to the page table.
   If WRITABLE is true, the user process may modify the page;
   otherwise, it is read-only.
   UPAGE must not already be mapped.
   KPAGE should probably be a page obtained from the user pool
   with palloc_get_page().
   Returns true on success, false if UPAGE is already mapped or
   if memory allocation fails. */
static bool
install_page (void *upage, void *kpage, bool writable)
{
  struct thread *th = thread_current ();

  /* Verify that there's not already a page at that virtual
     address, then map our page there. */
  return (pagedir_get_page (th->pagedir, upage) == NULL
          && pagedir_set_page (th->pagedir, upage, kpage, writable));
}


bool handle_mm_fault(struct vm_entry* vme){
    bool success;

    void* kaddr = palloc_get_page(0);
    switch(vme->type){
      case VM_BIN:
        if(load_file(kaddr, vme) == false){
          palloc_free_page(kaddr);
          return false;
        }
        break;
      case VM_FILE:
        
        break;
      case VM_ANON:

        break;
    }
    if(install_page(vme->vaddr, kaddr, vme->writable) == false){
      palloc_free_page(kaddr);
      return false;
    }
    vme->is_loaded = true;
    return true;
}