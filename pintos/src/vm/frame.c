#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "threads/synch.h"
#include "filesys/file.h"
#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include <threads/malloc.h>

struct lock lru_list_lock;
struct list lru_list;
struct page *lru_clock;

void lru_list_init(){
    lock_init(&lru_list_lock);
    list_init(&lru_list);
    lru_clock = NULL;
}
void add_page_to_lru_list(struct page* page){
    if (page == NULL)
        return;
    lock_acquire(&lru_list_lock);
    list_push_back(&lru_list, &page->lru);
    lock_release(&lru_list_lock);
}
void del_page_from_lru_list(struct page* page){
    if (page == NULL)
        return;
    lock_acquire(&lru_list_lock);
    // if user is to remove current victim, replace with next
    if (lru_clock == page){
        struct list_elem *next = list_next(&page->lru);
        lru_clock = list_entry(next, struct page, lru);
    }
    list_remove(&page->lru);
    lock_release(&lru_list_lock);
}

struct page* alloc_page(enum palloc_flags flags){
    if((flags & PAL_USER) == 0)
        return NULL;
    void* kaddr = palloc_get_page(flags);
    while(kaddr == NULL){
        try_to_free_pages();
        kaddr = palloc_get_page(flags);
    }
    struct page* new_page = (struct page*)malloc(sizeof(struct page));
    if (new_page == NULL){
        palloc_free_page(kaddr);
        return NULL;
    }
    new_page->kaddr = kaddr;
    new_page->thread = thread_current();
    add_page_to_lru_list(new_page);
    return new_page;
}

// kaddr로 page를 free함
void free_page(void* kaddr){
    struct page *lru_page;
    lock_acquire(&lru_list_lock);
    for(struct list_elem *e = list_begin(&lru_list); e != list_end(&lru_list); e = list_next(e)){
        lru_page = list_entry(e, struct page, lru);
        if(lru_page->kaddr == kaddr){
            __free_page(lru_page);
            break;
        }        
    }
    lock_release(&lru_list_lock);
}

// page로 palloc_free_page를 호출
void __free_page(struct page* page){
    palloc_free_page(page->kaddr);
    del_page_from_lru_list(page);
    free(page);
}

// palloc_get_page()가 실패했을때 여유공간확보하기
void try_to_free_pages(void){
    	struct thread *page_thread;
	struct list_elem *element;
	struct page *lru_page;
	lock_acquire(&lru_list_lock);
	if(list_empty(&lru_list) == true)
	{
		lock_release(&lru_list_lock);
		return;
	}
	while(true)
	{
		/* get next element */
		element = get_next_lru_clock();
		if(element == NULL){
			lock_release(&lru_list_lock);
			return;
		}
		lru_page = list_entry(element, struct page, lru);
		if(lru_page->vme->pinned == true)
			continue;
		page_thread = lru_page->thread;
		/* if page address is accessed, set accessed bit 0 and continue(it's not victim) */
		if(pagedir_is_accessed(page_thread->pagedir, lru_page->vme->vaddr))
		{
			pagedir_set_accessed(page_thread->pagedir, lru_page->vme->vaddr, false);
			continue;
		}
		/* if not accessed, it's victim */
		/* if page is dirty */
		if(pagedir_is_dirty(page_thread->pagedir, lru_page->vme->vaddr) || lru_page->vme->type == VM_ANON)
		{
			/* if vm_entry is mmap file, don't call swap out.*/
			if(lru_page->vme->type == VM_FILE)
			{
                sema_down(&filesys_mutex);
				file_write_at(lru_page->vme->file, lru_page->kaddr ,lru_page->vme->read_bytes, lru_page->vme->offset);
				sema_up(&filesys_mutex);
			}
			/* if not mmap_file, change type to ANON and call swap_out function */
			else
			{
				lru_page->vme->type = VM_ANON;
				lru_page->vme->swap_slot = swap_out(lru_page->kaddr);
 			}
		}
		lru_page->vme->is_loaded = false;
		pagedir_clear_page(page_thread->pagedir, lru_page->vme->vaddr);
		__free_page(lru_page);
		break;
	}
    lock_release(&lru_list_lock);
	return;
}

// 다음으로 victim 포인터인 lru_clock을 옮김
struct list_elem* get_next_lru_clock(void){
    struct list_elem *e;
    if(lru_clock == NULL)
        e = list_begin(&lru_list);
    else {
        // 다음으로 옮겼는데
        e = list_next(&lru_clock->lru);
        // 끝이면
        if (e == list_end(&lru_list))
            e = list_begin(&lru_list);
    }

    // list가 비어있음
    if (e == list_end(&lru_list))
        return NULL;
    lru_clock = list_entry(e, struct page, lru);
    return e;
}