#ifndef THREADS_PTE_H
#define THREADS_PTE_H

#include <list.h>
#include <hash.h>

//vm_entry->type에 들어감
#define VM_BIN 0 // 바이너리 파일로부터 데이터를 로드
#define VM_FILE 1 // 매핑된 파일로부터 데이터를 로드
#define VM_ANON 2 // 스왑영역으로부터 데이터를 로드

struct vm_entry{
    uint8_t type; /* VM_BIN, VM_FILE, VM_ANON의타입*/
    void* vaddr; /* vm_entry가관리하는가상페이지번호*/
    bool writable; /* True일경우해당주소에write 가능

    False일경우해당주소에write 불가능*/
    bool is_loaded; /* 물리메모리의탑재여부를알려주는플래그*/
    struct file* file;/* 가상주소와맵핑된파일*/

    /* Memory Mapped File 에서다룰예정*/
    struct list_elem mmap_elem; /* mmap리스트element */
    size_t offset;/* 읽어야할파일오프셋*/
    size_t read_bytes;/* 가상페이지에쓰여져있는데이터크기*/
    size_t zero_bytes;/* 0으로채울남은페이지의바이트*/

    /* Swapping 과제에서다룰예정*/
    size_t swap_slot; /* 스왑슬롯*/

    /* ‘vm_entry들을위한자료구조’ 부분에서다룰예정*/
    struct hash_elem elem; /*해시테이블Element */
}

void vm_init(struct hash *vm);
static unsigned vm_hash_func(const struct hash_elem *e, void *aux);
static bool vm_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux);

#endif /* vm/page.h */