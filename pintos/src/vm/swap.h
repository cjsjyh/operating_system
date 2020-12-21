#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <stddef.h>

#define SECTORS_PER_PAGE 8
#define SWAP_FREE 0
#define SWAP_USED 1

void swap_init(void);
size_t swap_out(void* kaddr);
void swap_in(size_t used_index, void* kaddr);

#endif