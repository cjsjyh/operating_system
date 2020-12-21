#include "vm/swap.h"
#include "vm/page.h"
#include "vm/frame.h"

struct lock swap_lock;
struct bitmap *swap_map;
struct block *swap_block;

void swap_init(void){

}

size_t swap_out(void* kaddr){

}

void swap_in(size_t used_index, void* kaddr){

}