#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <list.h>
#include "threads/thread.h"
#include "threads/palloc.h"

struct page {
    void *kaddr; // page physical addr
    struct vm_entry *vme; // 물리 페이지가 매핑된 가상 주소의 vm_entry
    struct thread *thread; // 해당 물리 페이지를 사용중인 쓰레드의 포인터
    struct list_elem lru; // list 연결을 위한 필드
};

struct page* alloc_page(enum palloc_flags flags);
void free_page(void* kaddr);
void __free_page(struct page* page);
void try_to_free_pages(void);

void lru_list_init(void);
void add_page_to_lru_list(struct page* page);
void del_page_from_lru_list(struct page* page);

struct list_elem* get_next_lru_clock(void);

#endif