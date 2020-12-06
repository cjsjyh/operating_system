#include "page.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

void vm_init(struct hash *vm)
{
    hash_init(vm, vm_hash_func, vm_less_func, NULL);
/* hash_init()으로해시테이블초기화*/
/* 인자로해시테이블과vm_hash_func과vm_less_func사용*/
}

static unsigned vm_hash_func(const struct hash_elem *e, void* aux)
{
    /* hash_entry()로element에대한vm_entry구조체검색*/
    /* hash_int()를이용해서vm_entry의멤버vaddr에대한해시값을구하고반환*/
    return hash_int((hash_entry(e, struct vm_entry, elem))->vaddr);
}

static bool vm_less_func(const struct hash_elem *a, const struct hash_elem *b)
{
    /* hash_entry()로각각의element에대한vm_entry구조체를얻은후vaddr비교(b가크다면true, a가 크다면 false)*/
    return hash_entry(a, struct vm_entry, elem))->vaddr < (hash_entry(a, struct vm_entry, elem))->vaddr;
}

bool insert_vme(struct hash *vm, struct vm_entry *vme)
{
    /*hash_insert()함수사용*/
    struct hash_elem *temp = hash_insert(vm, &(vme->elem));
    if (temp == NULL) 
        return true;
    return false;
}

bool delete_vme(struct hash *vm, struct vm_entry *vme)
{
    /* hash_delete()함수사용*/
    struct hash_elem *temp = hash_delete(vm, &(vme->elem));
    if (temp == NULL)
        return false;
    // hash_delete doesn't deallocate
    free(temp);
    return true;
}

struct vm_entry* find_vme(void* vaddr)
{
    /* pg_round_down()으로vaddr의페이지번호를얻음*/
    struct vm_entry temp_vme;
    temp_vme.vaddr = pg_round_down(vaddr);
    /* hash_find() 함수를사용해서hash_elem구조체얻음*/
    struct hash_elem *temp = hash_find(&thread_current()->vm, &temp_vme.elem);
    /* 만약존재하지않는다면NULL리턴*/
    if (temp == NULL)
        return NULL;
    /* hash_entry()로해당hash_elem의vm_entry구조체리턴*/
    return hash_entry(&thread_current()->vm,struct vm_entry, elem);
}

void vm_destroy(struct hash *vm)
{
/* hash_destroy()으로해시테이블의버킷리스트와vm_entry들을제거*/
    hash_destroy(vm, vm_destructor)
}

void vm_destructor(struct hash_elem *e, void *aux UNUSED){
    struct vm_entry *vme = hash_entry(e, struct vm_entry, elem);
    if (vme->is_loaded == true){
        void* phys_addr = pagedir_get_page(thread_current()->pagedir, vme->vaddr);
        free_page(phys_addr);
        pagedir_clear_page(thread_current()->pagedir, vme->vaddr);
    }
    free(vme);
}